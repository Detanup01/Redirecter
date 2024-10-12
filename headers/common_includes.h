#pragma once

#define WIN32_LEAN_AND_MEAN

// C/C++ includes
#include <cstdint>
#include <algorithm>
#include <string>
#include <string_view>
#include <chrono>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iterator>

#include <vector>
#include <map>
#include <set>
#include <queue>
#include <list>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <string.h>
#include <stdio.h>
#include <filesystem>
#include <optional>
#include <numeric>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <processthreadsapi.h>
#include <direct.h>
#include <iphlpapi.h> // Include winsock2 before this, or winsock2 iphlpapi will be unavailable
#include <shlobj.h>

// we need this for BCryptGenRandom() in base.cpp
#include <bcrypt.h>
// we need non-standard S_IFDIR for Windows
#include <sys/stat.h>

#define MSG_NOSIGNAL 0

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define PATH_SEPARATOR "\\"

#include <winhttp.h>
#include <intsafe.h> // MinGW cannot find DWordMult()
#include "../detours/detours.h"

#include "ip2string.h"
#include "wininet.h"

#define PRINT_DEBUG(a, ...) do {FILE *t = fopen("Redirecter.txt", "a"); fprintf(t, "%u " a, GetCurrentThreadId(), __VA_ARGS__); fclose(t);} while (0)