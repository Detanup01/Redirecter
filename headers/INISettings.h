#pragma once
#include "common_includes.h"
#include "../simpleini/SimpleIni.h"

void print_addrifno(addrinfo* addr);

void LoadIni(HMODULE hModule);

void print_addrinfo(addrinfo* addr);

void print_sockaddr(const sockaddr* addr);

bool CheckDNS(PCSTR dnsName, std::string* ip);

bool IpPortRedirection(const sockaddr** addr);

bool IsLogEnabled();

bool IsLogEnabled(std::string str);