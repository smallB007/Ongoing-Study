
// RemoteMediaServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "RemoteMediaServer.h"
#include "RemoteMediaServerDlg.h"
#include "../RemoteMediaUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRemoteMediaServerApp

BEGIN_MESSAGE_MAP(CRemoteMediaServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRemoteMediaServerApp ����

CRemoteMediaServerApp::CRemoteMediaServerApp()
    : m_hServMutex(NULL)
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CRemoteMediaServerApp ����

CRemoteMediaServerApp theApp;


// CRemoteMediaServerApp ��ʼ��

BOOL CRemoteMediaServerApp::InitInstance()
{
    if (NULL == m_hServMutex)
    {
        m_hServMutex = ::CreateMutex(NULL, FALSE, 
            _T("__RemoteMediaServer_mayadong7349_201308191624__"));
        if (m_hServMutex != NULL 
            && ::GetLastError() == ERROR_ALREADY_EXISTS)
        {
            AfxMessageBox(IDS_RUNNING, MB_OK | MB_ICONINFORMATION);
            HWND hWnd = ::FindWindow(_T("#32770"), _T("RemoteMediaServer"));
            if (::IsWindow(hWnd))
            {
                ::SetForegroundWindow(hWnd);
            }
            return FALSE;
        }
    }

    if (!cmVLPLogStartup(_T("RMSRunning.log")))
    {
        AfxMessageBox(IDS_INITLOGMODERR, MB_OK | MB_ICONWARNING);
    }
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("UBiTerSoft\\RemoteMedia"));

	CRemoteMediaServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}



int CRemoteMediaServerApp::ExitInstance()
{
    cmVLPLogShutdown();

    if (m_hServMutex != NULL)
    {
        ::CloseHandle(m_hServMutex);
        m_hServMutex = NULL;
    }

    return CWinApp::ExitInstance();
}
