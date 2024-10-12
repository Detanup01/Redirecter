#pragma once
#include "common_includes.h"
#include "../simpleini/SimpleIni.h"

void print_addrifno(addrinfo* addr);

void LoadIni(HMODULE hModule);

void print_addrinfo(addrinfo* addr);

bool CheckDNS(PCSTR dnsName, std::string* ip);

bool IsLogEnabled();

bool IsLogEnabled(std::string str);