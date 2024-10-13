#include "headers/INISettings.h"

std::map<std::string, std::string> DNSs = {};
std::map<std::string, std::string> Redirection_Ips = {};
std::map<std::string, std::string> Contains_DNSs = {};
std::map<USHORT, USHORT> Ports = {};
bool Loaded = false;
CSimpleIniA ini;

bool IsLogEnabled()
{
    if (!Loaded)
        return false;
    return ini.GetBoolValue("main", "logs");
}

bool IsLogEnabled(std::string str)
{
    if (!Loaded)
        return false;
    return ini.GetBoolValue("log", str.c_str());
}

/// <summary>
/// Printing AddrInfo Information
/// </summary>
/// <param name="addr">addrinfo pointer</param>
void print_addrinfo(addrinfo* addr)
{
    struct sockaddr_in* sockaddr_ipv4;
    struct sockaddr_in6* sockaddr_ipv6;
    char ipstringbuffer[46];
    std::string detailedmsg{};
    switch (addr->ai_family)
    {
        case AF_INET:
            sockaddr_ipv4 = (struct sockaddr_in*)addr->ai_addr;
            RtlIpv4AddressToStringA(&sockaddr_ipv4->sin_addr, ipstringbuffer);
            detailedmsg += std::format("Address: {}:{} | {}", ipstringbuffer, sockaddr_ipv4->sin_port, htons(sockaddr_ipv4->sin_port));
            break;
        case AF_INET6:
            sockaddr_ipv6 = (struct sockaddr_in6*)addr->ai_addr;
            RtlIpv6AddressToStringA(&sockaddr_ipv6->sin6_addr, ipstringbuffer);
            detailedmsg += std::format("Address: {}:{}", ipstringbuffer, sockaddr_ipv6->sin6_port);
            break;
        default:
            detailedmsg += std::format("Address: Unknown ({})", addr->ai_family);
            break;
    }
    detailedmsg += std::format("| {} {} {} {} {} ", addr->ai_flags, addr->ai_socktype, addr->ai_family, addr->ai_protocol, addr->ai_addrlen);
    if (IsLogEnabled("printaddr"))
        PRINT_DEBUG("print_addrinfo %s\n", detailedmsg.c_str());
}

/// <summary>
/// Printing SockAddr Information
/// </summary>
/// <param name="addr">sockaddr pointer</param>
void print_sockaddr(const sockaddr* addr)
{
    struct sockaddr_in* sockaddr_ipv4;
    struct sockaddr_in6* sockaddr_ipv6;
    char ipstringbuffer[46];
    std::string detailedmsg{};
    switch (addr->sa_family)
    {
        case AF_INET:
            sockaddr_ipv4 = (struct sockaddr_in*)addr;
            RtlIpv4AddressToStringA(&sockaddr_ipv4->sin_addr, ipstringbuffer);
            detailedmsg += std::format("Address(v4): {}:{}", ipstringbuffer, htons(sockaddr_ipv4->sin_port));
            break;
        case AF_INET6:
            sockaddr_ipv6 = (struct sockaddr_in6*)addr;
            RtlIpv6AddressToStringA(&sockaddr_ipv6->sin6_addr, ipstringbuffer);
            detailedmsg += std::format("Address(v6): {}:{} | {} {}", ipstringbuffer, htons(sockaddr_ipv6->sin6_port), sockaddr_ipv6->sin6_scope_id, sockaddr_ipv6->sin6_flowinfo);
            break;
        default:
            detailedmsg += std::format("Address: Unknown ({})", addr->sa_family);
            break;
    }
    if (IsLogEnabled("printaddr"))
        PRINT_DEBUG("print_sockaddr %s\n", detailedmsg.c_str());
}

bool getInAddr(const sockaddr* addr, sockaddr_in** sockaddr_ipv4, sockaddr_in6** sockaddr_ipv6)
{
    switch (addr->sa_family)
    {
        case AF_INET:
            *sockaddr_ipv4 = (struct sockaddr_in*)addr;
            return true;
        case AF_INET6:
            *sockaddr_ipv6 = (struct sockaddr_in6*)addr;
            return true;
        default:
            return false;
    }
}

void LoadIni(HMODULE hModule)
{
    if (Loaded)
        return;
    std::string PathAndName;
    const int BUFSIZE = 4096;
    char buffer[BUFSIZE];
    if (GetModuleFileNameA(hModule, buffer, BUFSIZE - 1) <= 0)
        return;
    PathAndName = buffer;
    size_t found = PathAndName.find_last_of("/\\");
    PathAndName = PathAndName.substr(0, found);
    std::string str{ PathAndName.c_str() };
    str.append("\\redirect.ini");
    ini.SetUnicode();

    SI_Error rc = ini.LoadFile(str.c_str());
    if (rc < 0)
        return;

    CSimpleIniA::TNamesDepend keys;
    if (ini.GetAllKeys("dns", keys))
    {
        CSimpleIniA::TNamesDepend::const_iterator it;
        for (it = keys.begin(); it != keys.end(); ++it)
        {
            std::string dns_name = { it->pItem };
            std::string dns_val = { ini.GetValue("dns", it->pItem) };
            DNSs.insert(make_pair(dns_name, dns_val));
            PRINT_DEBUG("DNSs: %s %s\n", dns_name.c_str(), dns_val.c_str());
        }
    }
    if (ini.GetAllKeys("dns_contains", keys))
    {
        CSimpleIniA::TNamesDepend::const_iterator it;
        for (it = keys.begin(); it != keys.end(); ++it)
        {
            std::string dns_name = { it->pItem };
            std::string dns_val = { ini.GetValue("dns_contains", it->pItem) };
            Contains_DNSs.insert(make_pair(dns_name, dns_val));
            PRINT_DEBUG("Contains_DNSs: %s %s\n", dns_name.c_str(), dns_val.c_str());
        }
    }
    if (ini.GetAllKeys("redirection", keys))
    {
        CSimpleIniA::TNamesDepend::const_iterator it;
        for (it = keys.begin(); it != keys.end(); ++it)
        {
            std::string ip_name = { it->pItem };
            std::string ip_val = { ini.GetValue("redirection", it->pItem) };
            Redirection_Ips.insert(make_pair(ip_name, ip_val));
            PRINT_DEBUG("Redirection_Ips: %s %s\n", ip_name.c_str(), ip_val.c_str());
        }
    }
    if (ini.GetAllKeys("port", keys))
    {
        CSimpleIniA::TNamesDepend::const_iterator it;
        for (it = keys.begin(); it != keys.end(); ++it)
        {
            USHORT port_name = (USHORT)strtoul(it->pItem, NULL, 0);
            USHORT port_val = (USHORT)strtoul(ini.GetValue("port", it->pItem), NULL, 0);
            Ports.insert(std::make_pair(port_name, port_val));
            PRINT_DEBUG("Ports: %hi %hi\n", port_name, port_val);
        }
    }
    Loaded = true;
}

bool CheckDNS(PCSTR dnsName, std::string* ip)
{
    std::string dns_str(dnsName);

    for (const auto& dns_pair : Contains_DNSs)
    {
        if (dns_str.contains(dns_pair.first))
        {
            if (IsLogEnabled())
                PRINT_DEBUG("[INI] Contains DNS FOUND: %s %s %s\n", dns_pair.first.c_str(), dns_str.c_str(), dns_pair.second.c_str());
            *ip = dns_pair.second.c_str();
            return true;
        }
    }

    for (const auto& dns_pair : DNSs)
    {
        if (dns_pair.first.contains(dns_str))
        {
            if (IsLogEnabled())
                PRINT_DEBUG("[INI] DNS FOUND: %s %s %s\n", dns_pair.first.c_str(), dns_str.c_str(), dns_pair.second.c_str());
            *ip = dns_pair.second.c_str();
            return true;
        }
    }
    return false;
    
}

bool IpPortRedirection(const sockaddr** addr)
{
    sockaddr_in* sockaddr_ipv4 = NULL;
    sockaddr_in6* sockaddr_ipv6 = NULL;
    if (getInAddr(*addr, &sockaddr_ipv4, &sockaddr_ipv6))
    {
        if (sockaddr_ipv4 != NULL)
        {
            std::string ipv4 = inet_ntoa(sockaddr_ipv4->sin_addr);
            for (const auto& ip_pair : Redirection_Ips)
            {
                if (ip_pair.first.contains(ipv4))
                {
                    PRINT_DEBUG("[INI] IP Redirection FOUND: %s %s %s\n", ip_pair.first.c_str(), ipv4.c_str(), ip_pair.second.c_str());
                    inet_pton(AF_INET, ip_pair.second.c_str(), &(sockaddr_ipv4->sin_addr));
                }
            }

            for (const auto& port_pair : Ports)
            {
                if (htons(sockaddr_ipv4->sin_port) == port_pair.first)
                {
                    PRINT_DEBUG("[INI] Port FOUND: %d %d %d\n", port_pair.first, sockaddr_ipv4->sin_port, port_pair.second);
                    sockaddr_ipv4->sin_port = htons(port_pair.second);
                }
            }
            *addr = (const sockaddr*)sockaddr_ipv4;
            return true;
        }
        if (sockaddr_ipv6 != NULL)
        {

        }
    }
    return false;
}


