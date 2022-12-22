#pragma once
#include <string>
#include <sys/types.h> //use struct stat
#include <sys/stat.h>
namespace FNLog
{
    static const int CHUNK_SIZE = 128;
    class FileHandler
    {
    public:
        FileHandler();
        ~FileHandler();
        bool is_open();
        long open(const char* path, const char* mod, struct stat& file_stat);
        void close();
        void write(const char* data, size_t len);
        void flush();
        std::string read_line();
        std::string read_content();
        static bool is_dir(const std::string & path);
        static bool is_file(const std::string & path);
        static bool create_dir(const std::string& path);
        static std::string process_id();
        static std::string process_name();
        static bool remove_file(const std::string& path);
        static struct tm time_to_tm(time_t t);
        static bool rollback(const std::string& path, int depth, int max_depth);
    public:
        char chunk_1_[128];
        FILE* file_;
    };
    inline int short_path(const char* path, int len)
    {
        int count = 3;
        if (path == nullptr || len <= 0)
        {
            return 0;
        }
        const char* last = path + len;
        while (last-- != path)
        {
            if (*last == '/' || *last == '\\')
            {
                if (--count <= 0)
                {
                    return (int)(last - path + 1);
                }
            }
        }
        return 0;
    }
}
