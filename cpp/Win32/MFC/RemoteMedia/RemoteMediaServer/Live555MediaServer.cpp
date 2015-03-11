// Live555MediaServer.cpp : implementation file
//

#include "stdafx.h"
#include "RemoteMediaServer.h"
#include "..\RemoteMediaUtility.h"
#include "Live555MediaServer.h"

#define LIVE555_MEDIA_SERVER_FILENAME _T("live555MediaServer.exe")

// CLive555MediaServer

CLive555MediaServer::CLive555MediaServer()
#ifdef ENABLE_TERMINATE_LIVE555
    : m_hLive555Proc(NULL)
#endif
{
#ifdef NDEBUG
    cmGetModulePathName(m_strLive555Path);
#else
    TCHAR szCurDir[_MAX_PATH];
    _tgetcwd(szCurDir, _countof(szCurDir));
    m_strLive555Path = szCurDir;
    m_strLive555Path += _T('\\');
#endif
    m_strLive555Path += _T("3rdparty\\live555\\");
}

CLive555MediaServer::~CLive555MediaServer()
{
    Terminate();
}

void CLive555MediaServer::SetLive555Path(LPCTSTR lpcszPath)
{
    m_strLive555Path = lpcszPath;
}

BOOL CLive555MediaServer::Start()
{
    CString strLive555MediaServer;
    cmGetFullPath(strLive555MediaServer, 
        m_strLive555Path, LIVE555_MEDIA_SERVER_FILENAME);
    WIN32_FIND_DATA fd;
    HANDLE hFileFind = ::FindFirstFile(strLive555MediaServer, &fd);

    if (!cmIsValidFileHandle(hFileFind))
    {
        cmVLPLogDump(RES_STR(IDS_LOG_LIVE555_NOT_FOUND), 
            CString(_T("3rdparty\\live555\\")) + LIVE555_MEDIA_SERVER_FILENAME);
        return FALSE;
    }
#if 0
    // TODO: �������Һܷѽ⡣
    if (::GetFileType(hFileFind) != FILE_TYPE_DISK)
    {
        ::FindClose(hFileFind);
        return FALSE;
    }
#endif
    ::FindClose(hFileFind);

#ifdef ENABLE_TERMINATE_LIVE555
    if (m_hLive555Proc != NULL)
#else
    if (cmIsExeRunning(LIVE555_MEDIA_SERVER_FILENAME))    
#endif
    {
        return TRUE;
    }

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = { NULL };
    // ����Ҫ����live555mediaserver.exe�������ڣ���Ϊʲô�Ҳ���
    // CREATE_NO_WINDOW��������CREATE_NEW_CONSOLEȻ�����ش����أ�
    // ������Ϊ����Ҫ���������˳�ʱ��һ�ֱȽ��º͵ķ�ʽ���������Ľ���
    // ����ͨ����live555mediaserver����WM_CLOSE��Ϣ��
    // ���ʹ��CREATE_NO_WINDOW�������FindWindow�Ҳ���live555mediaserver
    // �Ĵ��壨��ΪNO WINDOW��������CREATE_NEW_CONSOLE��ָ��
    // STARTUPINFO�е�wShowWindowΪFALSE�Ϳ���ʵ�ִ������壬���Ѵ�����������
    // ��������Ӱ��FindWindow��SendMessage�ĵ��á��Ӷ����ԱȽ��º͵���ֹ
    // live555mediaserver���̡�
    if (::CreateProcess(strLive555MediaServer, NULL, NULL, NULL, 
        FALSE, CREATE_NEW_CONSOLE, NULL, m_strLive555Path, &si, &pi))
    {
        ::CloseHandle(pi.hThread);
#ifdef ENABLE_TERMINATE_LIVE555
        m_hLive555Proc = pi.hProcess;
#else
        ::CloseHandle(pi.hProcess);
#endif
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CLive555MediaServer::Terminate()
{
#ifdef ENABLE_TERMINATE_LIVE555
    if (m_hLive555Proc != NULL)
    {
        ::TerminateProcess(m_hLive555Proc, EXIT_SUCCESS);
        m_hLive555Proc = NULL;
    }
#else
    CString strLive555MediaServer;
    cmGetFullPath(strLive555MediaServer, 
        m_strLive555Path, LIVE555_MEDIA_SERVER_FILENAME);
    HWND hLive555Wnd = ::FindWindow(_T("ConsoleWindowClass"), 
        strLive555MediaServer);
    if (::IsWindow(hLive555Wnd))
    {
        ::SendMessage(hLive555Wnd, WM_CLOSE, 0, 0);
    }
#endif
}


// CLive555MediaServer member functions
