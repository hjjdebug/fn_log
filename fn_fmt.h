#pragma once
#include "my_define.h"
namespace FNLog
{
    struct PriorityRender
    {
        const char* const priority_name_;
        int priority_len_;
        const char* const scolor_;
    };

    static const PriorityRender PRIORITY_RENDER[] =		// 其它文件也会用
    {
        {"[TRACE]", sizeof("[TRACE]") - 1, "\e[0m",                     },
        {"[DEBUG]", sizeof("[DEBUG]") - 1, "\e[0m",                     },
        {"[INFO ]", sizeof("[INFO ]") - 1, "\e[34m\e[1m",/*hight blue*/},
        {"[WARN ]", sizeof("[WARN ]") - 1, "\e[33m", /*yellow*/         },
        {"[ERROR]", sizeof("[ERROR]") - 1, "\e[31m", /*red*/            },
        {"[ALARM]", sizeof("[ALARM]") - 1, "\e[32m", /*green*/          },
        {"[FATAL]", sizeof("[FATAL]") - 1, "\e[35m",       /*purple*/   },
    }; 
    static_assert(PRIORITY_TRACE == 0, "");
    static_assert(sizeof(PRIORITY_RENDER) / sizeof(PriorityRender) == PRIORITY_MAX, "");
    
    template<int WIDE>
    int write_bin_unsafe(char* dst, long long number);
    template<int WIDE>
    int write_bin_unsafe(char* dst, unsigned long long number);
    template<int WIDE>
    int write_hex_unsafe(char* dst, long long number);
    template<int WIDE>
    int write_hex_unsafe(char* dst, unsigned long long number);

    template<int WIDE>
    int write_dec_unsafe(char* dst, long long number);
    template<int WIDE>
    int write_dec_unsafe(char* dst, unsigned long long number);

    int write_double_unsafe(char* dst, double number);
    int write_float_unsafe(char* dst, float number);
    int write_date_unsafe(char* dst, long long timestamp, unsigned int precise);
    int write_log_priority_unsafe(char* dst, int priority);
    int write_log_thread_unsafe(char* dst, unsigned int thread_id);
    int write_pointer_unsafe(char* dst, const void* ptr);
}
