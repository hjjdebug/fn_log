#pragma once
#ifndef _FN_LOG_LOAD_H_
#define _FN_LOG_LOAD_H_
#include "fn_logger.h"
namespace FNLog
{
    int LoadSharedMemory(Logger& logger);
    void UnloadSharedMemory(Logger& logger);
    int InitFromYMAL(Logger& logger, const std::string& text, const std::string& path);
    int InitFromYMALFile(Logger& logger, const std::string& path);
    int HotUpdateLogger(Logger& logger, int channel_id);
}
#endif
