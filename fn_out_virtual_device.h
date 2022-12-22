#pragma once
#ifndef _FN_LOG_OUT_VIRTUAL_DEVICE_H_
#define _FN_LOG_OUT_VIRTUAL_DEVICE_H_
#include "fn_atomic.h"
#include "fn_fmt.h"
namespace FNLog
{
    using VirtualDevicePtr = void (*)(const LogData& log);
    inline VirtualDevicePtr& RefVirtualDevice()
    {
        static VirtualDevicePtr g_virtual_device_ptr = NULL;
        return g_virtual_device_ptr;
    }
    inline void SetVirtualDevice(VirtualDevicePtr vdp)
    {
        RefVirtualDevice() = vdp;
    }
    inline void EnterProcOutVirtualDevice(Logger& logger, int channel_id, int device_id, LogData& log)
    {
        if (RefVirtualDevice())
        {
            Device& device = logger.shm_->channels_[channel_id].devices_[device_id];
            AtomicAddL(device, DEVICE_LOG_TOTAL_WRITE_LINE);
            AtomicAddLV(device, DEVICE_LOG_TOTAL_WRITE_BYTE, log.content_len_);
            (*RefVirtualDevice())(log);
        }
    }
}
#endif
