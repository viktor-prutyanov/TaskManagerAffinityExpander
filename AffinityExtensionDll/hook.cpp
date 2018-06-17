#include "stdafx.h"
#include "hook.h"
#include "import.h"

#include <CommCtrl.h>
#include <stdlib.h>

bool hookFuncByName(PCHAR name, ULONG_PTR newFunc) {
	ULONG_PTR pBase = get_pBase();
	InfoLog("pBase: %p\n", pBase);
	if (!checkMZ(pBase))
		return false;

	ULONG_PTR importRVA = getImportRVA(pBase);
	if (importRVA == NULL)
		InfoLogRet(false, "importRVA is NULL");

	PIMAGE_IMPORT_DESCRIPTOR pImpDir = (PIMAGE_IMPORT_DESCRIPTOR)(pBase + importRVA);
	while (pImpDir->Characteristics) {
		PVOID pOrigThunk = (PVOID)(pImpDir->OriginalFirstThunk + pBase);
		if (!pImpDir->OriginalFirstThunk)
			InfoLog("Original first thunk is NULL");

		PVOID pThunk = (PVOID)(pImpDir->FirstThunk + pBase);
		if (!pThunk)
			InfoLog("pThunk in NULL");

		ULONG_PTR pHookedFunc = changeFuncAddrByName(name, newFunc, pOrigThunk, pThunk, pBase);
		if (pHookedFunc != NULL) {
			InfoLog("Original address of hooked func: %p\n", pHookedFunc);
			break;
		}

		pImpDir++;
	}

	return true;
}

bool get_hListView(HWND hWnd, HWND *p_hListView, bool *is_hListView_got) {
	WCHAR buf[TITLE_SIZE];
	if (!GetWindowText(hWnd, buf, sizeof(buf)))
		InfoLogRet(false, "GetWindowText failed in get_hListView");

	if (!wcscmp(buf, PROCESSES)) {
		InfoLog("Processes table has found!");
		(*is_hListView_got) = true;
		if (!EnumChildWindows(hWnd, EnumChildProc, (LPARAM)p_hListView))
			InfoLog("EnumChildWindows failed in get_hListView");
	}

	return true;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	WCHAR buf[TITLE_SIZE];
	if (!GetWindowText(hwnd, buf, sizeof(buf)))
		InfoLogRet(false, "GetWindowText failed in EnumChilProc");

	if (!wcscmp(buf, PROCESSES)) {
		InfoLog("Processes ListView is found!");
		memcpy((PVOID)lParam, &hwnd, sizeof(hwnd));
		return false;
	}

	return true;
}

bool addAffinityColumn(HWND hWnd, int idx) {
	LVCOLUMN lvC;

	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.iSubItem = 7;
	lvC.pszText = L"Affinity";
	lvC.cx = 100;
	lvC.fmt = LVCFMT_LEFT;

	return ListView_InsertColumn(hWnd, idx, &lvC);
}

void getAffinityByPid(int pid, PWCHAR affinity, int nr_cpus) {
    HANDLE hProcess;
	DWORD_PTR systemAffinityMask, mask = 0, temp_mask;
    
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	GetProcessAffinityMask(hProcess, &mask, &systemAffinityMask);
    temp_mask = mask;

    /*
        Print '?' signs if mask is unavailable
    */
	for (int i = 0; i < nr_cpus; ++i) {
		char bit = temp_mask & 0x1;
        affinity[i] = mask ? (bit + '0') : '?';
        temp_mask >>= 1;
	}

	affinity[nr_cpus] = '\0';
}

bool drawAffinityByPid(HWND hWnd, int pidColNum) {
	if (pidColNum == -1)
		InfoLogRet(false, "pidColNum is -1 in drawAffinityByPID");

	int itemCount = ListView_GetItemCount(hWnd);

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int nr_cpus = sysinfo.dwNumberOfProcessors;

	WCHAR buf[BUFFSIZE] = {};
	WCHAR affBuf[MAX_CPU_NUM] = {};

	for (int iItem = 0; iItem < itemCount; ++iItem) {
		ListView_GetItemText(hWnd, iItem, pidColNum, buf, sizeof(buf));

		int pid = _wtoi(buf);

        getAffinityByPid(pid, affBuf, nr_cpus);

		int affinityCol = getListViewNumOfCols(hWnd) - 1;  // -1 because of affinity column itself
		ListView_SetItemText(hWnd, iItem, affinityCol, affBuf);
	}

	return true;
}

int getListViewNumOfCols(HWND hWnd) {
	HWND hWndHdr = ListView_GetHeader(hWnd);
	if (!hWndHdr)
		InfoLog("ListView_GetHeader returned NULL in getListViewNumOfCols");

	int numOfCols = Header_GetItemCount(hWndHdr);
	if (numOfCols == -1)
		InfoLog("Header_getItemCount failed in getListViewNumOfCols");

	return numOfCols;
}