#include "../headers/aWinHook.h"
#include "../headers/common_includes.h"
#include "../headers/aHookOthers.h"
#include "../headers/aWS2.h"
#include "../headers/aWinHttp.h"
#include "../headers/aWini.h"
#include "../headers/INISettings.h"


HINSTANCE (WINAPI* Real_LoadLibrary)(
	LPCTSTR lpLibFileName   // address of filename of library module
) = LoadLibrary;

HINSTANCE WINAPI Mine_LoadLibrary(
    LPCTSTR lpLibFileName
)
{
    if (IsLogEnabled("loadlib"))
        PRINT_DEBUG("LoadLibraryW %ls\n", lpLibFileName);
    return Real_LoadLibrary(lpLibFileName);
}


HINSTANCE(WINAPI* Real_LoadLibraryA)(
    LPCSTR lpLibFileName   // address of filename of library module
    ) = LoadLibraryA;

HINSTANCE WINAPI Mine_LoadLibraryA(
    LPCSTR lpLibFileName
)
{
    if (IsLogEnabled("loadlib"))
        PRINT_DEBUG("LoadLibraryA %s\n", lpLibFileName);
    return Real_LoadLibraryA(lpLibFileName);
}

HMODULE(WINAPI* Real_LoadLibraryExW)(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags
    ) = LoadLibraryExW;

HMODULE WINAPI Mine_LoadLibraryExW(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags
)
{
    if (IsLogEnabled("loadlib"))
        PRINT_DEBUG("LoadLibraryExW %ls\n", lpLibFileName);
    return Real_LoadLibraryExW(lpLibFileName, hFile ,dwFlags);
}


HANDLE (WINAPI* Real_CreateThread)(
    IN OPTIONAL  LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    IN            SIZE_T                  dwStackSize,
    IN            LPTHREAD_START_ROUTINE  lpStartAddress,
    IN OPTIONAL  __drv_aliasesMem LPVOID lpParameter,
    IN            DWORD                   dwCreationFlags,
    OUT OPTIONAL LPDWORD                 lpThreadId
) = CreateThread;


HANDLE WINAPI Mine_CreateThread(
    IN OPTIONAL  LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    IN            SIZE_T                  dwStackSize,
    IN            LPTHREAD_START_ROUTINE  lpStartAddress,
    IN OPTIONAL  __drv_aliasesMem LPVOID lpParameter,
    IN            DWORD                   dwCreationFlags,
    OUT OPTIONAL LPDWORD                 lpThreadId
)
{
    if (IsLogEnabled("thread"))
        PRINT_DEBUG("CreateThread\n");
    auto ret = Real_CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
    if (ret)
    {
        DetourTransactionBegin();
        DetourUpdateThread(ret);
        HookAll();
        HookWin();
        HookWS2();
        HookWini();
        HookWinHttp();
        DetourTransactionCommit();      
    }
    if (lpThreadId != NULL && IsLogEnabled("thread"))
        PRINT_DEBUG("CreateThread ID: %lu\n", *lpThreadId);
    return ret;
}


BOOL (WINAPI* Real_IsDebuggerPresent)();


BOOL WINAPI Mine_IsDebuggerPresent(void)
{
    return true;
}

void(__stdcall* Real_OutputDebugStringA)(LPCSTR lpOutputString) = OutputDebugStringA;
void __stdcall Mine_OutputDebugStringA(LPCSTR lpOutputString)
{
    if (IsLogEnabled("debugger"))
        PRINT_DEBUG("OutputDebugStringA %s\n", lpOutputString);
    Real_OutputDebugStringA(lpOutputString);
}

void(__stdcall* Real_OutputDebugStringW)(LPCWSTR lpOutputString) = OutputDebugStringW;
void __stdcall Mine_OutputDebugStringW(LPCWSTR lpOutputString)
{
    if (IsLogEnabled("debugger"))
        PRINT_DEBUG("OutputDebugStringW %ws\n", lpOutputString);
    Real_OutputDebugStringW(lpOutputString);
}

void HookWin(void)
{
    Real_IsDebuggerPresent = IsDebuggerPresent;
    DetourAttach(reinterpret_cast<PVOID*>(&Real_LoadLibrary), reinterpret_cast<PVOID>(Mine_LoadLibrary));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_LoadLibraryA), reinterpret_cast<PVOID>(Mine_LoadLibraryA));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_LoadLibraryExW), reinterpret_cast<PVOID>(Mine_LoadLibraryExW));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_CreateThread), reinterpret_cast<PVOID>(Mine_CreateThread));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_IsDebuggerPresent), reinterpret_cast<PVOID>(Mine_IsDebuggerPresent));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_OutputDebugStringA), reinterpret_cast<PVOID>(Mine_OutputDebugStringA));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_OutputDebugStringW), reinterpret_cast<PVOID>(Mine_OutputDebugStringW));
}