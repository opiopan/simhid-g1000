//-------------------------------------------------------------------------
//  olog.h
//-------------------------------------------------------------------------

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OLOG_FATAL      0
#define OLOG_ERROR      1
#define OLOG_WARNING    2
#define OLOG_INFO       3
#define OLOG_DEBUG      4

#ifndef OLOG_LEVEL
#define OLOG_LEVEL OLOG_INFO
#endif

#define OLOG_BACKEND_SEGGER_RTT 1

#if defined(OLOG_SEGGER_RTT)
#define OLOG_BACKEND OLOG_BACKEND_SEGGER_RTT
#else
#define OLOG_BACKEND OLOG_BACKEND_SEGGER_RTT
#endif

#if defined(OLOG_DISABLE)
#define olog_init()
#define olog_write(...)
#define olog_printf(...)
#define olog_vprintf(...)
#define olog_log(...)
#define olog_dumpmem(...)
#elif OLOG_BACKEND == OLOG_BACKEND_SEGGER_RTT
#include <SEGGER_RTT.h>
#define olog_init() SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP)
#define olog_write(buf, len) SEGGER_RTT_Write(0, (buf), (len))
#define olog_printf(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#define olog_vprintf(fmt, va_list) SEGGER_RTT_vprintf(0, (fmt), (va_list))
void olog_log(int level, const char* fmt, ...);
void olog_dumpmem(const void* mem, size_t len);
#else
#define olog_init()
#define olog_write(...)
#define olog_printf(...)
#define olog_vprintf(...)
#define olog_log(...)
#define olog_dumpmem(...)
#endif

#if OLOG_LEVEL >= OLOG_FATAL
#define OLOG_LOGF(...) olog_log(OLOG_FATAL, __VA_ARGS__)
#else
#define OLOG_LOGF(...)
#endif

#if OLOG_LEVEL >= OLOG_ERROR
#define OLOG_LOGE(...) olog_log(OLOG_ERROR, __VA_ARGS__)
#else
#define OLOG_LOGE(...)
#endif

#if OLOG_LEVEL >= OLOG_WARNING
#define OLOG_LOGW(...) olog_log(OLOG_WARNING, __VA_ARGS__)
#else
#define OLOG_LOGW(...)
#endif

#if OLOG_LEVEL >= OLOG_INFO
#define OLOG_LOGI(...) olog_log(OLOG_INFO, __VA_ARGS__)
#else
#define OLOG_LOGI(...)
#endif

#if OLOG_LEVEL >= OLOG_DEBUG
#define OLOG_LOGD(...) olog_log(OLOG_DEBUG, __VA_ARGS__)
#else
#define OLOG_LOGD(...)
#endif

#ifdef __cplusplus
}
#endif
