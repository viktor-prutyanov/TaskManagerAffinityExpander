#include "stdafx.h"
#include "DllInjector.h"

int main()
{
	DllInjector injector = DllInjector(PROGRAM_PATH);
	if (!injector.inject(DLL_PATH))
		ErrorLogRet(-1, "Injection failed");

	return 0;
}
