/***************************************************************
 * Name:      RemoteMediaUtility.h
 * Purpose:   
 * Author:    myd7349 (myd7349@gmail.com)
 * Created:   2013-08-20 15:02
 * Copyright: 
 * License:   
 **************************************************************/
#pragma once

#include <afx.h>
#include <afxsock.h>

#include "RemoteMediaListCtrl.h"
#include "VLPLog.h"

#define RES_STR(id) CString(MAKEINTRESOURCE(id))
#define cm_STR(c)           #c
#define cmSTR(c)            cm_STR(c)
#define cmMCAT(l, r)        l##r
#define cmCSTRCAT(l, r)     l r
#define cmFILE              _T(__FILE__)
#define cmLINE              __LINE__
#define cmFUNCTION          _T(__FUNCTION__)
#define cmFUNIN             cmCSTRCAT(_T("(i): "), cmFUNCTION)
#define cmFUNOUT            cmCSTRCAT(_T("(o): "), cmFUNCTION)
ULONGLONG cmGetFileSize(LPCTSTR lpcszFileName);
CString &cmGetReadableFileSize(CString &strSize, const ULONGLONG &ullSizeInBytes);
int cmCompareReadableFileSize(const CString &strLSize, const CString &strRSize);
CString &cmGetModulePathName(CString &strPath, HMODULE hMod = NULL);
CString &cmGetFullPath(CString &strFullPath, const CString &strPath, const CString &strFile);
inline BOOL cmIsValidFileHandle(HANDLE hFile)
{
    return (hFile != INVALID_HANDLE_VALUE);
}
BOOL cmIsExeRunning(LPCTSTR lpcszExeFile);
CString &cmTimeSecs2HMS(CString &strTime, const DWORD &dwTimeInSecs);
CString &cmGetCurDateTime(CString &strDateTime);
BOOL cmGetErrMsg(CString &strMsgContainer, 
    const DWORD &dwErrCode = ::GetLastError(), 
    const WORD &wLangID = ::GetSystemDefaultLangID());
CString &cmGenerateGUID(CString &strGUID);

// RemoteMediaServerĬ�϶˿ں�
enum 
{
    DEF_PORT_NUM = 7349, 
    REV_PORT_NUM = 1028
};

// ��RemoteMedia��Server��Client֮��ͨ��TCP��������ݵ�����
union cmRM_DATA_T
{
    // �ͻ��������˷��͵����ݰ��ĺ���
    enum RMC_DATA_T
    {
        RMC_CLIENT = 0x7349FFC0, // ��ʾһ��Client��������
        RMC_PREVIEW = 0x7349FFC1, // ��ʾԤ�����󣨿ͻ�����ͨ�����紮�����������϶�ý�壩
        RMC_MEDIA = 0x7349FFC2, // ��ʾ�������󣨿ͻ��������ط������ĳ����ý���ļ���
        RMC_CLOSE = 0x7349FFC3 // ��ʾClient֪ͨServer�ͻ�����Ҫ�Ͽ�����
    } rmc_dat_type;

    // �������ͻ��˷��͵����ݰ�����
    enum RMS_DATA_T
    {
        RMS_ACCEPT = 0x1028FFD0, // ��ʾ����˽����˿ͻ��˵���������
        RMS_REFUSE = 0x1028FFD1, // ��ʾ����˾ܾ��˿ͻ��˵���������
        RMS_RTSPREADY = 0x1028FFD2, // ��ʾ�������Ӧ�˿ͻ��˵�RMC_PREVIEW����׼���ṩý��Ԥ������
        RMS_RTSPNOTREADY = 0x1028FFD3, // ��ʾ�������Ӧ�˿ͻ��˵�RMC_PREVIEW�����޷��ṩԤ������
        RMS_MEDIA = 0x1028FFD4, // ��ʾ�������Ӧ�˿ͻ��˵�RMC_MEDIA������ͻ��˷����ļ�
        RMS_CLOSE = 0x1028FFD5 // ��ʾ����˳���Ҫ�˳�
    } rms_dat_type;
};

// �����Ƿ���˳����ǿͻ��˳����ڴ������ݵ�ʱ��
// Ҫ�ȴ���һ������ͷ������ͷ�����˽�Ҫ��������ݵ������Լ����ݵĳ��ȡ�
struct cmRMDataStreamHeader
{
    cmRM_DATA_T rmDatType; // ��������
    DWORD       dwDatLen;  // ���ݳ��ȣ����ֽ�Ϊ��λ������ͷ��С���������ڣ�
};

class cmCIcon
{
public:
    cmCIcon() throw();
    virtual ~cmCIcon();
    BOOL Load(UINT uResID, HINSTANCE hResInst = AfxGetResourceHandle());
    void Free();
    operator HICON() throw()
    {
        return m_hICO;
    }
private:
    cmCIcon(const cmCIcon &);
    cmCIcon &operator=(const cmCIcon &);
private:
    HICON m_hICO;
};

class cmCBytes
{
public:
    cmCBytes(const DWORD &dwSize);
    virtual ~cmCBytes();
    operator void * const() const throw() { return m_lpbyBuf; }
    const DWORD &GetSizeInBytes() const throw() { return m_dwSizeInBytes; }
private:
    DWORD m_dwSizeInBytes;
    BYTE m_byArr[1024 * 512];
    LPBYTE m_lpbyBuf;
};

