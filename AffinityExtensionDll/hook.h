#pragma once

bool hookFuncByName(PCHAR name, ULONG_PTR newFunc);

bool get_hListView(HWND hWnd, HWND *p_hListView, bool *is_hListView_got);
int getListViewNumOfCols(HWND hWnd);

bool addAffinityColumn(HWND hWnd, int idx);
bool drawAffinityByPid(HWND hWnd, int pidColNum);
//bool getAffinityByPid(int pid, PWCHAR affBuf, int cpuNum);

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);