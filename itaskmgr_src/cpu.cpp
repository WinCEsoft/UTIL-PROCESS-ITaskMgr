#include "stdafx.h"
#include "ITaskMgr.h"
#include "cpu.h"

static BOOL ShowCpuStatus(ThreadPack* pTP);
static BOOL DrawGraph(ThreadPack* pTP, HWND hwndDraw);

//-----------------------------------------------------------------------------
// cpu graph dialog proc
//-----------------------------------------------------------------------------
BOOL CALLBACK DlgProcCpu(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static ThreadPack* pTP = NULL;

	switch(Msg)
	{
	case WM_THREADPACK_POINTER:
	{
		pTP = (ThreadPack*)wParam;
		HWND hwndDraw = GetDlgItem(hDlg, IDC_CPU_DRAW);
		DrawGraph(pTP, hwndDraw);
		ShowCpuStatus(pTP);
		return TRUE;
	}	
	
	// ----------------------------------------------------------
	case WM_INITDIALOG:
	{
		return TRUE;
	}
	
	// ----------------------------------------------------------
	case WM_SIZE:
	{ 
		if( pTP == NULL )
			return 0;

		HWND hwndDraw = GetDlgItem(hDlg, IDC_CPU_DRAW);
		HWND hwndText = GetDlgItem(hDlg, IDC_CPU_TEXT);

		HDWP hdwp;
		RECT rcTab;
		RECT rcDraw;
		RECT rcTitle;

		SetRect(&rcTab, 0, 0, LOWORD(lParam), HIWORD(lParam));
		TabCtrl_AdjustRect(hDlg, FALSE, &rcTab);

		GetClientRect(hwndDraw, &rcDraw);
		GetClientRect(hwndText, &rcTitle);

		hdwp = BeginDeferWindowPos(2);
		
		DeferWindowPos(hdwp, hwndDraw, HWND_TOP
			, 5
			, 5
			, rcTab.right - rcTab.left -5 -5
			, rcTab.bottom - rcTitle.bottom -5 -5
			, 0);

		DeferWindowPos(hdwp, hwndText, HWND_TOP
			, 5
			, rcTab.bottom - rcTitle.bottom
			, rcTab.right - rcTab.left -5 -5
			, rcTitle.bottom
			, 0);

		EndDeferWindowPos(hdwp);
		ShowCpuStatus(pTP);
		return 0;
	} 
	
	// ----------------------------------------------------------
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
		DrawGraph(pTP, lpdis->hwndItem);
		return TRUE;
	}

	case WM_TIMER:
	{
		ShowCpuStatus(pTP);
		InvalidateRect(GetDlgItem(hDlg, IDC_CPU_DRAW), NULL, FALSE);
		break;
	}

	default:
		break;

	}
	return FALSE;

}

//-----------------------------------------------------------------------------
// show cpu status text
//-----------------------------------------------------------------------------
static BOOL ShowCpuStatus(ThreadPack* pTP)
{
	if( pTP == NULL )
		return FALSE;

	MEMORYSTATUS ms;
	TCHAR szFmt[128];

	HWND hDlg;
	HWND hwndStatus;

	if( (hDlg = pTP->hwndCpupower ) == NULL )
		return FALSE;
	if( (hwndStatus = GetDlgItem(hDlg, IDC_CPU_TEXT) ) == NULL )
		return FALSE;

	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);


	DWORD dwTotalMem = ms.dwTotalPhys>>10;
	DWORD dwUsedMem = dwTotalMem - (ms.dwAvailPhys>>10);

	wsprintf(szFmt
		, _T("CPU time\t%d%%\r\n")
		  _T("Memory used\t%dKB/%dKB")
		, (int)(pTP->chPowHistory[0])
		, dwUsedMem, dwTotalMem );

	SetWindowText(hwndStatus, szFmt);
	return TRUE;
}

//-----------------------------------------------------------------------------
// draw graph
//-----------------------------------------------------------------------------
static BOOL DrawGraph(ThreadPack* pTP, HWND hwndDraw)
{
	int ii;

	HDC hDC;
	RECT rc;

	HPEN hpenDarkGreen;
	HPEN hpenLightGreen;
	HPEN hpenYellow;
	HPEN hOldPen;

	POINT pntBuf[HISTORY_MAX];
	memset( pntBuf, 0, sizeof(POINT)*HISTORY_MAX );

	GetClientRect(hwndDraw, &rc);

	if( pTP->nMode != MODE_CPUPOWER )
		return TRUE;


	static int nXLine = 0;
	nXLine = (nXLine+=2) %12;

	// drawing

	if( !(hpenDarkGreen = CreatePen(PS_SOLID, 1, RGB(0, 127, 0))))
	{
		return FALSE;
	}

	if( !(hpenLightGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0))))
	{
		DeleteObject(hpenDarkGreen);
		return FALSE;
	}

	if( !(hpenYellow = CreatePen(PS_SOLID, 1, RGB(255, 255, 0))))
	{
		DeleteObject(hpenDarkGreen);
		DeleteObject(hpenLightGreen);
		return FALSE;
	}

	if(!(hDC = GetDC(hwndDraw)))
	{
		DeleteObject(hpenDarkGreen);
		DeleteObject(hpenLightGreen);
		DeleteObject(hpenYellow);
		return FALSE;
	}

	// paint background
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));

	Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);

	hOldPen = (HPEN)SelectObject(hDC, hpenDarkGreen);

	POINT pnt[2];
	for( ii = 0; ii < rc.bottom - rc.top; ii+=12 )
	{
		pnt[0].x = rc.left;
		pnt[1].x = rc.right;
		pnt[0].y = pnt[1].y = ii + rc.top;
		Polyline(hDC, &pnt[0], 2);
	}

	for( ii = 12; ii < (rc.right - rc.left) + nXLine; ii+=12 )
	{
		pnt[0].x = pnt[1].x = ii + rc.left - nXLine;
		pnt[0].y = rc.top;
		pnt[1].y = rc.bottom;
		Polyline(hDC, &pnt[0], 2);
	}

	SelectObject(hDC, hOldPen);
	POINT* pPoint;

	// Draw CPU POWER
	pPoint = &pntBuf[0];
	int xx;

	xx = rc.right;
	for( ii = 0; (ii < HISTORY_MAX) && (xx > rc.left); ii++, xx-=2, pPoint++ )
	{
		LONG lHeight = (100 - pTP->chPowHistory[ii]);
		pPoint->x = xx;
		pPoint->y = rc.top + lHeight * (rc.bottom - rc.top)/100 ;
	}

	hOldPen = (HPEN)SelectObject(hDC, hpenLightGreen);
	Polyline(hDC, &pntBuf[0], ii);
	SelectObject(hDC, hOldPen);

	// Draw Memory load
	pPoint = &pntBuf[0];

	xx = rc.right;
	for( ii = 0; (ii < HISTORY_MAX) && (xx > rc.left); ii++, xx-=2, pPoint++ )
	{
		LONG lHeight = (100 - pTP->chMemHistory[ii]);
		pPoint->x = xx;
		pPoint->y = rc.top + lHeight * (rc.bottom - rc.top)/100 ;
	}

	hOldPen = (HPEN)SelectObject(hDC, hpenYellow);
	Polyline(hDC, &pntBuf[0], ii);
	SelectObject(hDC, hOldPen);



	// finish
	SelectObject(hDC, hOldPen);
	DeleteObject(hpenDarkGreen);
	DeleteObject(hpenLightGreen);
	DeleteObject(hpenYellow);
	ReleaseDC(hwndDraw, hDC);

	return TRUE;
}

