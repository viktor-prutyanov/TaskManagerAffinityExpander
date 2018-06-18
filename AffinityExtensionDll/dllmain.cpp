#include "stdafx.h"
#include "hook.h"
#include "hooks.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserv)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!hookFunction(STR_SET_WINDOW_LONG_PTR_W, (ULONG_PTR)H_SetWindowLongPtrW))
			log_report("Failed to hook %s", STR_SET_WINDOW_LONG_PTR_W);

		if (!hookFunction(STR_SEND_MESSAGE_W, (ULONG_PTR)H_SendMessageW))
			log_report("Failed to hook %s", STR_SEND_MESSAGE_W);
            
        SetLastError(0);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

