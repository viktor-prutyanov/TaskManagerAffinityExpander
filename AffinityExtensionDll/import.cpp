#include "stdafx.h"
#include "import.h"
#include "peb.h"

ULONG_PTR substituteFuncByName(PCHAR name, ULONG_PTR newFunc, PVOID pOrigThunk, PVOID pThunk, ULONG_PTR pBase) {
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBase;
	PIMAGE_NT_HEADERS pPEHeader = (PIMAGE_NT_HEADERS)(pBase + pDosHeader->e_lfanew);
	WORD machine = pPEHeader->FileHeader.Machine;

    if (machine == IMAGE_FILE_MACHINE_AMD64)
        return do_substituteFuncByName(name, newFunc, (PIMAGE_THUNK_DATA64)pOrigThunk, (PIMAGE_THUNK_DATA64)pThunk, pBase);

    return NULL;
}

ULONG_PTR do_substituteFuncByName(PCHAR name, ULONG_PTR newFunc, PIMAGE_THUNK_DATA64 pOrigThunk, PIMAGE_THUNK_DATA64 pThunk, ULONG_PTR pBase) {
    PCHAR current_name;

	while (pOrigThunk->u1.AddressOfData) {
		if (!pOrigThunk || pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64 || !pThunk)
			log_report_err(NULL, "Failed to substitute %s", name);

		current_name = ((PIMAGE_IMPORT_BY_NAME)(pOrigThunk->u1.AddressOfData + pBase))->Name;
		if (!strcmp(current_name, name)) {
			ULONG_PTR originalAddr = (ULONG_PTR)(pThunk->u1.Function);
			log_report("Function %s has been found at 0x%llx", current_name, originalAddr);

			DWORD oldProtect;
			VirtualProtect(&(pThunk->u1.Function), sizeof(newFunc), PAGE_EXECUTE_READWRITE, &oldProtect);
			LPVOID dst = memcpy(&(pThunk->u1.Function), &newFunc, sizeof(newFunc));
			VirtualProtect(&(pThunk->u1.Function), sizeof(newFunc), oldProtect, &oldProtect);

			return originalAddr;
		}

		pThunk++;
		pOrigThunk++;
	}

	return NULL;
}

ULONG_PTR getImportDirRVA(ULONG_PTR pBase) {
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBase;
	PIMAGE_NT_HEADERS pPEHeader = (PIMAGE_NT_HEADERS)(pBase + pDosHeader->e_lfanew);
	PIMAGE_DATA_DIRECTORY pDataDirectory = getDataDirectory(pPEHeader);
	return pDataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
}

#pragma warning(push)
#pragma warning(disable: 4172)
PIMAGE_DATA_DIRECTORY getDataDirectory(PIMAGE_NT_HEADERS pPEHeader) {
	PIMAGE_DATA_DIRECTORY pDataDirectory;
	WORD machine = pPEHeader->FileHeader.Machine;

	if (machine == IMAGE_FILE_MACHINE_AMD64) {
		IMAGE_OPTIONAL_HEADER64 optHeader;
		memcpy(&optHeader, &(pPEHeader->OptionalHeader), sizeof(optHeader));
		pDataDirectory = optHeader.DataDirectory;
	}
	else {
        return NULL;
	}

	return pDataDirectory;
}
#pragma warning(push)

ULONG_PTR code_get_image_base_addr() {
	DWORD offset = 0x60;
	PPEB64 pPeb = (PPEB64)__readgsqword(offset);
	return pPeb->ImageBaseAddress;
}

bool checkMZ(ULONG_PTR pBase) {
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBase;
    char M = pDosHeader->e_magic & 0xFF;
	char Z = (pDosHeader->e_magic >> 8) & 0xFF;

    if (M == 'M' && Z == 'Z')
        return true;

	log_report_err(false, "PE signature check failed");
}
