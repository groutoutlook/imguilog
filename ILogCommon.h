#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef MLS_EXPORT_LIBRARY
    #ifdef _WIN32
        #ifdef MLS_LIB_COMPILE
            #define MLS_PUBLIC_API __declspec(dllexport)
        #else
            #define MLS_PUBLIC_API __declspec(dllimport)
        #endif
    #else
        #define MLS_PUBLIC_API
    #endif
#else
    #define MLS_PUBLIC_API
#endif

    typedef enum [[maybe_unused]] ILog_LogColour
    {
        ILOG_LOG_COLOUR_GREEN = 0,
        ILOG_LOG_COLOUR_YELLOW = 1,
        ILOG_LOG_COLOUR_RED = 2,
        ILOG_LOG_COLOUR_WHITE = 3,
        ILOG_LOG_COLOUR_BLUE = 4,
        ILOG_LOG_COLOUR_NULL = 5
    } ILog_LogColour;

    typedef enum ILog_LogType
    {
        ILOG_LOG_TYPE_WARNING = 1,
        ILOG_LOG_TYPE_ERROR = 2,
        ILOG_LOG_TYPE_NOTE = 4,
        ILOG_LOG_TYPE_SUCCESS = 0,
        ILOG_LOG_TYPE_MESSAGE = 3
    } ILog_LogType;

    typedef enum [[maybe_unused]] ILog_LogOperations
    {
        // This is the default operation
        ILOG_LOG_OPERATION_TERMINAL,
        ILOG_LOG_OPERATION_FILE,
        ILOG_LOG_OPERATION_FILE_AND_TERMINAL,
    } ILog_LogOperations;

    typedef struct MLS_PUBLIC_API ILog_Timer
    {
        double duration;
        double startPos;
    } ILog_Timer;

#ifdef __cplusplus
}
#endif