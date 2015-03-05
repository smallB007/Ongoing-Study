// 2014-08-20T13:17+08:00
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>

// �۸��������С���ŵ���Ҫ�ԡ�
#define PRINT_RECT(rc, newline) \
    _tprintf(_T("(%4d,%4d,%4d,%4d)%c"), (rc).left, (rc).top, (rc).right, (rc).bottom, \
        newline ? '\n' : ' ')

// ֮ǰ����һֱ��ΪSubtractRect�����ڼ���Python���������ϵĲ����

// ��������MSDN�Ͻ��������
// file:///E:/doc/msvc/url/SubtractRect/CRect%20SubtractRect.maff
// http://msdn.microsoft.com/zh-cn/library/09k82h2b%28v=vs.110%29.aspx
// 1.
// The subtraction is the smallest rectangle that contains 
// all of the points in lpRectScr1 that are not in the intersection 
// of lpRectScr1 and lpRectScr2.
// 2.
// The rectangle specified by lpRectSrc1 will be unchanged if 
// the rectangle specified by lpRectSrc2 doesn't completely overlap 
// the rectangle specified by lpRectSrc1 in at least one of the 
// x- or y-directions. 
// 3.
//  SubtractRect is not the same as operator - nor operator -=. 
// Neither of these operators ever calls SubtractRect. 

void SubtractRectWrapper(LPRECT lprcRes, LPCRECT lpcrcRect1, LPCRECT lpcrcRect2)
{
    PRINT_RECT(*lpcrcRect1, FALSE);
    _tprintf(_T("- "));
    PRINT_RECT(*lpcrcRect2, FALSE);
    _tprintf(_T("= "));

    SubtractRect(lprcRes, lpcrcRect1, lpcrcRect2);
    PRINT_RECT(*lprcRes, TRUE);
}
int _tmain(int argc, _TCHAR *argv[])
{
    RECT rcRes;
    RECT rcRect1;
    RECT rcRect2;

    // �μ���SubtractRect_Interpretation.png�����ͼ��ʾ��
    SetRect(&rcRect1, 10, 10, 100, 100);
    SetRect(&rcRect2, 50, 50, 150, 150);
    SubtractRectWrapper(&rcRes, &rcRect1, &rcRect2);

    // �μ���SubtractRect_Interpretation.png���Ҳ�ͼ��ʾ��
    SetRect(&rcRect2, 50, 10, 150, 150);
    SubtractRectWrapper(&rcRes, &rcRect1, &rcRect2);

    // ���㴰����󻯺����ʾ����
    SetRect(&rcRect1, 0, 0, 
        GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    SetRectEmpty(&rcRect2);
    HWND hTrayWnd = FindWindow(_T("Shell_TrayWnd"), _T(""));
    if (hTrayWnd != NULL) {
        GetWindowRect(hTrayWnd, &rcRect2);
    }
    SubtractRectWrapper(&rcRes, &rcRect1, &rcRect2);

    // rect2 ��ȫ������rect1�ڡ�
    SetRect(&rcRect1, 10, 10, 100, 100);
    SetRect(&rcRect2, 10, 10, 20, 30);
    SubtractRectWrapper(&rcRes, &rcRect1, &rcRect2);

    // rect1��ȫ������rect2�ڡ�
    SetRect(&rcRect1, 10, 10, 30, 20);
    SetRect(&rcRect2, 10, 10, 100, 100);
    SubtractRectWrapper(&rcRes, &rcRect1, &rcRect2);

    // �ܽ᣺
    // 1. �����rect1�м�ȥ rect1��rect2�Ľ��� �õ���ͼ���Ǿ���ʱ�����ؽ������
    //    ������Ρ�
    // 2. �����rect1�м�ȥ rect1��rect2�Ľ��� �õ���ͼ�β��Ǿ��Σ�Ҳ����˵��
    //    ������x�᷽���ϣ�����y�᷽���ϣ�rect2��������ȫ��סrect1��ĳһ�ߣ���
    //    �򷵻ؽ��Ϊrect1��

    system("pause");

    return 0;
}
