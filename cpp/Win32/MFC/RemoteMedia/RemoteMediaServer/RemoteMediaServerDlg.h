// 2013-08-24T14:16+08:00
// RemoteMediaServerDlg.h : ͷ�ļ�
//

#pragma once
#include <afxlistctrl.h>
#include <afxext.h>
#include <afxwin.h>

#include "../RemoteMediaUtility.h"
#include "RMSSocket.h"
#include "Live555MediaServer.h"
#include "MKVMerge.h"

class CRMSConnect;

// CRemoteMediaServerDlg �Ի���
class CRemoteMediaServerDlg : public CDialogEx
{
// ����
public:
	CRemoteMediaServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REMOTEMEDIASERVER_DIALOG };

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
    cmCIcon m_icoAdd;
    cmCIcon m_icoRemove;
    cmCIcon m_icoHelp;
    cmCIcon m_icoOK;
private:
    CRemoteMediaListCtrl m_ctrlPlaylist;
    CButton m_btnAdd;
    CButton m_btnRemove;
    CButton m_btnHelp;
    CButton m_btnOK;
    CMenu   m_menuAdd;
private:
    CRMSSocket m_socketListen;

    CLive555MediaServer m_live555;
private:
    void InitUI();
protected:
    afx_msg void OnBnClickedBtnAdd();
    afx_msg void OnBnClickedBtnRemove();
    afx_msg void OnBnClickedBtnHelp();
    afx_msg void OnDestroy();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnNcHitTest(CPoint point);
public:
    CString &GetFileGuid(CString &strGuid, const CString &strFileName);
    const CString &GetMediaServerPath() const throw();
};
