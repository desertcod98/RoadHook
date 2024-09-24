# RoadHook

RoadHook is a C library that allows you to hook functions (currently only supports x86).
## Including in C/C++ project

RoadHook is a static library, so you need the .lib file and the .h file, you can get them in the [latest release](https://github.com/desertcod98/RoadHook/releases/latest).

You can then include the library in your C/C++ project like this:
```c
#include "E:\RoadHook\RoadHook.h"
#pragma comment(lib, "E:\\RoadHook\\RoadHook.lib")
```

## Usage

```c
#include <stdio.h>
#include <windows.h>
#include "E:\RoadHook\RoadHook.h"
#pragma comment(lib, "E:\\RoadHook\\RoadHook.lib")

//If you are using the trampoline function, define it with the correct calling convention and arguments.
typedef int (WINAPI* MessageBoxWType)(HWND, LPCWSTR, LPCWSTR, UINT);
//Function pointer to the 'original' function (in reality it is the trampoline function)
MessageBoxWType messageBoxW = NULL;


//Define the function to which execution will be redirected
void WINAPI hookedFunction(HWND hwnd, LPCWSTR lptext, LPCWSTR lpcaption, UINT utype){
    printf("hoooooked");
    //Using the function pointer of the trampoline function will result in a 'normal' call to MessageBoxW
    messageBoxW(hwnd, lptext, L"I have been hooked!", MB_ICONERROR);
}

int main() {
    //Normal behaviour of MessageBoxW
    MessageBoxW(NULL, L"testText", L"testCaption", MB_OK);
    //Initialize the hook, it will not work yet, but it will create needed structures and create the trampoline function
    initializeHook(&MessageBoxW, &hookedFunction, (LPVOID*) &messageBoxW);
    //Enable the hook
    enableHook(&MessageBoxW);
    //Now execution will be redirected to our 'hookedFunction'
    MessageBoxW(NULL, L"testText", L"testCaption", MB_OK);
    //Disable the hook
    disableHook(&MessageBoxW);
    //Normal behaviour of MessageBoxW
    MessageBoxW(NULL, L"testText", L"testCaption", MB_OK);
    return 0;
}

```
## Building
The .lib file found in the [releases](https://github.com/desertcod98/RoadHook/releases/latest) was built with Visual Studio 2022.

## Dependencies

RoadHook depends on:

* [zydis](https://github.com/zyantific/zydis) as a x86 disassembler engine (needed to build the trampoline function).
* [c-vector](https://github.com/eteran/c-vector) because I am currently too lazy to build my own implementation of a dynamic array.

## License

RoadHook is licensed under the MIT license.
