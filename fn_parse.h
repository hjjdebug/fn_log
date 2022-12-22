#pragma once
#ifndef _FN_LOG_PARSE_H_
#define _FN_LOG_PARSE_H_
#include "fn_logger.h"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
namespace FNLog
{
    enum ParseErrorCode
    {
        PEC_NONE, 
        PEC_ERROR,
        PEC_ILLEGAL_CHARACTER,
        PEC_ILLEGAL_KEY,
        PEC_NOT_CLOSURE,
        PEC_ILLEGAL_ADDR_IP,
        PEC_ILLEGAL_ADDR_PORT,
        PEC_UNDEFINED_DEVICE_KEY,
        PEC_UNDEFINED_DEVICE_TYPE,
        PEC_UNDEFINED_CHANNEL_KEY,
        PEC_UNDEFINED_GLOBAL_KEY,
        PEC_DEVICE_NOT_ARRAY,
        PEC_DEVICE_INDEX_OUT_MAX,
        PEC_DEVICE_INDEX_NOT_SEQUENCE,
 
        PEC_CHANNEL_NOT_ARRAY,
        PEC_CHANNEL_INDEX_OUT_MAX,
        PEC_CHANNEL_INDEX_NOT_SEQUENCE,
        PEC_NO_ANY_CHANNEL,
    };
    enum LineType
    {
        LINE_NULL,		//开始状态
        LINE_ARRAY,		//其它状态,要求以'-'开始
        LINE_BLANK,		//空行,例如#开始
        LINE_EOF,		//文件尾
    };
    enum BlockType
    {
        BLOCK_NULL,		//开始状态
        BLOCK_PRE_KEY,  //遇到'-' block转为PRE_KEY 状态,LINE 转为ARRAY状态
        BLOCK_KEY,		//遇到正常字符(key键可以开始的字符)，转为key状态
        BLOCK_PRE_SEP, //遇到分割字符，转变到key-value 分割状态, 常用的是':‘字符
        BLOCK_PRE_VAL,	//遇到':’字符，可转为VAL 状态
        BLOCK_VAL,		//正常字符，由PRE_VAL 可转至VAL状态
        BLOCK_CLEAN,	//当遇到\r\n'#'或0时，转变到CLEAN状态
    };
    enum ReseveKey
    {
        RK_NULL,
        RK_SHM_KEY,
        RK_CHANNEL,
        RK_DEVICE,
        RK_SYNC,
        RK_DISABLE,
        RK_HOT_UPDATE,
        RK_LOGGER_NAME,
        PK_LOGGER_DESC,
        RK_PRIORITY,
        RK_CATEGORY,
        RK_CATEGORY_EXTEND,
        RK_CATEGORY_FILTER,
        RK_IDENTIFY,
        RK_IDENTIFY_EXTEND,
        RK_IDENTIFY_FILTER,
        RK_OUT_TYPE,
        RK_FILE,
        RK_PATH,
        RK_LIMIT_SIZE,
        RK_ROLLBACK,
        RK_FILE_STUFF_UP,
        RK_UDP_ADDR,
    };
     ReseveKey ParseReserve(const char* begin, const char* end);
     LogPriority ParsePriority(const char* begin, const char* end);
     bool ParseBool(const char* begin, const char* end);
     long long ParseNumber(const char* begin, const char* end);
     bool ParseString(const char* begin, const char* end, char * buffer, int buffer_len, int& write_len);
     ChannelType ParseChannelType(const char* begin, const char* end);
     DeviceOutType ParseOutType(const char* begin, const char* end);
     std::pair<long long, const char*> ParseAddresIP(const char* begin, const char* end);
     void ParseAddres(const char* begin, const char* end, long long & ip, long long& port);
     unsigned long long ParseBitArray(const char* begin, const char* end);
    struct Line
    {
        int blank_;
        int line_type_;	//行类型是什么? 注释行或者其它类型
        int block_type_; //块类型指明具体的数据类型
        int key_;		//哪种保留的key?
        const char* key_begin_;
        const char* key_end_;
        const char* val_begin_;
        const char* val_end_;
    };
    struct LexState
    {
        int line_number_;
        const char* first_;
        const char* current_;
        const char* end_;
        Line line_;
        SHMLogger::Channels channels_;
        int channel_size_;
        long long shm_key_;
        bool hot_update_;
        char desc_[Logger::MAX_LOGGER_DESC_LEN];
        int desc_len_;
        char name_[Logger::MAX_LOGGER_NAME_LEN];
        int name_len_;
    };
     void InitState(LexState& state);
     int Lex_Line(LexState& ls);
     int ParseDevice(LexState& ls, Device& device, int indent);
     int ParseChannel(LexState& ls, Channel& channel, int indent);
     int ParseLogger(LexState& ls, const std::string& text);
}
#endif
