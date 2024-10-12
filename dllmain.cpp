// dllmain.cpp : Defines the entry point for the DLL application.
#include "headers/common_includes.h"
#include "headers/aHookOthers.h"
#include "headers/aWinHook.h"
#include "headers/aWS2.h"
#include "headers/aWini.h"
#include "headers/aWinHttp.h"
#include "headers/INISettings.h"


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    LoadIni(hModule);
    switch (ul_reason_for_call)
    {
        case DLL_THREAD_ATTACH:
        case DLL_PROCESS_ATTACH:
        {
            //PRINT_DEBUG("Hooking lan only functions\n");
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            HookAll();
            HookWin();
            HookWS2();
            HookWini();
            HookWinHttp();
            DetourTransactionCommit();
            break;
        }
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        {
            /*
            PRINT_DEBUG("UnHooking lan only functions\n");
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(reinterpret_cast<PVOID*>(&Real_SendTo), reinterpret_cast<PVOID>(Mine_SendTo));
            DetourDetach(reinterpret_cast<PVOID*>(&Real_Connect), reinterpret_cast<PVOID>(Mine_Connect));
            DetourDetach(reinterpret_cast<PVOID*>(&Real_WSAConnect), reinterpret_cast<PVOID>(Mine_WSAConnect));
            if (Real_WinHttpConnect)
            {
                DetourDetach(reinterpret_cast<PVOID*>(&Real_WinHttpConnect), reinterpret_cast<PVOID>(Mine_WinHttpConnect));
                DetourDetach(reinterpret_cast<PVOID*>(&Real_WinHttpOpen), reinterpret_cast<PVOID>(Mine_WinHttpOpen));
                DetourDetach(reinterpret_cast<PVOID*>(&Real_WinHttpOpenRequest), reinterpret_cast<PVOID>(Mine_WinHttpOpenRequest));
            }
            DetourTransactionCommit();*/
            break;
        }
        default:
            break;
    }
    return TRUE;
}