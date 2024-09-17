#pragma once
#include <Windows.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

	void initializeHook(LPVOID originalFunction, LPVOID redirectedFunction);
	void enableHook(LPVOID originalFunction);
	void disableHook(LPVOID originalFunction);

#ifdef __cplusplus
}
#endif