// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <type_traits>
#define _X86_
#include <minwindef.h>
#include <Winerror.h>
#include <libloaderapi.h>
#define LSTATUS LONG
typedef ACCESS_MASK REGSAM;

struct Data {
    enum{
        dowrd, string
    } type;
    const char* name;
    union {
        DWORD d;
        const char* s;
    };
};
char _inmmserv_name[500];
Data data[] = {
    {Data::dowrd, "Version", 0x238},
    {Data::dowrd, "WA_Hide", 0},
    {Data::dowrd, "WA_SkipWaveOutCheck", 0},
    {Data::dowrd, "SMX_Hide", 0},
    {Data::dowrd, "DCDP_Hide", 0},
    {Data::dowrd, "DCDP_SkipWaveOutCheck", 0},
    {Data::dowrd, "DCDP_EmulatePositionFunc", 0x1},
    {Data::dowrd, "Lilith_Hide", 0},
    {Data::dowrd, "ErrDispLevel", 0x2},
    {Data::dowrd, "ErrLogLevel", 0x3},
    {Data::dowrd, "NoticeLogged", 0x1},
    {Data::dowrd, "SE_Priority", 0x1},
    {Data::dowrd, "Enabled", 0x1},

    {Data::dowrd, "Player0", 0x6},
    {Data::dowrd, "Player1", 0x6},
    {Data::dowrd, "Player2", 0x1},
    {Data::dowrd, "Player3", 0},
    {Data::dowrd, "Player4", 0},
    {Data::dowrd, "Player5", 0},
    {Data::dowrd, "default", 0x2},
    {Data::dowrd, "cdplayer", 0x1},
    {Data::dowrd, "cddrive", 0},


    {.type = Data::string, .name = "_inmmserv", .s = _inmmserv_name},
    {.type = Data::string, .name = "WA_Path", .s = ""},
    {.type = Data::string, .name = "SMX_Path", .s = ""},
    {.type = Data::string, .name = "DCDP_Path", .s = ""},
    {.type = Data::string, .name = "Lilith_Path", .s = ""},
    {.type = Data::string, .name = "LogName", .s = R"(_inmm.log)"},

    {.type = Data::string, .name = "FileMask0", .s = R"(*.wav)"},
    {.type = Data::string, .name = "FileMask1", .s = R"(*.mp3)"},
    {.type = Data::string, .name = "FileMask2", .s = R"(*.mid|*.rmi)"},
    {.type = Data::string, .name = "FileMask3", .s = ""},
    {.type = Data::string, .name = "FileMask4", .s = ""},
    {.type = Data::string, .name = "FileMask5", .s = ""},

};


void my_memcpy(void* dst, const void* src, size_t size) {
    unsigned char* curr_dst = (unsigned char*)dst;
    const unsigned char* curr_src = (const unsigned char*)src;
    for (int i = 0; i != size; ++i) {
        curr_dst[i] = curr_src[i];
    }
}

int my_strlen(const char* lhs) {
    auto* curr = lhs;
    for (; *curr; ++curr);
    return curr - lhs;
}

char* my_strrchr(char* src, char ch) {
    auto lhs_len = my_strlen(src);

    char* curr = src + lhs_len; // search '\0'

    for (; curr >= src && *curr != ch; --curr);

    return curr;
}
// only equal return 0
int my_strcmp(const char* lhs, const char* rhs) {
    auto lhs_len = my_strlen(lhs);
    auto rhs_len = my_strlen(rhs);
    if (lhs_len != rhs_len) return -1;

    for (int i = 0; i != lhs_len; ++i) {
        if(lhs[i] != rhs[i]) return -1;
    }
    return 0;
}



extern "C" {
    BOOL APIENTRY DllMain(HMODULE hModule,
        DWORD  ul_reason_for_call,
        LPVOID lpReserved
    )
    {
        GetModuleFileNameA(NULL, _inmmserv_name, 500);
        auto* curr = my_strrchr(_inmmserv_name, '\\');
        my_memcpy(curr, "\\_inmmserv.exe", 15);

        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
        }
        return TRUE;
    }


    __declspec(dllexport) LSTATUS APIENTRY RegOpenKeyExA(
        HKEY   hKey,
        LPCSTR lpSubKey,
        DWORD  ulOptions,
        REGSAM samDesired,
        PHKEY  phkResult
    ) {
        return ERROR_SUCCESS;
    }

    __declspec(dllexport) LSTATUS APIENTRY RegCloseKey(
        HKEY hKey
    ) {
        return ERROR_SUCCESS;
    }

    __declspec(dllexport) LSTATUS APIENTRY RegQueryValueExA(
        HKEY    hKey,
        LPCSTR  lpValueName,
        LPDWORD lpReserved,
        LPDWORD lpType,
        LPBYTE  lpData,
        LPDWORD lpcbData
    ) {
        if (!lpValueName) {
            return ERROR_FILE_NOT_FOUND;
        }
        bool failure = true;
        for (auto i = 0; i != std::extent_v<decltype(data)>; ++i) {
            if (my_strcmp(lpValueName, data[i].name) == 0) {
                failure = false;
                const void* ptr;
                int len = 4;
                if (data[i].type == Data::dowrd) {
                    ptr = &data[i].d;
                }
                else {
                    ptr = data[i].s;
                    len = my_strlen(data[i].s) + 1;
                }
                my_memcpy(lpData, ptr, len);
                *lpcbData = len;
            }
        }
        if (failure) {
            return ERROR_FILE_NOT_FOUND;
        }
        return ERROR_SUCCESS;
    }
}