#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include "include/RoadHook.h"


void hookFunction(LPVOID originalFunction, LPVOID hookedFunction)
{
	uint8_t jmpInstruction[5];
	DWORD oldProtect;
	VirtualProtect(originalFunction, sizeof(jmpInstruction), 0x40, &oldProtect);
	//TODO save original bytes so you can unhook
	//devi calcolare distanza((originalFunction + 5), hookedFunction) con un signed 32 bit
	uint32_t distance = (uint32_t)hookedFunction - ((uint32_t)originalFunction + 5);
	
	
	jmpInstruction[0] = 0xE9;
	memcpy(&jmpInstruction[1], &distance, sizeof(distance));
	memcpy(originalFunction, &jmpInstruction, sizeof(jmpInstruction));
}
