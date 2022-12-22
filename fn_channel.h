#pragma once
#ifndef _FN_LOG_CHANNEL_H_
#define _FN_LOG_CHANNEL_H_
#include "fn_logger.h"
namespace FNLog
{
     void EnterProcDevice(Logger& logger, int channel_id, int device_id, LogData & log);
     void DispatchLog(Logger & logger, Channel& channel, LogData& log);
     void EnterProcChannel(Logger& logger, int channel_id);
     bool BlockInput(Logger& logger, int channel_id, int priority, int category, long long identify);
     int HoldChannel(Logger& logger, int channel_id, int priority, int category, long long identify);
     int PushChannel(Logger& logger, int channel_id, int hold_idx);
     int TransmitChannel(Logger& logger, int channel_id, const LogData& log);
     void InitLogData(Logger& logger, LogData& log, int channel_id, int priority, int category, unsigned long long identify, unsigned int prefix);
}
#endif
