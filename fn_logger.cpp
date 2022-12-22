#include "fn_logger.h"
#include "fn_core.h"
#include "fn_load.h"
#include "fn_startlog.h"
namespace FNLog
{
     void InitLogger(Logger& logger)
    {
        logger.hot_update_ = false;
        logger.logger_state_ = LOGGER_STATE_UNINIT;
        memset(logger.desc_, 0, Logger::MAX_LOGGER_DESC_LEN);
        logger.desc_len_ = 0;
        memset(logger.name_, 0, Logger::MAX_LOGGER_NAME_LEN);
        logger.name_len_ = 0;
        std::string name = FileHandler::process_name();
        name = name.substr(0, Logger::MAX_LOGGER_NAME_LEN - 1);
        memcpy(logger.name_, name.c_str(), name.length() + 1);
        logger.name_len_ = (int)name.length();
        logger.shm_key_ = 0;
        logger.shm_ = NULL;
        
#if ((defined WIN32) && !KEEP_INPUT_QUICK_EDIT)
        HANDLE input_handle = ::GetStdHandle(STD_INPUT_HANDLE);
        if (input_handle != INVALID_HANDLE_VALUE)
        {
            DWORD mode = 0;
            if (GetConsoleMode(input_handle, &mode))
            {
                mode &= ~ENABLE_QUICK_EDIT_MODE;
                mode &= ~ENABLE_INSERT_MODE;
                mode &= ~ENABLE_MOUSE_INPUT;
                SetConsoleMode(input_handle, mode);
            }
        }
#endif
    }
	Logger::Logger()
	{
		InitLogger(*this);
	}
	Logger::~Logger()
	{
		while (logger_state_ != LOGGER_STATE_UNINIT)
		{
			int ret = StopLogger(*this);
			if (ret != 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		} ;
		UnloadSharedMemory(*this);
	}
}
