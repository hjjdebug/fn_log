#pragma once
#include <string>
#include <array>
#include <thread>
#include <mutex>
#include <atomic>
#include "fn_atomic.h"	
#include "filehandler.h"
#include "udphandler.h"
#include "my_define.h"
namespace FNLog
{
    struct LogData
    {
    public:
        static const int LOG_SIZE = FN_LOG_MAX_LOG_SIZE;
    public:
        std::atomic_int    data_mark_; //0 invalid, 1 hold, 2 ready
        int     channel_id_;
        int     priority_;
        int     category_; 
        long long     identify_;
        int     code_line_;
        int     code_func_len_;
        int     code_file_len_;
        const char* code_func_;
        const char* code_file_;
        long long timestamp_;        //create timestamp
        int precise_; //create time millionsecond suffix
        unsigned int thread_;
        int prefix_len_;
        int content_len_;
        char content_[LOG_SIZE]; //content
    };
    struct Device
    {
    public:
        static const int MAX_PATH_SYS_LEN = 255;
        static const int MAX_PATH_LEN = 200;
        static const int MAX_LOGGER_NAME_LEN = 50;
        static const int MAX_ROLLBACK_LEN = 4;
        static const int MAX_ROLLBACK_PATHS = 5;
// static_assert 令ctags 分析不清楚，故注释掉.
//        static_assert(MAX_PATH_LEN + MAX_LOGGER_NAME_LEN + MAX_ROLLBACK_LEN < MAX_PATH_SYS_LEN, "");
//       static_assert(LogData::LOG_SIZE > MAX_PATH_SYS_LEN*2, "unsafe size"); // promise format length: date, time, source file path, function length.
//        static_assert(MAX_ROLLBACK_PATHS < 10, "");
#ifdef FN_LOG_USING_ATOM_CFG
        using ConfigFields = std::array<std::atomic_llong, DEVICE_CFG_MAX_ID>;
#else
        using ConfigFields = long long[DEVICE_CFG_MAX_ID];	//用的是该行定义,保留一些配置信息，如是否enable,优先级，rollback 等信息
#endif // FN_LOG_USING_ATOM_CFG
        using LogFields = std::array<std::atomic_llong, DEVICE_LOG_MAX_ID>; //保留LOG 的一些信息... 定义的太细了!
    public:
        int device_id_;
        unsigned int out_type_;
        char out_file_[MAX_LOGGER_NAME_LEN];
        char out_path_[MAX_PATH_LEN];
        ConfigFields config_fields_;
        LogFields log_fields_;
    };
    struct Channel
    {
    public:
#ifdef FN_LOG_USING_ATOM_CFG
        using ConfigFields = std::array<std::atomic_llong, CHANNEL_CFG_MAX_ID>;
#else
        using ConfigFields = long long[CHANNEL_CFG_MAX_ID];
#endif // FN_LOG_USING_ATOM_CFG
        using LogFields = std::array<std::atomic_llong, CHANNEL_LOG_MAX_ID>;
        static const int MAX_DEVICE_SIZE = 20;
    public:
        char chunk_1_[CHUNK_SIZE];
        int  channel_id_;
        int  channel_type_;
        unsigned int channel_state_;
        time_t yaml_mtime_;
        time_t last_hot_check_;
        int chunk_;
        int virtual_device_id_;
        int device_size_;
        Device devices_[MAX_DEVICE_SIZE]; //channel 包含device,最多20个
        ConfigFields config_fields_;
        LogFields log_fields_;
    };
    struct RingBuffer
    {
    public:
        static const int BUFFER_LEN = FN_LOG_MAX_LOG_QUEUE_SIZE;
    public:
        char chunk_1_[CHUNK_SIZE];
        std::atomic_int write_idx_;
        char chunk_2_[CHUNK_SIZE];
        std::atomic_int hold_idx_;
        char chunk_3_[CHUNK_SIZE];
        std::atomic_int read_idx_;
        char chunk_4_[CHUNK_SIZE];
        std::atomic_int proc_idx_;
        char chunk_5_[CHUNK_SIZE];
        LogData buffer_[BUFFER_LEN];
    };
    struct SHMLogger
    {
        static const int MAX_CHANNEL_SIZE = FN_LOG_MAX_CHANNEL_SIZE;
        using Channels = std::array<Channel, MAX_CHANNEL_SIZE>;
        using RingBuffers = std::array<RingBuffer, MAX_CHANNEL_SIZE>;
        int shm_id_;
        int shm_size_; 
        int channel_size_;
        Channels channels_;		//SHMLogger 包含channel, 包含RingBuffer
        RingBuffers ring_buffers_;
    };
    class Logger
    {
    public:
        static const int MAX_CHANNEL_SIZE = SHMLogger::MAX_CHANNEL_SIZE;
        static const int HOTUPDATE_INTERVEL = FN_LOG_HOTUPDATE_INTERVEL;
        static const int MAX_LOGGER_DESC_LEN = 50;
        static const int MAX_LOGGER_NAME_LEN = 250;
        using ReadLocks = std::array<std::mutex, MAX_CHANNEL_SIZE>;
        using ReadGuard = AutoGuard<std::mutex>;
        using AsyncThreads = std::array<std::thread, MAX_CHANNEL_SIZE>;
        using FileHandles = std::array<FileHandler, MAX_CHANNEL_SIZE* Channel::MAX_DEVICE_SIZE>;
        using UDPHandles = std::array<UDPHandler, MAX_CHANNEL_SIZE* Channel::MAX_DEVICE_SIZE>;
    public:
        using StateLock = std::recursive_mutex;
        using StateLockGuard = AutoGuard<StateLock>; //使用自动哨兵,状态递归锁
        using ScreenLock = std::mutex;
        using ScreenLockGuard = AutoGuard<ScreenLock>;
    public:
        Logger();
        ~Logger();
        bool hot_update_;
        std::string yaml_path_;
        unsigned int logger_state_;			//logger 状态
        StateLock state_lock;				// 声明一个状态锁变量
        char desc_[MAX_LOGGER_DESC_LEN];	//logger 的名称和描述是0字符结尾字符串
        int desc_len_;
        char name_[MAX_LOGGER_NAME_LEN];
        int name_len_;
        long long shm_key_;
        SHMLogger* shm_;					// share memory logger 是一个结构
        ReadLocks read_locks_;
        AsyncThreads async_threads;
        ScreenLock screen_lock_;
        FileHandles file_handles_;
        UDPHandles udp_handles_;
    };
}
