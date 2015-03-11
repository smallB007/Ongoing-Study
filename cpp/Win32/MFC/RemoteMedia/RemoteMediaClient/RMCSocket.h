// 2013-08-23T09:03+08:00
#pragma once

#include "../RemoteMediaUtility.h"

class CRemoteMediaClientDlg;

class CRMCSocket : public CSocket
{
public:
	CRMCSocket();
	virtual ~CRMCSocket();
    virtual void OnReceive(int nErrorCode);
public:
    BOOL BoundToUI(CRemoteMediaClientDlg *pUI);
    BOOL IsUIBounded()
    {
        return m_pUI != NULL;
    }
    BOOL IsHandshakeOK();
protected:
    void RecvDatStrmHeader();
    void RecvDatStrm();
private:
    CRemoteMediaClientDlg *m_pUI;
    BOOL m_bIsDatHdrComing; // ������Ҫ���յ������Ƿ�������ͷ
    cmRMDataStreamHeader m_rmRecvStrmH; // ���һ�ν��յ�������ͷ
    BOOL m_bIsHandShakeOK; // ��¼�ͻ��˺ͷ�����Ƿ����ֳɹ�
};


