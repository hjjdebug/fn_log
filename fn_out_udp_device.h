#pragma once
#include "fn_atomic.h"
#include "fn_fmt.h"
namespace FNLog
{
    inline void EnterProcOutUDPDevice(Logger& logger, int channel_id, int device_id, LogData& log)
    {
        auto& udp = logger.udp_handles_[channel_id * Channel::MAX_DEVICE_SIZE + device_id];
        if (!udp.is_open())
        {
            udp.open();
        }
        if (!udp.is_open())
        {
            return;
        }
        Device& device = logger.shm_->channels_[channel_id].devices_[device_id];
        long long ip = AtomicLoadC(device, DEVICE_CFG_UDP_IP);
        long long port = AtomicLoadC(device, DEVICE_CFG_UDP_PORT);
        udp.write((unsigned long)ip, (unsigned short)port, log.content_, log.content_len_);
        AtomicAddL(device, DEVICE_LOG_TOTAL_WRITE_LINE);
        AtomicAddLV(device, DEVICE_LOG_TOTAL_WRITE_BYTE, log.content_len_);
    }
}
