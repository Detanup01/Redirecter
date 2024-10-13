#include "../headers/aWS2.h"
#include "../headers/common_includes.h"
#include "../headers/INISettings.h"

#pragma comment(lib, "ws2_32.lib")

int (WINAPI* Real_Connect)(SOCKET s, const sockaddr* addr, int namelen) = connect;
int (WINAPI* Real_WSAConnect)(SOCKET s, const sockaddr* addr, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS) = WSAConnect;
INT (WINAPI* Real_getaddrinfo)(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA* pHints, PADDRINFOA* ppResult) = getaddrinfo;

int WINAPI Mine_Connect(SOCKET s, const sockaddr* addr, int namelen)
{
    if (IsLogEnabled("ws2"))
        PRINT_DEBUG("Connect\n");
    print_sockaddr(addr);
    if (IpPortRedirection(&addr))
        print_sockaddr(addr);
    return Real_Connect(s, addr, namelen);
}

int WINAPI Mine_WSAConnect(SOCKET s, const sockaddr* addr, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS)
{
    if (IsLogEnabled("ws2"))
        PRINT_DEBUG("WSAConnect\n");
    print_sockaddr(addr);
    if (IpPortRedirection(&addr))
        print_sockaddr(addr);
    return Real_WSAConnect(s, addr, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS);
}

INT WINAPI Mine_getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA* pHints, PADDRINFOA* ppResult)
{
    if (IsLogEnabled("ws2"))
        PRINT_DEBUG("getaddrinfo %s %s \n", pNodeName, pServiceName);
    std::string Ip;
    if (CheckDNS(pNodeName, &Ip))
    {
        if (IsLogEnabled("ws2"))
            PRINT_DEBUG("getaddrinfo redirected to: %s \n", Ip.c_str());
        return Real_getaddrinfo(Ip.c_str(), pServiceName, pHints, ppResult);
    }
    return Real_getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
}


void HookWS2(void)
{
    DetourAttach(reinterpret_cast<PVOID*>(&Real_Connect), reinterpret_cast<PVOID>(Mine_Connect));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_WSAConnect), reinterpret_cast<PVOID>(Mine_WSAConnect));
    DetourAttach(reinterpret_cast<PVOID*>(&Real_getaddrinfo), reinterpret_cast<PVOID>(Mine_getaddrinfo));
}