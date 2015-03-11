
// RemoteMediaServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteMediaServer.h"
#include "RemoteMediaServerDlg.h"
#include "RMSConnect.h"

#include <afxdialogex.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteMediaServerDlg �Ի���

CRemoteMediaServerDlg::CRemoteMediaServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRemoteMediaServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteMediaServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LST_PLAYLIST, m_ctrlPlaylist);
    DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
    DDX_Control(pDX, IDC_BTN_REMOVE, m_btnRemove);
    DDX_Control(pDX, IDOK, m_btnOK);
    DDX_Control(pDX, IDC_BTN_HELP, m_btnHelp);
}

BEGIN_MESSAGE_MAP(CRemoteMediaServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_ADD, &CRemoteMediaServerDlg::OnBnClickedBtnAdd)
    ON_BN_CLICKED(IDC_BTN_REMOVE, &CRemoteMediaServerDlg::OnBnClickedBtnRemove)
    ON_BN_CLICKED(IDC_BTN_HELP, &CRemoteMediaServerDlg::OnBnClickedBtnHelp)
    ON_WM_DESTROY()
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CRemoteMediaServerDlg ��Ϣ�������

BOOL CRemoteMediaServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    m_ctrlPlaylist.InitUI();
    m_ctrlPlaylist.LoadPlayList();
    InitUI();

    if (m_menuAdd.GetSafeHmenu() == NULL)
    {
        m_menuAdd.LoadMenu(IDM_ADD);
    }

    cmVLPLogDump(RES_STR(IDS_LOG_LIVE555_STARTING));
    if (!m_live555.Start())
    {
        cmVLPLogDump(RES_STR(IDS_LOG_LIVE555_START_FAILED));
        SendMessage(WM_CLOSE);
    }

    cmVLPLogDump(RES_STR(IDS_LOG_SOCKET_INITIALIZING));
    if (!AfxSocketInit())
    {
        cmVLPLogDump(RES_STR(IDS_LOG_SOCKET_INITIALIZING));
        cmVLPLogDumpError();
        SendMessage(WM_CLOSE);
        return TRUE;
    }

    // ��������socket
    cmVLPLogDump(RES_STR(IDS_LOG_LISTENING_SOCKET_CREATING));
    DWORD dwPortNum;
    if ((dwPortNum = DEF_PORT_NUM, !m_socketListen.Create(DEF_PORT_NUM))
        && (dwPortNum = REV_PORT_NUM, !m_socketListen.Create(REV_PORT_NUM)))
    {
        cmVLPLogDump(RES_STR(IDS_LOG_LISTENING_SOCKET_CREATE_FAILED));
        cmVLPLogDumpError();
    }
    else
    {
        cmVLPLogDump(RES_STR(IDS_LOG_LISTENING_SOCKET_CREATED), 
            &m_socketListen, dwPortNum);
        m_socketListen.Listen();
    }
    m_socketListen.BoundToUI(this);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRemoteMediaServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRemoteMediaServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRemoteMediaServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRemoteMediaServerDlg::InitUI()
{
    if (m_icoAdd.Load(IDI_ICO_ADD)) { m_btnAdd.SetIcon(m_icoAdd); }
    else { m_btnAdd.SetWindowText(RES_STR(IDS_ADD)); }

    if (m_icoRemove.Load(IDI_ICO_REMOVE)) { m_btnRemove.SetIcon(m_icoRemove); }
    else { m_btnRemove.SetWindowText(RES_STR(IDS_REMOVE)); }

    if (m_icoHelp.Load(IDI_ICO_HELP)) { m_btnHelp.SetIcon(m_icoHelp); }
    else { m_btnHelp.SetWindowText(RES_STR(IDS_ABOUT)); }

    if (m_icoOK.Load(IDI_ICO_EXIT)) { m_btnOK.SetIcon(m_icoOK); }
    else { m_btnOK.SetWindowText(RES_STR(IDS_EXIT)); }
}

void CRemoteMediaServerDlg::OnBnClickedBtnAdd()
{
#if 0 // TODO Ϊ�����˵������Ϣ��Ӧ����
    if (m_menuAdd.GetSafeHmenu() != NULL)
    {
        CRect rtAddBtn;
        m_btnAdd.GetClientRect(&rtAddBtn);
        m_btnAdd.ClientToScreen(&rtAddBtn);
        m_menuAdd.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, 
            rtAddBtn.left, rtAddBtn.bottom, this);
    }
    else
#endif
    {
        SendMessage(WM_COMMAND, MAKEWPARAM(IDM_ADD_FILE, 0));
    }
    m_socketListen.UpdateClientsPlaylist();
}


void CRemoteMediaServerDlg::OnBnClickedBtnRemove()
{
    m_ctrlPlaylist.DeleteSelectedItems();
    m_socketListen.UpdateClientsPlaylist();
}


void CRemoteMediaServerDlg::OnBnClickedBtnHelp()
{
    SendMessage(WM_SYSCOMMAND, MAKEWPARAM(IDM_ABOUTBOX, 0));
}

void CRemoteMediaServerDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    m_socketListen.Close();
    AfxSocketTerm();
    m_live555.Terminate();
    m_ctrlPlaylist.SaveUILayout();
    m_menuAdd.DestroyMenu();
}

BOOL CRemoteMediaServerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (0 == HIWORD(wParam)) // Menu
    {
        switch (LOWORD(wParam))
        {
        case IDM_ADD_FILE: 
            {
                CFileDialog dlgFile(TRUE, NULL, NULL, 
                    OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST 
                    | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, 
                    RES_STR(IDS_FILETYPEFILTER), this);
                if (dlgFile.DoModal() == IDOK)
                {
                    // Caution:
                    // �������һ��ѡ�����ļ�����������OPENFILENAME�ڲ�
                    // �Ĵ洢�ļ����ֵĻ�����������������޷���������ѡ����ļ�
                    // ���ļ����洢����
                    CString strReadableSize;
                    POSITION p = dlgFile.GetStartPosition();
                    for (CString strFileName; p != NULL;)
                    {
                        strFileName = dlgFile.GetNextPathName(p);
                        cmGetReadableFileSize(strReadableSize, 
                            cmGetFileSize(strFileName));
                        m_ctrlPlaylist.AddFileToMap(strFileName, strReadableSize);
                    }
                    m_ctrlPlaylist.UpdateContents();
                    m_ctrlPlaylist.SavePlayList();
                }
            }
            break;
        case IDM_ADD_FOLDER: AfxMessageBox(_T("Folder")); break;
        case IDM_ADD_URL: AfxMessageBox(_T("URL")); break;
        default: break;
        }
    }
    return CDialogEx::OnCommand(wParam, lParam);
}

LRESULT CRemoteMediaServerDlg::OnNcHitTest(CPoint point)
{
    LRESULT lRet = CDialogEx::OnNcHitTest(point);
    
    if (HTCLIENT == lRet && ::GetAsyncKeyState(MK_LBUTTON) < 0)
    {
        return HTCAPTION;
    }
    return lRet;
}

CString &CRemoteMediaServerDlg::GetFileGuid(CString &strGuid, const CString &strFileName)
{
    m_ctrlPlaylist.GetFileGUID(strGuid, strFileName);
    return strGuid;
}

const CString &CRemoteMediaServerDlg::GetMediaServerPath() const throw()
{
    return m_live555.GetLive555Path();
}

