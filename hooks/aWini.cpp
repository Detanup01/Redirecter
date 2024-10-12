#include "../headers/aWini.h"
#include "../headers/common_includes.h"
#include "../headers/INISettings.h"

HINTERNET (WINAPI* ReaL_InternetConnectW)(
	IN HINTERNET     hInternet,
	IN LPCWSTR       lpszServerName,
	IN INTERNET_PORT nServerPort,
	IN LPCWSTR       lpszUserName,
	IN LPCWSTR       lpszPassword,
	IN DWORD         dwService,
	IN DWORD         dwFlags,
	IN DWORD_PTR     dwContext
) = InternetConnectW;


HINTERNET WINAPI  Mine_InternetConnectW(
	IN HINTERNET     hInternet,
	IN LPCWSTR       lpszServerName,
	IN INTERNET_PORT nServerPort,
	IN LPCWSTR       lpszUserName,
	IN LPCWSTR       lpszPassword,
	IN DWORD         dwService,
	IN DWORD         dwFlags,
	IN DWORD_PTR     dwContext
)
{
	if (IsLogEnabled("wini"))
		PRINT_DEBUG("Mine_InternetConnectW %ls %d %ls %lu\n", lpszServerName, nServerPort, lpszUserName, dwService);
	return ReaL_InternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

BOOL (WINAPI* Real_InternetCrackUrlW)(
	IN      LPCWSTR           lpszUrl,
	IN      DWORD             dwUrlLength,
	IN      DWORD             dwFlags,
	IN OUT LPURL_COMPONENTSW lpUrlComponents
) = InternetCrackUrlW;

BOOL WINAPI Mine_InternetCrackUrlW(
	IN      LPCWSTR           lpszUrl,
	IN      DWORD             dwUrlLength,
	IN      DWORD             dwFlags,
	IN OUT LPURL_COMPONENTSW lpUrlComponents
)
{
	if (IsLogEnabled("wini"))
		PRINT_DEBUG("Mine_InternetCrackUrlW %ls\n", lpszUrl);
	return Real_InternetCrackUrlW(lpszUrl, dwUrlLength, dwFlags, lpUrlComponents);
}

HINTERNET (WINAPI* Real_InternetOpenW)(
	IN LPCWSTR lpszAgent,
	IN DWORD   dwAccessType,
	IN LPCWSTR lpszProxy,
	IN LPCWSTR lpszProxyBypass,
	IN DWORD   dwFlags
) = InternetOpenW;

HINTERNET WINAPI Mine_InternetOpenW(
	IN LPCWSTR lpszAgent,
	IN DWORD   dwAccessType,
	IN LPCWSTR lpszProxy,
	IN LPCWSTR lpszProxyBypass,
	IN DWORD   dwFlags
)
{
	if (IsLogEnabled("wini"))
		PRINT_DEBUG("Mine_InternetOpenW %ls\n", lpszAgent);
	return Real_InternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
}

HINTERNET (WINAPI* Real_HttpOpenRequestW)(
	IN HINTERNET hConnect,
	IN LPCWSTR   lpszVerb,
	IN LPCWSTR   lpszObjectName,
	IN LPCWSTR   lpszVersion,
	IN LPCWSTR   lpszReferrer,
	IN LPCWSTR* lplpszAcceptTypes,
	IN DWORD     dwFlags,
	IN DWORD_PTR dwContext
) = HttpOpenRequestW;


HINTERNET WINAPI Mine_HttpOpenRequestW(
	IN HINTERNET hConnect,
	IN LPCWSTR   lpszVerb,
	IN LPCWSTR   lpszObjectName,
	IN LPCWSTR   lpszVersion,
	IN LPCWSTR   lpszReferrer,
	IN LPCWSTR* lplpszAcceptTypes,
	IN DWORD     dwFlags,
	IN DWORD_PTR dwContext
)
{
	if (IsLogEnabled("wini"))
		PRINT_DEBUG("Real_HttpOpenRequestW %ls %ls\n", lpszVerb, lpszObjectName);
	return Real_HttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

void HookWini(void)
{
	DetourAttach(reinterpret_cast<PVOID*>(&ReaL_InternetConnectW), reinterpret_cast<PVOID>(Mine_InternetConnectW));
	DetourAttach(reinterpret_cast<PVOID*>(&Real_InternetCrackUrlW), reinterpret_cast<PVOID>(Mine_InternetCrackUrlW));
	DetourAttach(reinterpret_cast<PVOID*>(&Real_InternetOpenW), reinterpret_cast<PVOID>(Mine_InternetOpenW));
	DetourAttach(reinterpret_cast<PVOID*>(&Real_HttpOpenRequestW), reinterpret_cast<PVOID>(Mine_HttpOpenRequestW));
}
