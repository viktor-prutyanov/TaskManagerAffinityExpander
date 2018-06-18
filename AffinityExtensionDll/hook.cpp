#include "stdafx.h"
#include "hook.h"
#include "import.h"

#include <CommCtrl.h>
#include <stdlib.h>

/*
    Finds function address in PE Import Directory and replace it with given address
*/
bool hookFunction(PCHAR name, ULONG_PTR newFunc) {
	ULONG_PTR imageBase = code_get_image_base_addr();
    ULONG_PTR importRVA, pHookedFunc;
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    PVOID pOrigThunk, pThunk;

	if (!checkMZ(imageBase))
		return false;

	if (!(importRVA = getImportDirRVA(imageBase)))
		log_report_err(false, "Invalid import RVA");

	importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(imageBase + importRVA);
	while (importDesc->Characteristics) {
		pOrigThunk = (PVOID)(importDesc->OriginalFirstThunk + imageBase);

		if (!importDesc->OriginalFirstThunk)
			log_report("Invalid OriginalFirstThunk");

		if (!(pThunk = (PVOID)(importDesc->FirstThunk + imageBase)))
			log_report("Invalid FirstThunk");

		if (pHookedFunc = substituteFuncByName(name, newFunc, pOrigThunk, pThunk, imageBase))
			break;

		importDesc++;
	}

	return true;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	WCHAR buf[TITLE_SIZE];

	if (!GetWindowText(hwnd, buf, sizeof(buf)))
		log_report_err(false, "Failed GetWindowText");

	if (!wcscmp(buf, STR_PROCESSES)) {
		log_report("ListView has been found in \"Processes\" tab");
		memcpy((PVOID)lParam, &hwnd, sizeof(hwnd));

		return false;
	}

	return true;
}


bool get_hListView(HWND hWnd, HWND *p_hListView, bool *hListView_flag) {
	WCHAR buf[TITLE_SIZE];

	if (!GetWindowText(hWnd, buf, sizeof(buf)))
		log_report_err(false, "Failed GetWindowText");

	if (!wcscmp(buf, STR_PROCESSES)) {
		log_report("\"Processes\" tab has been found");
		(*hListView_flag) = true;
        EnumChildWindows(hWnd, EnumChildProc, (LPARAM)p_hListView);
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

bool displayAffinityByPid(HWND hWnd, int pid_column) {
    int itemCount = ListView_GetItemCount(hWnd);
	SYSTEM_INFO sysinfo;
	WCHAR buf[BUFFSIZE] = {};
	WCHAR affBuf[MAX_CPU_NUM] = {};
    int nr_cpus, pid, affinity_column;

    if (pid_column == -1)
        return false;

	GetSystemInfo(&sysinfo);
	nr_cpus = sysinfo.dwNumberOfProcessors;

	for (int i = 0; i < itemCount; i++) {
		ListView_GetItemText(hWnd, i, pid_column, buf, sizeof(buf));
		pid = _wtoi(buf);
        getAffinityByPid(pid, affBuf, nr_cpus);
        affinity_column = getNrColumns(hWnd) - 1;
		ListView_SetItemText(hWnd, i, affinity_column, affBuf);
	}

	return true;
}

int getNrColumns(HWND hWnd) {
	HWND hWndHeader = ListView_GetHeader(hWnd);
    int nr_columns;

	if ((nr_columns = Header_GetItemCount(hWndHeader)) == -1)
		log_report("Header_GetItemCount failed");

	return nr_columns;
}