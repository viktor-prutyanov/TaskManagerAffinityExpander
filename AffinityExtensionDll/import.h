#pragma once

ULONG_PTR substituteFuncByName(PCHAR name, ULONG_PTR newFunc, PVOID pOrigThunk, PVOID pThunk, ULONG_PTR pBase);
ULONG_PTR do_substituteFuncByName(PCHAR name, ULONG_PTR newFunc, PIMAGE_THUNK_DATA64 pOrigThunk, PIMAGE_THUNK_DATA64 pThunk, ULONG_PTR pBase);

ULONG_PTR code_get_image_base_addr();
ULONG_PTR getImportDirRVA(ULONG_PTR pBase);
PIMAGE_DATA_DIRECTORY getDataDirectory(PIMAGE_NT_HEADERS pPEHeader);

bool checkMZ(ULONG_PTR pBase);
