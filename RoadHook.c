#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include "include/RoadHook.h"
#include "cvector.h"
#include "Zydis.h"

typedef struct {
	LPVOID originalFunction;
	LPVOID redirectedFunction;
	bool isActive;
	uint8_t backupPrologue[20];
} HookedFunc;

cvector_vector_type(HookedFunc) gHookedFuncs = NULL;

HookedFunc* findHookedFunc(LPVOID originalFunction) {
	HookedFunc* hookedFunc = NULL;
	for (int i = 0; i < cvector_size(gHookedFuncs); i++) {
		if (cvector_at(gHookedFuncs, i)->originalFunction == originalFunction) {
			hookedFunc = cvector_at(gHookedFuncs, i);
		}
	}
	return hookedFunc;
}

void initializeHook(LPVOID originalFunction, LPVOID redirectedFunction, LPVOID* trampolineFunction)
{
	HookedFunc hookedFunc;
	uint8_t backupPrologue[20];

	ZyanU64 runtime_address = 0x0040AC50;
	ZyanUSize offset = 0;
	ZydisDisassembledInstruction instruction;
	while (ZYAN_SUCCESS(ZydisDisassembleIntel(
		/* machine_mode:    */ ZYDIS_MACHINE_MODE_LEGACY_32,
		/* runtime_address: */ runtime_address,
		/* buffer:          */ (ZyanU8*)originalFunction + offset,
		/* length:          */ sizeof(backupPrologue) - offset,
		/* instruction:     */ &instruction
	)) && offset < 5) {
		memcpy(&backupPrologue[0] + offset, (uint8_t*)originalFunction + offset, instruction.info.length);
		offset += instruction.info.length;
		runtime_address += instruction.info.length;
	}
	memcpy(&hookedFunc.backupPrologue, &backupPrologue, sizeof(backupPrologue));
	hookedFunc.isActive = false;
	hookedFunc.originalFunction = originalFunction;
	hookedFunc.redirectedFunction = redirectedFunction;
	cvector_push_back(gHookedFuncs, hookedFunc);

	//Create trampoline function
	int sizeOfPrologue = offset;
	LPVOID trampolineFuncAddr = VirtualAlloc(NULL, sizeOfPrologue + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy(trampolineFuncAddr, &backupPrologue, sizeOfPrologue);

	//Craft and insert JMP instruction
	uint32_t distance = ((uint32_t)originalFunction + sizeOfPrologue) - ((uint32_t)trampolineFuncAddr + sizeOfPrologue + 5);
	uint8_t jmpInstruction[5];
	jmpInstruction[0] = 0xE9;
	memcpy(&jmpInstruction[1], &distance, sizeof(distance));
	memcpy((char*)trampolineFuncAddr + sizeOfPrologue, &jmpInstruction, sizeof(jmpInstruction));
	*trampolineFunction = trampolineFuncAddr;
}

void enableHook(LPVOID originalFunction) {
	HookedFunc* hookedFunc = findHookedFunc(originalFunction);
	if (hookedFunc == NULL) {
		printf("Error! No hook created on that function! Use InitializeHook"); 
		return;
	}

	if (!hookedFunc->isActive) {
		uint8_t jmpInstruction[5];
		DWORD oldProtect;
		VirtualProtect(originalFunction, sizeof(jmpInstruction), 0x40, &oldProtect);
		//TODO save original bytes so you can unhook
		//devi calcolare distanza((originalFunction + 5), hookedFunction) con un signed 32 bit
		uint32_t distance = (uint32_t)hookedFunc->redirectedFunction - ((uint32_t)originalFunction + 5);

		jmpInstruction[0] = 0xE9;
		memcpy(&jmpInstruction[1], &distance, sizeof(distance));
		memcpy(originalFunction, &jmpInstruction, sizeof(jmpInstruction));
		hookedFunc->isActive = true;
	}
}

void disableHook(LPVOID originalFunction) {
	HookedFunc* hookedFunc = findHookedFunc(originalFunction);
	if (hookedFunc == NULL) {
		printf("Error! No hook created on that function! Use InitializeHook");
		return;
	}
	if(hookedFunc->isActive){
		memcpy(originalFunction, &hookedFunc->backupPrologue, 5);
		hookedFunc->isActive = false;
	}
}
