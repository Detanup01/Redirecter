#include "../headers/aHookOthers.h"
#include "../headers/common_includes.h"
#include "../headers/INISettings.h"

int (__cdecl* Real_AssertFailed)(const char* pstrFile, unsigned int nLine, const char* pMsg);

int __cdecl Mine_AssertFailed(const char* pstrFile, unsigned int nLine, const char* pMsg)
{
	if(IsLogEnabled())
		PRINT_DEBUG("Mine_AssertFailed %s, %s, %u\n", pstrFile, pMsg, nLine);
	return Real_AssertFailed(pstrFile, nLine, pMsg);
}

int(__cdecl* Real_V_ExtractDomainFromURL)(DWORD a, const char* b, int c);

int __cdecl Mine_V_ExtractDomainFromURL(DWORD a, const char* b, int c)
{
	if (IsLogEnabled())
		PRINT_DEBUG("Mine_V_ExtractDomainFromURL %lu, %s, %d\n", a, b, c);
	return Real_V_ExtractDomainFromURL(a, b, c);
}

void HookAll()
{
	//?AssertFailed@?$AssertMsgHelper@$00@@SA_NPBDI0@Z
	auto tier0_module = GetModuleHandle(L"tier0_s.dll");
	if (tier0_module)
	{
		//PRINT_DEBUG("module 'tier0_s.dll' found and hooking\n");
		Real_AssertFailed = (decltype(Real_AssertFailed))GetProcAddress(tier0_module, "?AssertFailed@?$AssertMsgHelper@$00@@SA_NPBDI0@Z");
		if (!Real_AssertFailed)
		{
			if (IsLogEnabled())
				PRINT_DEBUG("getprocaddr failed");
		}
		DetourAttach(reinterpret_cast<PVOID*>(&Real_AssertFailed), reinterpret_cast<PVOID>(Mine_AssertFailed));
	}


	auto vstdlib_s_module = GetModuleHandle(L"vstdlib_s.dll");
	if (vstdlib_s_module)
	{
		//PRINT_DEBUG("module 'vstdlib_s.dll' found and hooking\n");
		Real_V_ExtractDomainFromURL = (decltype(Real_V_ExtractDomainFromURL))GetProcAddress(vstdlib_s_module, "V_ExtractDomainFromURL");
		if (!Real_V_ExtractDomainFromURL)
		{
			if (IsLogEnabled())
				PRINT_DEBUG("getprocaddr failed");
		}
		DetourAttach(reinterpret_cast<PVOID*>(&Real_V_ExtractDomainFromURL), reinterpret_cast<PVOID>(Mine_V_ExtractDomainFromURL));
	}
}