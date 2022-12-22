#pragma once
#ifndef _FN_LOG_CORE_H_
#define _FN_LOG_CORE_H_
#include "fn_channel.h"
namespace FNLog
{
     int PushLog(Logger& logger, int channel_id, int hold_idx, bool state_safly_env = false);
    //not thread-safe
     Channel* NewChannel(Logger& logger, int channel_type);
    //not thread-safe
     Device* NewDevice(Logger& logger, Channel& channel, int out_type);
     int StartChannels(Logger& logger);
     int StopChannels(Logger& logger);
     int CleanChannels(Logger& logger);
     long long GetChannelLog(Logger& logger, int channel_id, ChannelLogEnum field);
     long long GetDeviceLog(Logger& logger, int channel_id, int device_id, DeviceLogEnum field);
     void SetChannelConfig(Logger& logger, int channel_id, ChannelConfigEnum field, long long val);
     void SetDeviceConfig(Logger& logger, int channel_id, int device_id, DeviceConfigEnum field, long long val);
     long long GetDeviceConfig(Logger& logger, int channel_id, int device_id, DeviceConfigEnum field);
     void BatchSetChannelConfig(Logger& logger, ChannelConfigEnum cce, long long v);
     void BatchSetDeviceConfig(Logger& logger, DeviceOutType out_type, DeviceConfigEnum dce, long long v);
}
#endif
