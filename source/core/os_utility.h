#ifndef _OS_UTILITY_H_
#define _OS_UTILITY_H_

#include <compiler/ignored_warnings.h>

#ifdef _WIN32
#define NEW_LINE "\r\n"
#elif __linux__
#define NEW_LINE "\n"
#endif

#include <string>
#include <cstdlib>

#if __linux__
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <cstdio>
#include <cstddef>
#elif _WIN32
#include <windows.h>
#endif

#include <core/file_utility.h>

#define MAX_OS_LAST_ERROR_LENGTH 1024

namespace core
{

#if ! defined(UNIT_TEST)
inline void setCurrentWorkingDirectory(const std::string& dir)
{
    chdir(dir.c_str()); // Linux from unistd , Windows from windows.h
}
#endif

inline std::string getCurrentExecutableDirectory()
{
    std::string ret;
    const std::size_t tempBufferLength = 1024;
    char tempBuffer[tempBufferLength] = {char(NULL)};
#ifdef __linux__
    char procAddress[32];
    snprintf(procAddress, 32, "/proc/%d/exe", getpid());
    int bytes = readlink(procAddress, tempBuffer, tempBufferLength);
    tempBuffer[bytes] = '\0';
#elif _WIN32
    GetModuleFileName(NULL, tempBuffer, tempBufferLength);
#endif
    ret = tempBuffer;
    ret = core::getDirectoryNameFromPath(ret);
    return ret;
}

inline std::string getCurrentWorkingDirectory()
{
    std::string ret;
    char* temp = getenv("PATH");
    ret = temp;
    return ret;
}
inline std::string getLastError()
{
    std::string ret;
#ifdef __linux__
    ret = strerror(errno);
#elif _WIN32
    DWORD errCode = GetLastError();
    char *err;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        errCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
        (LPTSTR)&err,
        0,
        nullptr);

    ret = err;
#endif
    return ret;
}

inline bool amIAdmin()
{
    bool ret = false;
#ifdef __linux__
    if( ! getuid() )
    {
        ret = true;
    }
#elif _WIN32
#if ! _DEBUG
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
        {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken)
    {
        CloseHandle(hToken);
    }

    ret = fRet == TRUE ? true : false;
#else
    ret = true;
#endif
#endif
    return ret;
}

}// namespace

#endif