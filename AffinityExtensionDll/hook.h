#pragma once

bool hookFunction(PCHAR name, ULONG_PTR newFunc);

bool get_hListView(HWND hWnd, HWND *p_hListView, bool *hListView_flag);
int getNrColumns(HWND hWnd);

bool addAffinityColumn(HWND hWnd, int idx);
bool displayAffinityByPid(HWND hWnd, int pid_column);

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);