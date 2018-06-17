#pragma once

unsigned char loop[] = {
	0x90,		// loop:	nop
	0x90,		//			nop
	0x90,		//			nop
	0x90,		//			nop
	0x90,		//			nop
	0xEB, 0xF9 	//			jmp loop
};

unsigned char get_pBase[] = {
	0x65, 0x4C, 0x8B, 0x24, 0x25, 0x60, 0x00, 0x00, 0x00, // mov r12, gs:[0x60]		;peb
	0x4D, 0x8B, 0x64, 0x24, 0x10, 						  // mov r12, [r12 + 0x10]	;Peb --> ImageBaseAddress 
	0x4C, 0x89, 0x21,									  // mov [rcx], r12			;save in mem
	0xC3 
};

unsigned char get_base_adr_x64[] = {
	0x65, 0x4C, 0x8B, 0x24, 0x25, 0x60, 0x00, 0x00, 0x00, // mov r12, gs:[0x60]		;peb
	0x4D, 0x8B, 0x64, 0x24, 0x18,						  // mov r12, [r12 + 0x18]	;Peb --> LDR 
	0x4D, 0x8B, 0x64, 0x24, 0x20,						  // mov r12, [r12 + 0x20]	;Peb.Ldr.InMemoryOrderModuleList
	0x4D, 0x8B, 0x24, 0x24,								  // mov r12, [r12]			;2st entry
	0x4D, 0x8B, 0x24, 0x24,								  // mov r12, [r12]			;3nd entry
	0x4D, 0x8B, 0x64, 0x24, 0x20,						  // mov r12, [r12 + 0x20]	;kernel32.dll base address!
	0x4C, 0x89, 0x21,									  // mov [rcx], r12			;save in mem
	0xC3 
};

unsigned char byte_code_myLoadLibrary[] = {
	0x53,						// push        rbx
	0x48, 0x83, 0xEC, 0x20,		// sub         rsp, 20h
	0x48, 0x89, 0xCB,			// mov         rbx, rcx
	0x48, 0x83, 0xC1, 0x10,		// add         rcx, 10h
	0xFF, 0x13,					// call        qword ptr [rbx] 
	0x48, 0x8B, 0x43, 0x08,		// mov         rax, qword ptr [rbx+8] 
	0x48, 0x83, 0xC4, 0x20,		// add         rsp, 20h 
	0x5B,						// pop         rbx
	0xFF, 0xE0					// jmp         rax 
};