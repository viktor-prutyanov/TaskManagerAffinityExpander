#pragma once

#define SET_WINDOW_LONG_PTR_W "SetWindowLongPtrW"
#define SEND_MESSAGE_W "SendMessageW"

LONG_PTR
WINAPI
HookedSetWindowLongPtrW(
	_In_ HWND hWnd,
	_In_ int nIndex,
	_In_ LONG_PTR dwNewLong);

LRESULT
WINAPI
HookedSendMessageW(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_Pre_maybenull_ _Post_valid_ WPARAM wParam,
	_Pre_maybenull_ _Post_valid_ LPARAM lParam);
