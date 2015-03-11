// 2013-08-25T18::04+08:00
#pragma once

#include "../RemoteMediaUtility.h"
#include "MKVMerge.h"

class CRemoteMediaServerDlg;

class CRMSConnect : public CSocket
{
public:
	CRMSConnect();
	virtual ~CRMSConnect();
    virtual void OnReceive(int nErrorCode);
public:
    BOOL BoundToUI(CRemoteMediaServerDlg *pUI);
    BOOL IsUIBounded()
    {
        return m_pUI != NULL;
    }
    CRemoteMediaServerDlg *GetBoundedUI() { return m_pUI; }
protected:
    void RecvDatStrmHeader();
    void RecvDatStrm();
private:
    CRemoteMediaServerDlg *m_pUI;
    CMKVMerge m_mkvmerge;
    BOOL m_bIsDatHdrComing; // ������Ҫ���յ������Ƿ�������ͷ
    cmRMDataStreamHeader m_rmRecvStrmH; // ���һ�ν��յ�������ͷ
};
