#pragma once

enum errorParm_t
{
  ERR_FATAL = 0x0,
  ERR_DROP = 0x1,
  ERR_SERVERDISCONNECT = 0x2,
  ERR_DISCONNECT = 0x3,
  ERR_SCRIPT = 0x4,
  ERR_SCRIPT_DROP = 0x5,
  ERR_LOCALIZATION = 0x6,
};

#define Com_Error(code, fmt, ...) Com_ErrorLine(__FILE__, __LINE__, code, fmt, __VA_ARGS__)

typedef void (__cdecl * Com_PrintError_t)(int channel, const char *fmt, ...);
static Com_PrintError_t Com_PrintError = (Com_PrintError_t)0x00522AE0;

typedef int (__cdecl * Com_sprintf_t)(char *dest, int size, const char *fmt, ...);
static Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x0056192B;

typedef void (__cdecl * Com_ErrorLine_t)(const char *file, int line, errorParm_t code, const char *fmt, ...);
static Com_ErrorLine_t Com_ErrorLine = (Com_ErrorLine_t)0x00522D0C;

typedef void (__cdecl * I_strncpyz_t)(char *dest, const char *src, int destsize);
static I_strncpyz_t I_strncpyz = (I_strncpyz_t)0x0056162F;

typedef int (__cdecl * I_strnicmp_t)(const char *s0, const char *s1, int n);
static I_strnicmp_t I_strnicmp = (I_strnicmp_t)0x00561652;

typedef void (__cdecl * Com_PrintWarning_t)(int channel, const char *fmt, ...);
static Com_PrintWarning_t Com_PrintWarning = (Com_PrintWarning_t)0x00522B66;