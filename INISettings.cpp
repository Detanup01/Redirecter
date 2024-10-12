#include "headers/INISettings.h"

std::map<std::string, std::string> DNSs = {};
std::map<std::string, std::string> Redirection_Ips = {};
std::map<std::string, std::string> Contains_DNSs = {};
std::map<int, int> Ports = {};
bool Loaded = false;
CSimpleIniA ini;

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
        detailedmsg += std::format("Address: {}:{}", inet_ntoa(sockaddr_ipv4->sin_addr), sockaddr_ipv4->sin_port);
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
    if (IsLogEnabled("addrinfo"))
        PRINT_DEBUG("print_addrinfo %s\n", detailedmsg.c_str());
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

    const char* network_serverIp;
    network_serverIp = ini.GetValue("network", "serverIp");

    CSimpleIniA::TNamesDepend keys;
    if (ini.GetAllKeys("dns", keys))
    {
        CSimpleIniA::TNamesDepend::const_iterator it;
        for (it = keys.begin(); it != keys.end(); ++it)
        {
            std::string dns_name = { it->pItem };
            std::string dns_val = { ini.GetValue("dns", it->pItem) };
            DNSs.insert(make_pair(dns_name, dns_val));
            if (IsLogEnabled())
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
            if (IsLogEnabled())
                PRINT_DEBUG("Contains_DNSs: %s %s\n", dns_name.c_str(), dns_val.c_str());
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

bool IsLogEnabled()
{
    if (!Loaded)
        return false;
    return ini.GetBoolValue("main","logs");
}

bool IsLogEnabled(std::string str)
{
    if (!Loaded)
        return false;
    return ini.GetBoolValue("log", str.c_str());
}