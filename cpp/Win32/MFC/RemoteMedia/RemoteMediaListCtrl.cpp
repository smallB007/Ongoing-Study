// RemoteMediaListCtrl.cpp : implementation file
//
#include "stdafx.h"

#ifdef REMOTE_MEDIA_SERVER
#  include "RemoteMediaServer/resource.h"
#elif defined(REMOTE_MEDIA_CLIENT)
#  include "RemoteMediaClient/resource.h"
#else
#  error Definition of REMOTE_MEDIA_SERVER or REMOTE_MEDIA_CLIENT is required. 
#endif

#include "RemoteMediaListCtrl.h"
#include "RemoteMediaUtility.h"

#include <clocale>

// ͨ���ļ�����ȡ��Ӧ��Guid
class _CAtrrFindHelper
{
public:
    _CAtrrFindHelper(const CString &strFileName)
    {
        m_strFileName = strFileName;
    }
    bool operator()(pugi::xml_node node) const
    {
        return (::lstrcmpi(m_strFileName, 
            (LPCTSTR)(node.attribute(_T("Name")).as_string())) 
            == 0);
    }
private:
    CString m_strFileName;
};

// CRemoteMediaListCtrl

IMPLEMENT_DYNAMIC(CRemoteMediaListCtrl, CMFCListCtrl)

CRemoteMediaListCtrl::CRemoteMediaListCtrl()
{
    VERIFY(_tsetlocale(LC_ALL, _T("")) != NULL);
}

CRemoteMediaListCtrl::~CRemoteMediaListCtrl()
{
}


void CRemoteMediaListCtrl::AddFileToMap(const CString &strFileName, 
    const CString &strFileSize)
{
    m_mapFiles[strFileName] = strFileSize;
}

void CRemoteMediaListCtrl::DeleteSelectedItems()
{
    ASSERT(GetSafeHwnd() != NULL);
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    if (GetFirstSelectedItemPosition() == NULL)
    {
        return;
    }

    TRY 
    {
        for (int i = -1; (i = GetNextItem(i, LVNI_SELECTED))
            != -1;)
        {
            m_mapFiles.RemoveKey(GetItemText(i, 0));
        }
    }
    CATCH_ALL (e) 
    {
    }
    END_CATCH_ALL

    UpdateContents();
    SavePlayList();
}

void CRemoteMediaListCtrl::UpdateContents()
{
    ASSERT(GetSafeHwnd() != NULL);
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CWaitCursor waitCur;

    DeleteAllItems();

    POSITION p = m_mapFiles.GetStartPosition();
    CString strKey;
    CString strVal;
    for (; p != NULL;)
    {
        m_mapFiles.GetNextAssoc(p, strKey, strVal);
        InsertItem(0, strKey);
    }

    TRY
    {
        int iItemCnt = GetItemCount();
        CString strFileSize;
        for (int i = 0; i < iItemCnt; ++i)
        {
            m_mapFiles.Lookup(GetItemText(i, 0), strFileSize);
            SetItemText(i, 1, strFileSize);
        }
    }
    CATCH_ALL(e)
    {
    }
    END_CATCH_ALL

    Sort(GetSortedColumn(), IsAscending());
}

CString &CRemoteMediaListCtrl::GetFileGUID(CString &strGUID, 
    const CString &strFileName)
{
    strGUID.Empty();

    _CAtrrFindHelper attrFindHelper(strFileName);

    pugi::xml_document xmlPlaylist;
    pugi::xml_parse_result parseRet = 
        xmlPlaylist.load_file(DEFAULT_PLAYLIST_FILENAME);
    if (parseRet)
    {
        pugi::xml_node nodeParent = xmlPlaylist.child(_T("RemoteMediaPlaylist"));
        if (nodeParent)
        {
            pugi::xml_node nodeFind = nodeParent.find_child(attrFindHelper);
            if (nodeFind)
            {
                pugi::xml_attribute attrFileName = nodeFind.attribute(_T("Guid"));
                if (attrFileName)
                {
                    strGUID = (LPCTSTR)attrFileName.as_string();
                }
            }
        }
    }

    return strGUID;
}

void CRemoteMediaListCtrl::InitUI()
{
    ASSERT(GetSafeHwnd() != NULL);
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    DWORD dwFileNameColWid = AfxGetApp()->GetProfileInt(_T("UIPreference"), 
        _T("FileNameColumnWidth"), 400);
    CRect rtHeaderCtrl;
    GetHeaderCtrl().GetClientRect(&rtHeaderCtrl);
    DWORD dwHeaderCtrlWid = rtHeaderCtrl.Width();
    if (dwFileNameColWid == 0 || dwFileNameColWid > dwHeaderCtrlWid)
    {
        dwFileNameColWid = dwHeaderCtrlWid >> 1;
    }
    InsertColumn(0, RES_STR(IDS_FILENAME), LVCFMT_LEFT, dwFileNameColWid);
    InsertColumn(1, RES_STR(IDS_FILESIZE), LVCFMT_LEFT, 
        AfxGetApp()->GetProfileInt(_T("UIPreference"), 
        _T("FileSizeColumnWidth"), dwHeaderCtrlWid - dwFileNameColWid));
    // TODO �����һ�п����400 �ڶ��п����300 ��header control�Ŀ����600�Ļ�
    // ��ʹ400��300������ֵ�Ѵ���ע����ٴ���������ʱ��ˮƽ������Ҳ�����Զ����֡�
    // �����������Win7 64λ�ϻ���֣���WinXP 32λ��δ���֣�
    // ���ң���WinXP 32λ��ʹ��CDialogEx��Ϊ���࣬��ʱ�������������ɫ���ܷ�ӳ
    // ��ǰ�����Ƿ񼤻�

    m_iSortedColumn = AfxGetApp()->GetProfileInt(_T("UIPreference"), 
        _T("SortedColumn"), 0);
    m_bAscending = (BOOL)(AfxGetApp()->GetProfileInt(_T("UIPreference"), 
        _T("IsAscending"), TRUE));
}

void CRemoteMediaListCtrl::SaveUILayout()
{
    ASSERT(GetSafeHwnd() != NULL);
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    AfxGetApp()->WriteProfileInt(_T("UIPreference"), _T("FileNameColumnWidth"), 
        GetColumnWidth(0));
    AfxGetApp()->WriteProfileInt(_T("UIPreference"), _T("FileSizeColumnWidth"), 
        GetColumnWidth(1));
    AfxGetApp()->WriteProfileInt(_T("UIPreference"), _T("SortedColumn"), 
        m_iSortedColumn);
    AfxGetApp()->WriteProfileInt(_T("UIPreference"), _T("IsAscending"), 
        (int)m_bAscending);
}

BOOL CRemoteMediaListCtrl::LoadPlayList(/* LPCTSTR lpcszPlaylist */)
{
    m_mapFiles.RemoveAll();

    pugi::xml_document xmlPlaylist;
    // pugi::xml_parse_result parseRet = xmlPlaylist.load_file(lpcszPlaylist);
    pugi::xml_parse_result parseRet = 
        xmlPlaylist.load_file(DEFAULT_PLAYLIST_FILENAME);
    if (!parseRet)
    {
        return FALSE;
    }

    pugi::xml_node nodeParent = xmlPlaylist.child(_T("RemoteMediaPlaylist"));
    if (!nodeParent)
    {
        return FALSE;
    }

    for (pugi::xml_node_iterator nit = nodeParent.begin(); 
        nit != nodeParent.end(); ++nit)
    {
        pugi::xml_attribute attrFile = nit->attribute(_T("Name"));
        pugi::xml_attribute attrSize = nit->attribute(_T("Size"));

        if (attrFile && attrSize)
        {
            m_mapFiles[attrFile.value()] = attrSize.value();
        }
    }

    UpdateContents();
    return TRUE;
}

BOOL CRemoteMediaListCtrl::SavePlayList(/* LPCTSTR lpcszPlaylist */)
{
    CString strFormat(MAKEINTRESOURCE(IDS_PLSCOMMENT));
    CString strComment;
    CString strDateTime;
    strComment.Format(LPCTSTR(strFormat), cmGetCurDateTime(strDateTime));

    pugi::xml_document xmlPlaylist;
    pugi::xml_node     nodeComment;

    // д��ע����Ϣ
    nodeComment = xmlPlaylist.append_child(pugi::node_comment);
    nodeComment.set_value(reinterpret_cast<const pugi::char_t *>(
        LPCTSTR(strComment)));
    
    // ��Ӷ���Ԫ��
    pugi::xml_node nodeParent = xmlPlaylist.append_child(_T("RemoteMediaPlaylist"));
    if (!nodeParent)
    {
        return FALSE;
    }

    // �����
    // TODO: ���ֵ�����ʽ��UpdateContents���õ��ĵ�����ʽЧ�����в�����
    // CMap::InitHashTable��ô�ã�
    CString strGUID;

    for (cmRMPlaylist::CPair *pElem = m_mapFiles.PGetFirstAssoc(); 
        pElem != NULL; pElem = m_mapFiles.PGetNextAssoc(pElem))
    {
        pugi::xml_node nodeItem = nodeParent.append_child(_T("File"));
        if (nodeItem)
        {
            nodeItem.append_attribute(_T("Name")) = pElem->key;
            nodeItem.append_attribute(_T("Size")) = pElem->value;
            nodeItem.append_attribute(_T("Guid")) = cmGenerateGUID(strGUID);
        }
    }

    // д��XML�ļ�
    // return (BOOL)(xmlPlaylist.save_file(lpcszPlaylist) != false);
    return (BOOL)(xmlPlaylist.save_file(DEFAULT_PLAYLIST_FILENAME) != false);
}

BEGIN_MESSAGE_MAP(CRemoteMediaListCtrl, CMFCListCtrl)
END_MESSAGE_MAP()

void CRemoteMediaListCtrl::Sort(int iColumn, BOOL bAscending, BOOL bAdd)
{
    int iItemCnt = GetItemCount();
    for (int i = 0; i < iItemCnt; ++i)
    {
        SetItemData(i, i);
    }
    CMFCListCtrl::Sort(iColumn, bAscending, bAdd);
}

// MFC��������������麯����
// The framework calls this method when it compares two items. 
// The default implementation always returns 0. 
// You must override this function to provide a sorting algorithm. 
// �������Ĺ��̣�
// OnColumnClick->Sort->SortItems->CompareProc��һ���ص�������->OnCompareItems
// ��Ҫ�Զ���������ԣ�ֻoverride������������С�
// ��list view�ĸ���item����ʱͨ����list view����LVM_SORTITEMS��Ϣ��ʵ�ֵ�
// ��ʵ���ϣ�SortItems�����������������ģ���
// ��MSDN��LVITEM Structure��lParam��������������
// Value specific to the item. If you use the LVM_SORTITEMS message, 
// the list-view control passes this value to the application-defined 
// comparison function. You can also use the LVM_FINDITEM message to 
// search a list-view control for an item with a specified lParam value. 
// ��MFC����ǿ���ͨ������SetItemData�������������lParam��
// ���������SetItemData�ͱȽ������ˣ����磬���ǿ���overrideһ��
// Sort��������������SetItemData��
// ���ﲻ�ò��²�һ��MFC�ˣ�
// �ͱ�������ԣ��������е�list view control�����У�һ�б�ʾ�ļ�����
// ��һ�б�ʾ�ļ���С�����ļ�����һ�н�������ʱ����ʹ����ͨ���ַ����ȽϺ��������ˡ�
// ���Ƕ����ļ���С��������ʹ�õ������磺8M��1.0G��300KB������readable��ʽ��
// ���԰��ļ���С��������ʱ���ұ������¶���CompareProc������������������ˡ�
// ����Ȼ����ȻCRemoteMediaListCtrl����ػ����ػ���ÿһ�е�������Զ��ܾ��壩��
// ���Һβ���InsertColumn��InsertItem��Щ��������װ��CRemoteMediaListCtrl�����أ�
// �ź����ǣ���̫���ܡ�����MFC����ν�ġ����ι��족����ʹ��������һ��
// CRemoteMediaListCtrl���͵�object������Ҳ�޷���֤���Ѱ󶨵�һ��list view control
// ���ˣ������ϵ���Insert*���޴�̸�����������InsertColumn����Ĳ�������û�취
// ������װ��CRemoteMediaListCtrl�С��ͱ�������ԣ�������CRemoteMediaServerDlg
// �����InsertColumn��InsertItem��Щ�����ġ�
// ���磬��CRemoteMediaServerDlg::InitDialog����ʹ��������䣺
// m_ctrlPlaylist.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
// ������list view control�ķ�񣬶������������װ��CRemoteMediaListCtrl����ڲ�Ҳ
// ���ѡ�MFC�еġ����ι��족������������ǣ���ʹ����һ���򵥵�����֮ǰ��
// ��Ҳ���ò�����һЩ���ѿ��Ƶġ�ǰ�ᡱ�����磬����������
// CListCtrl::SetExtendedStyleʱ������һ��CListCtrl���͵Ķ����⣬�һ��ÿ������Ƿ�
// �󶨵���һ����Ч��list view control�ϣ�ͨ��Create��̬����Ҳ�ã�����ͨ��
// add variableʹ��ͶԻ�����Դģ����һ�����е�list view control����һ��Ҳ�ã���
// ��ʵ���ǲ�ˬ��
// ��ʹ�Ƿ�װ�ˣ��������Ѿ���CRemoteMediaListCtrl��������������Ч��Ҳ��ǿ���⣬������
// ��װ�����ĸ����ԡ�
//
// 2014-07-18 
// ����MSDN������ON_NOTIFY���Ķ���
// TN062: Message Reflection for Windows Controls 
int CRemoteMediaListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
    switch (iColumn)
    {
    case 0: 
        return _tcscoll(GetItemText(lParam1, iColumn), 
            GetItemText(lParam2, iColumn));
    case 1:
        return cmCompareReadableFileSize(GetItemText(lParam1, iColumn), 
            GetItemText(lParam2, iColumn));
    default: 
        return CMFCListCtrl::OnCompareItems(lParam1, lParam2, iColumn);
    }
}

