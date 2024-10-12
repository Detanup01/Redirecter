#include "../headers/aWinHttp.h"
#include "../headers/common_includes.h"
#include "../headers/INISettings.h"


HINTERNET(WINAPI* Real_WinHttpConnect)(
    IN HINTERNET     hSession,
    IN LPCWSTR       pswzServerName,
    IN INTERNET_PORT nServerPort,
    IN DWORD         dwReserved
    ) = WinHttpConnect;

HINTERNET WINAPI Mine_WinHttpConnect(
    IN HINTERNET     hSession,
    IN LPCWSTR       pswzServerName,
    IN INTERNET_PORT nServerPort,
    IN DWORD         dwReserved
)
{
    if (IsLogEnabled("http"))
        PRINT_DEBUG("Mine_WinHttpConnect %ls %u\n", pswzServerName, nServerPort);
    return Real_WinHttpConnect(hSession, pswzServerName, nServerPort, dwReserved);
}

HINTERNET(WINAPI* Real_WinHttpOpen)(IN LPCWSTR pszAgentW,
    IN DWORD dwAccessType,
    IN LPCWSTR pszProxyW,
    IN LPCWSTR pszProxyBypassW,
    IN DWORD dwFlags) = WinHttpOpen;

HINTERNET WINAPI Mine_WinHttpOpen(
    IN LPCWSTR pszAgentW,
    IN DWORD dwAccessType,
    IN LPCWSTR pszProxyW,
    IN LPCWSTR pszProxyBypassW,
    IN DWORD dwFlags
) {
    if (IsLogEnabled("http"))
        PRINT_DEBUG("Mine_WinHttpOpen %ls\n", pszAgentW);
    return Real_WinHttpOpen(pszAgentW, dwAccessType, pszProxyW, pszProxyBypassW, dwFlags);
}


HINTERNET(WINAPI* Real_WinHttpOpenRequest)(
    IN HINTERNET hConnect,
    IN LPCWSTR   pwszVerb,
    IN LPCWSTR   pwszObjectName,
    IN LPCWSTR   pwszVersion,
    IN LPCWSTR   pwszReferrer,
    IN LPCWSTR* ppwszAcceptTypes,
    IN DWORD     dwFlags
    ) = WinHttpOpenRequest;

HINTERNET WINAPI Mine_WinHttpOpenRequest(
    IN HINTERNET hConnect,
    IN LPCWSTR   pwszVerb,
    IN LPCWSTR   pwszObjectName,
    IN LPCWSTR   pwszVersion,
    IN LPCWSTR   pwszReferrer,
    IN LPCWSTR* ppwszAcceptTypes,
    IN DWORD     dwFlags
) {
    if (IsLogEnabled("http"))
        PRINT_DEBUG("Mine_WinHttpOpenRequest %ls %ls\n", pwszVerb, pwszObjectName);
    return Real_WinHttpOpenRequest(hConnect, pwszVerb, pwszObjectName, pwszVersion, pwszReferrer, ppwszAcceptTypes, dwFlags);
}

BOOL(WINAPI* Real_WinHttpSendRequest)(
    IN HINTERNET hRequest,
    IN LPCWSTR   lpszHeaders,
    IN DWORD     dwHeadersLength,
    IN LPVOID    lpOptional,
    IN DWORD     dwOptionalLength,
    IN DWORD     dwTotalLength,
    IN DWORD_PTR dwContext
    ) = WinHttpSendRequest;


BOOL WINAPI Mine_WinHttpSendRequest(
    IN HINTERNET hRequest,
    IN LPCWSTR   lpszHeaders,
    IN DWORD     dwHeadersLength,
    IN LPVOID    lpOptional,
    IN DWORD     dwOptionalLength,
    IN DWORD     dwTotalLength,
    IN DWORD_PTR dwContext
) {
    if (IsLogEnabled("http"))
        PRINT_DEBUG("Mine_WinHttpSendRequest %p\n", hRequest);
    return Real_WinHttpSendRequest(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength, dwTotalLength, dwContext);
}

void HookWinHttp(void)
{
    DetourAttach(reinterpret_cast<PVOID*>(&Real_WinHttpConnect), reinterpret_cast<PVOID>(Mine_WinHttpConnect));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_WinHttpOpen), reinterpret_cast<PVOID>(Mine_WinHttpOpen));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_WinHttpOpenRequest), reinterpret_cast<PVOID>(Mine_WinHttpOpenRequest));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_WinHttpSendRequest), reinterpret_cast<PVOID>(Mine_WinHttpSendRequest));
}