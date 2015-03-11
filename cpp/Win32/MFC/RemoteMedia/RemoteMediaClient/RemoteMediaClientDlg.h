// 2013-08-23T26:06+08:00
// RemoteMediaClientDlg.h : ͷ�ļ�
//

#pragma once

#include <afxlistctrl.h>
#include <afxcmn.h>
#include <afxwin.h>

#include "VLCWrapper.h"
#include "../RemoteMediaUtility.h"
#include "RMCSocket.h"

// CRemoteMediaClientDlg �Ի���
class CRemoteMediaClientDlg : public CDialogEx
{
// ����
public:
	CRemoteMediaClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REMOTEMEDIACLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
    cmCIcon m_icoConn;
    cmCIcon m_icoDisConn;
    cmCIcon m_icoPreview;
    cmCIcon m_icoAbout;
    cmCIcon m_icoExit;
private:
    CVLCWrapper m_vlcPlayer;
    CRMCSocket m_socketConn;
    DWORD m_dwRetryTimes; // ����������ʧ�ܺ�������������
    CString m_strFileName; // ҪԤ�����ļ�������
    CString m_strServIP;
private:
    CRemoteMediaListCtrl m_ctrlPlaylist;
    CIPAddressCtrl m_ctrlServIPAddr;
    CButton m_btnConn;
    CButton m_btnPreview;
    CButton m_btnAbout;
    CButton m_btnExit;
    CEdit m_edtPortNum;
    CButton m_chkUseDefPort;
protected:
    void InitUI();
    void LoadSettings();
    void SaveSettings();
    CString &RetrieveServIP(CString &strServIP);
public:
    void ConnectToServer();
    void DisConnectWithServer();
    void TryToHandshake();
    void LoadPlaylist();
    void PreviewSelectedFile();
public:
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedBtnConnect();
    afx_msg void OnBnClickedBtnPreview();
    afx_msg void OnBnClickedBtnHelp();
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnBnClickedChkUsedefport();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
