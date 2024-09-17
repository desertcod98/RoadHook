#pragma once
#include <Windows.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	void hookFunction(LPVOID originalFunction, LPVOID hookedFunction);

#ifdef __cplusplus
}
#endif