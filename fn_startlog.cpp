#include "fn_startlog.h"
#include "fn_core.h"
#include "fn_load.h"
namespace FNLog
{
	Logger& GetDefaultLogger()
	{
		static Logger logger;
		return logger;
	}
	int FastStartDefaultLogger(const std::string& config_text)
	{
		int ret = ParseAndStartLogger(GetDefaultLogger(), config_text);
		if (ret != 0)
		{
			printf("fast start default logger error. ret:<%d>.\n", ret);
			return ret;
		}
		return 0;
	}
     int FastStartDefaultLogger()
    {
        static const std::string default_config_text =
            R"----(
 # default channel 0
   # write full log to pname.log 
   # write error log to pname_error.log 
   # view  info log to screen 
 # sync channel 1 
   # write full log to pname.log
   # write info log to pname_info.log
   # view  info log to screen 
 - channel: 0
    sync: async
    -device: 0
        disable: false
        out_type: file
        file: "$PNAME"
        rollback: 4
        limit_size: 100 m #only support M byte
    -device: 1
        disable: false
        out_type: file
        priority: error
        file: "$PNAME_error"
        rollback: 4
        limit_size: 100 m #only support M byte
    -device:2
        disable: false
        out_type: screen
        priority: info
 - channel: 1
    sync: sync
    -device: 0
        disable: false
        out_type: file
        file: "$PNAME_sync"
        rollback: 4
        limit_size: 100 m #only support M byte
    -device: 1
        disable: false
        out_type: file
        priority: info
        file: "$PNAME_sync_info"
        rollback: 4
        limit_size: 100 m #only support M byte
    -device:2
        disable: false
        out_type: screen
        priority: info 
)----";
        return FastStartDefaultLogger(default_config_text);
    }
	 int FastStartDebugLogger()
	 {
		 int ret = FastStartDefaultLogger();
		 if (ret == 0)
		 {
			 BatchSetDeviceConfig(GetDefaultLogger(), DEVICE_OUT_FILE, DEVICE_CFG_PRIORITY, PRIORITY_TRACE);
			 SetDeviceConfig(GetDefaultLogger(), 0, 1, DEVICE_CFG_PRIORITY, PRIORITY_ERROR); //error file is still error file    
			 BatchSetDeviceConfig(GetDefaultLogger(), DEVICE_OUT_SCREEN, DEVICE_CFG_PRIORITY, PRIORITY_DEBUG);
		 }
		 return ret;
	 }

	 int LoadAndStartDefaultLogger(const std::string& config_path)
	 {
		 int ret = LoadAndStartLogger(GetDefaultLogger(), config_path);
		 if (ret != 0)
		 {
			 printf("load auto start default logger error. ret:<%d>.\n", ret);
			 return ret;
		 }
		 return 0;
	 }

	 int StartLogger(Logger& logger)
	 {
		 if (logger.logger_state_ != LOGGER_STATE_UNINIT)
		 {
			 printf("start error. state:<%u> not uninit:<%u>.\n", logger.logger_state_, LOGGER_STATE_UNINIT);
			 return -1;
		 }
		 if (logger.shm_ == NULL || logger.shm_->channel_size_ > Logger::MAX_CHANNEL_SIZE || logger.shm_->channel_size_ <= 0)
		 {
			 printf("start error. channel size:<%d> invalid.\n", logger.shm_->channel_size_);
			 return -2;
		 }
		 Logger::StateLockGuard state_guard(logger.state_lock);
		 if (logger.logger_state_ != LOGGER_STATE_UNINIT)
		 {
			 printf("start error. state:<%u> not uninit:<%u>.\n", logger.logger_state_, LOGGER_STATE_UNINIT);
			 return -3;
		 }
		 if (logger.shm_->channel_size_ > Logger::MAX_CHANNEL_SIZE || logger.shm_->channel_size_ <= 0)
		 {
			 printf("start error. channel size:<%d> invalid.\n", logger.shm_->channel_size_);
			 return -4;
		 }
		 logger.logger_state_ = LOGGER_STATE_INITING;
		 if (StartChannels(logger) != 0)
		 {
			 StopChannels(logger);
			 logger.logger_state_ = LOGGER_STATE_UNINIT;
			 return -5;
		 }
		 logger.logger_state_ = LOGGER_STATE_RUNNING;
		 return 0;
	 }
	 int StopLogger(Logger& logger)
	 {
		 if (logger.shm_->channel_size_ > Logger::MAX_CHANNEL_SIZE || logger.shm_->channel_size_ <= 0)
		 {
			 printf("try stop error. channel size:<%d> invalid.\n", logger.shm_->channel_size_);
			 return -1;
		 }
		 if (logger.logger_state_ != LOGGER_STATE_RUNNING)
		 {
			 printf("try stop logger error. state:<%u> not running:<%u>.\n", logger.logger_state_, LOGGER_STATE_RUNNING);
			 return -2;
		 }
		 Logger::StateLockGuard state_guard(logger.state_lock);

		 if (logger.logger_state_ != LOGGER_STATE_RUNNING)
		 {
			 printf("try stop logger error. state:<%u> not running:<%u>.\n", logger.logger_state_, LOGGER_STATE_RUNNING);
			 return -3;
		 }
		 logger.logger_state_ = LOGGER_STATE_CLOSING;
		 StopChannels(logger);
		 CleanChannels(logger);

		 for (auto& writer : logger.file_handles_)
		 {
			 if (writer.is_open())
			 {
				 writer.close();
			 }
		 }
		 for (auto& writer : logger.udp_handles_)
		 {
			 if (writer.is_open())
			 {
				 writer.close();
			 }
		 }
		 logger.logger_state_ = LOGGER_STATE_UNINIT;
		 return 0;
	 }
	 int ParseAndStartLogger(Logger& logger, const std::string& config_content)
	 {
		 if (logger.logger_state_ != LOGGER_STATE_UNINIT)
		 {
			 printf("parse and start error. state:<%u> not uninit:<%u> by fast try.\n", logger.logger_state_, LOGGER_STATE_UNINIT);
			 return -1;
		 }
		 Logger::StateLockGuard state_guard(logger.state_lock);
		 if (logger.logger_state_ != LOGGER_STATE_UNINIT)
		 {
			 printf("parse and start error. state:<%u> not uninit:<%u> in locked check.\n", logger.logger_state_, LOGGER_STATE_UNINIT);
			 return -2;
		 }
		 int ret = InitFromYMAL(logger, config_content, "");
		 if (ret != 0)
		 {
			 printf("init and load default logger error. ret:<%d>.\n", ret);
			 return ret;
		 }
		 ret = StartLogger(logger);
		 if (ret != 0)
		 {
			 printf("start default logger error. ret:<%d>.\n", ret);
			 return ret;
		 }
		 return 0;
	 }
	 int LoadAndStartLogger(Logger& logger, const std::string& confg_path)
	 {
		 if (logger.logger_state_ != LOGGER_STATE_UNINIT)
		 {
			 printf("load and start error. state:<%u> not uninit:<%u>.\n", logger.logger_state_, LOGGER_STATE_UNINIT);
			 return -1;
		 }
		 Logger::StateLockGuard state_guard(logger.state_lock);
		 int ret = InitFromYMALFile(logger, confg_path);
		 if (ret != 0)
		 {
			 printf("init and load default logger error. ret:<%d>.\n", ret);
			 return ret;
		 }
		 ret = StartLogger(logger);
		 if (ret != 0)
		 {
			 printf("start default logger error. ret:<%d>.\n", ret);
			 return ret;
		 }
		 return 0;
	 }
}
