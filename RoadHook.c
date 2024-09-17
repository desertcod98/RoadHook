#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include "include/RoadHook.h"
#include "cvector.h"

typedef struct {
	LPVOID originalFunction;
	LPVOID redirectedFunction;
	bool isActive;
	uint8_t backupPrologue[5];
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

void initializeHook(LPVOID originalFunction, LPVOID redirectedFunction)
{
	HookedFunc hookedFunc;
	uint8_t backupPrologue[5];
	memcpy(&hookedFunc.backupPrologue, originalFunction, sizeof(backupPrologue));
	hookedFunc.isActive = false;
	hookedFunc.originalFunction = originalFunction;
	hookedFunc.redirectedFunction = redirectedFunction;
	cvector_push_back(gHookedFuncs, hookedFunc);
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
		memcpy(originalFunction, &hookedFunc->backupPrologue, sizeof(hookedFunc->backupPrologue));
		hookedFunc->isActive = false;
	}
}
