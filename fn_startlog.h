#pragma once
#include "fn_logger.h"
namespace FNLog
{
     Logger& GetDefaultLogger();
     int FastStartDefaultLogger(const std::string& config_text);
     int FastStartDefaultLogger();
     int FastStartDebugLogger(); //未用,调试?
     int LoadAndStartDefaultLogger(const std::string& config_path); //未用

     int StartLogger(Logger& logger);
     int StopLogger(Logger& logger);
     int ParseAndStartLogger(Logger& logger, const std::string& config_content);
     int LoadAndStartLogger(Logger& logger, const std::string& confg_path);
}
