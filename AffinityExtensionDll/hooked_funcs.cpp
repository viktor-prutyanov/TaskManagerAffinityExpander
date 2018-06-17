#include "stdafx.h"
#include "hooked_funcs.h"
#include "hook.h"

#include <CommCtrl.h>
#include <stdlib.h>

//	ListView handle
HWND g_hListView;
bool is_hListView_got = false;

//  Indicator of column added
//	and global variable to understand the moment to add
bool isAffinityColAdded = false;
int numOfInitializedCols = 0;

LONG_PTR
WINAPI
HookedSetWindowLongPtrW(
	_In_ HWND hWnd,
	_In_ int nIndex,
	_In_ LONG_PTR dwNewLong)
{
	LONG_PTR ret = SetWindowLongPtr(hWnd, nIndex, dwNewLong);

	if (!is_hListView_got)
		if (!get_hListView(hWnd, &g_hListView, &is_hListView_got))
			InfoLog("get_hListView failed");

	return ret;
}

LRESULT
WINAPI
HookedSendMessageW(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_Pre_maybenull_ _Post_valid_ WPARAM wParam,
	_Pre_maybenull_ _Post_valid_ LPARAM lParam)
{
	LRESULT ret = 0;

	if (is_hListView_got && hWnd == g_hListView && Msg == LVM_INSERTCOLUMNW && !isAffinityColAdded) {
		ret = SendMessageW(hWnd, Msg, wParam, lParam);
		numOfInitializedCols++;

		int colsNum = getListViewNumOfCols(hWnd);
		InfoLog("   cols: %d, init_cols: %d", colsNum, numOfInitializedCols);

		if (!addAffinityColumn(hWnd, colsNum))
			InfoLog("addAffinityColumn failed in HookedSendMessageW");
		else {
			InfoLog("Affinity column successfully added!");
			isAffinityColAdded = true;
		}

		goto end_hook;
	}

	if (is_hListView_got && hWnd == g_hListView && Msg == WM_SETREDRAW && isAffinityColAdded) {
		if (!drawAffinityByPid(hWnd, PID_COL_NUM))
			InfoLog("drawAffinityByPID failed in HookedSendMessageW");
	}

	ret = SendMessageW(hWnd, Msg, wParam, lParam);

end_hook:
	return ret;
}