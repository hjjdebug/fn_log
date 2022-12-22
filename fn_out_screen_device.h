#pragma once
#ifndef _FN_LOG_OUT_SCREEN_DEVICE_H_
#define _FN_LOG_OUT_SCREEN_DEVICE_H_
#include "fn_fmt.h"
#include "fn_logger.h"
namespace FNLog
{
    inline void EnterProcOutScreenDevice(Logger& logger, int channel_id, int device_id, LogData& log)
    {
        Logger::ScreenLockGuard l(logger.screen_lock_);
        Device& device = logger.shm_->channels_[channel_id].devices_[device_id];
        AtomicAddL(device, DEVICE_LOG_TOTAL_WRITE_LINE);
        AtomicAddLV(device, DEVICE_LOG_TOTAL_WRITE_BYTE, log.content_len_);
        int priority = log.priority_;
        if (log.priority_ < PRIORITY_INFO)
        {
            printf("%s", log.content_);
            return;
        }
        if (priority >= PRIORITY_MAX)
        {
            priority = PRIORITY_ALARM;
        }
        printf("%s%s\e[0m", PRIORITY_RENDER[priority].scolor_, log.content_);
    }
}
#endif
