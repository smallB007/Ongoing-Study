// RMSSocket.cpp : implementation file
//

#include "stdafx.h"
#include "RemoteMediaServer.h"
#include "RemoteMediaServerDlg.h"
#include "RMSSocket.h"
#include "RMSConnect.h"
#include "FileMapping.h"
#include "../RemoteMediaUtility.h"

#include <new>

// CRMSSocket

CRMSSocket::CRMSSocket()
    : m_pUI(NULL)
{
}

CRMSSocket::~CRMSSocket()
{
    FreeClientList();
}


// CRMSSocket member functions

BOOL CRMSSocket::BoundToUI(CRemoteMediaServerDlg *pUI)
{
    ASSERT(pUI != NULL && pUI->GetSafeHwnd() != NULL);
    if (NULL == pUI || pUI->GetSafeHwnd() == NULL)
    {
        return FALSE;
    }

    m_pUI = pUI;
    return TRUE;
}

void CRMSSocket::UpdateClientsPlaylist()
{
    ASSERT(*this != INVALID_SOCKET);
    if (INVALID_SOCKET == *this)
    {
        return;
    }

    cmVLPLogDump(RES_STR(IDS_LOG_UPDATE_CLIENT_PLAYLIST), this);
    POSITION p = m_sockClientList.GetHeadPosition();
    for (int i = 0; i < m_sockClientList.GetCount(); ++i)
    {
        CRMSConnect *pSock = m_sockClientList.GetNext(p);
        if (pSock != NULL && *pSock != INVALID_SOCKET)
        {
            SendPlaylist(pSock);
        }
    }
}

void CRMSSocket::SendPlaylist(CRMSConnect *pSock)
{
    if (pSock != NULL && *pSock != INVALID_SOCKET)
    {
        cmVLPLogDump(RES_STR(IDS_LOG_SENDING_PLAYLIST), this, pSock);

        cmRMDataStreamHeader rmsStrmH;
        rmsStrmH.rmDatType.rms_dat_type = cmRM_DATA_T::RMS_ACCEPT;
        // ���㲥���б�Ĵ�С
        cmCFileMapping filePlaylist;
        if (filePlaylist.Attach(DEFAULT_PLAYLIST_FILENAME))
        {
            rmsStrmH.dwDatLen = (DWORD)filePlaylist.GetFileSize();
            // ��������ͷ
            pSock->Send(&rmsStrmH, sizeof(rmsStrmH));
            // ���Ͳ����б�
            pSock->Send(filePlaylist, (int)filePlaylist.GetFileSize());
        }
        else
        {
            rmsStrmH.dwDatLen = 0;
            // ��������ͷ
            pSock->Send(&rmsStrmH, sizeof(rmsStrmH));
        }
    }
}

void CRMSSocket::OnAccept(int nErrorCode)
{
    cmVLPLogDump(RES_STR(IDS_LOG_CONNECT_REQUEST), this);

    // ��ÿ��õ�socket���Ӷ���
    CRMSConnect *pRMCConn = GetUnusedSockObj();
    if (NULL == pRMCConn || *pRMCConn != INVALID_SOCKET)
    {
        cmVLPLogDump(RES_STR(IDS_LOG_NO_VALID_CONNECT_SOCKET_OBJECT), this);
        return;
    }


    cmVLPLogDump(RES_STR(IDS_LOG_ACCEPTING_CLIENT_REQUEST), this);
    // ���ܿͻ��˵�����
    if (Accept(*pRMCConn))
    {
#ifdef _VLPLOG_ON
        CString strClientIP;
        UINT uPortNum;
        pRMCConn->GetPeerName(strClientIP, uPortNum);
        cmVLPLogDump(RES_STR(IDS_LOG_CONNECTION_ESTABLISHED), 
            this, strClientIP, uPortNum, pRMCConn);
#endif
        pRMCConn->BoundToUI(m_pUI);
        // ��ͻ��˷������ݣ���ʾ���ӳɹ�
        SendPlaylist(pRMCConn);
    }

    CSocket::OnAccept(nErrorCode);
}

void CRMSSocket::FreeClientList()
{
    POSITION p = m_sockClientList.GetHeadPosition();
#ifdef _VLPLOG_ON
    if (p != NULL)
    {
        cmVLPLogDump(RES_STR(IDS_LOG_FREE_CLIENT_LIST), this);
    }
#endif
    for (int i = 0; i < m_sockClientList.GetCount(); ++i)
    {
        CRMSConnect *pSock = m_sockClientList.GetNext(p);
        if (pSock != NULL)
        {
            if (*pSock != INVALID_SOCKET)
            {
                // ֪ͨÿ�������Ӷ��󣬷���˳���Ҫ�ر���
                cmRMDataStreamHeader rmStrmH;
                rmStrmH.rmDatType.rms_dat_type = cmRM_DATA_T::RMS_CLOSE;
                rmStrmH.dwDatLen = 0;
                pSock->Send(&rmStrmH, sizeof(rmStrmH));
                pSock->Close();
                cmVLPLogDump(RES_STR(IDS_LOG_CLOSE_CONNECTTED_SOCKET), this, pSock);
            }
            cmVLPLogDump(RES_STR(IDS_LOG_DELETE_SOCKET_OBJECT), this, pSock);
            delete pSock;
            pSock = NULL;
        }
    }
    m_sockClientList.RemoveAll();
}

CRMSConnect *CRMSSocket::GetUnusedSockObj()
{
    // �������б���Ѱ�ҿ��е�socket���Ӷ���
    POSITION p = m_sockClientList.GetHeadPosition();
    for (int i = 0; i < m_sockClientList.GetCount(); ++i)
    {
        CRMSConnect *pSock = m_sockClientList.GetNext(p);
        if (pSock != NULL && INVALID_SOCKET == *pSock)
        {
            cmVLPLogDump(RES_STR(IDS_LOG_FIND_CREATED_SOCKET_OBJECT), this, pSock);
            return pSock;
        }
    }

    // ���δ�ҵ��������µ�socket���Ӷ��󣬲���ӵ��ͻ������Ӷ���list��
    CRMSConnect *pNewSock;
    try 
    { 
        pNewSock = new CRMSConnect;
        m_sockClientList.AddTail(pNewSock);
        cmVLPLogDump(RES_STR(IDS_LOG_CREATE_NEW_SOCKET_OBJECT), this, pNewSock);
    }
    catch (std::bad_alloc &) 
    { 
        pNewSock = NULL; 
        cmVLPLogDump(RES_STR(IDS_LOG_CREATE_NEW_SOCKET_OBJECT_FAILED), this);
    }
    return pNewSock;
}
