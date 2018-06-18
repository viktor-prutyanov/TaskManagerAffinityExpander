#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define STR_SET_WINDOW_LONG_PTR_W "SetWindowLongPtrW"
#define STR_SEND_MESSAGE_W "SendMessageW"

#define BUFFSIZE 256
#define MAX_CPU_NUM 64
#define TITLE_SIZE 64
#define STR_PROCESSES L"Processes"

#define DEFAULT_COLUMNS_NUM 7
#define PID_COLUMN 1

#include "log_report.h"