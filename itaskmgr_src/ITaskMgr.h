#if !defined(__ITASKMGR_H__INCLUDED_)
#define __ITASKMGR_H__INCLUDED_

#define APPNAME	_T("ITaskMgr")
#define HISTORY_MAX	512

#define MODE_ICON		-1
#define MODE_CPUPOWER	0
#define MODE_PROCESS	1
#define MODE_TASKLIST	2

#define MY_NOTIFYICON			(WM_APP + 1000)
#define WM_THREADPACK_POINTER	(WM_USER + 1001)

typedef struct _ThreadPack
{
	HINSTANCE g_hInst;
	HWND hDlg;
	UINT nMode;
	BOOL bEnd;

	HANDLE hIdleThread;
	
	HWND hwndTab;
	HWND hwndProcessList;
	HWND hwndCpupower;
	HWND hwndTaskList;

	DWORD dwInterval;
	HICON hIcon[12];
	NOTIFYICONDATA	nidTrayIcon;

	char chPowHistory[HISTORY_MAX];
	char chMemHistory[HISTORY_MAX];
} ThreadPack;

LPARAM GetSelectedItemLParam(HWND hwndLView);
void SelectItemLParam(HWND hwndLView, LPARAM lParam);

#endif // !defined(__ITASKMGR_H__INCLUDED_)
