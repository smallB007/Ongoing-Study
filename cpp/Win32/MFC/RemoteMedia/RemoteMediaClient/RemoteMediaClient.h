// 2013-08-11T12:56+08:00
// RemoteMediaClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRemoteMediaClientApp:
// �йش����ʵ�֣������ RemoteMediaClient.cpp
//

class CRemoteMediaClientApp : public CWinApp
{
public:
	CRemoteMediaClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRemoteMediaClientApp theApp;