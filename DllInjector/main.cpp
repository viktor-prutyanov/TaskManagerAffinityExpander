#include "stdafx.h"
#include "Injector.h"
#include <iostream>

int main()
{
	Injector injector = Injector(PROGRAM_PATH, DLL_PATH);

    if (injector.Inject()) {
        std::cout << "Something went wrong." << std::endl;
    }

	return 0;
}
