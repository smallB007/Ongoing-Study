// 2013-08-19T16:42+08:00
// RemoteMediaServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRemoteMediaServerApp:
// �йش����ʵ�֣������ RemoteMediaServer.cpp
//

class CRemoteMediaServerApp : public CWinApp
{
public:
	CRemoteMediaServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
private:
    HANDLE m_hServMutex;
public:
    virtual int ExitInstance();
};

extern CRemoteMediaServerApp theApp;