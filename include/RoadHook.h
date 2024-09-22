#pragma once
#include <Windows.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

	void initializeHook(LPVOID originalFunction, LPVOID redirectedFunction, LPVOID* trampolineFunction);
	void enableHook(LPVOID originalFunction);
	void disableHook(LPVOID originalFunction);

#ifdef __cplusplus
}
#endif