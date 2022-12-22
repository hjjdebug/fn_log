#include "filehandler.h"
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
namespace FNLog
{
    FileHandler::FileHandler()
    {
        file_ = nullptr;
    }
    FileHandler::~FileHandler()
    {
        close();
    }
    long FileHandler::open(const char* path, const char* mod, struct stat& file_stat)
    {
        if (file_ != nullptr)
        {
            fclose(file_);
            file_ = nullptr;
        }
        file_ = fopen(path, mod);
        if (file_)
        {
            if (fstat(fileno(file_), &file_stat) != 0)
            {
                fclose(file_);
                file_ = nullptr;
                return -1;
            }
            return file_stat.st_size;
        }
        return -2;
    }
    void FileHandler::close()
    {
        if (file_ != nullptr)
        {
#if !defined(__APPLE__) && !defined(WIN32) 
            if (file_ != nullptr)
            {
                int fd = fileno(file_);
                fsync(fd);
                posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
                fsync(fd);
            }
#endif
            fclose(file_);
            file_ = nullptr;
        }
    }
    bool FileHandler::is_open()
    {
        return file_ != nullptr;
    }
    void FileHandler::write(const char* data, size_t len)
    {
        if (file_ && len > 0)
        {
            if (fwrite(data, 1, len, file_) != len)
            {
                close();
            }
        }
    }
    void FileHandler::flush()
    {
        if (file_)
        {
            fflush(file_);
        }
    }
    std::string FileHandler::read_line()
    {
        char buf[500] = { 0 };
        if (file_ && fgets(buf, 500, file_) != nullptr)
        {
            return std::string(buf);
        }
        return std::string();
    }
    std::string FileHandler::read_content()
    {
        std::string content;
        if (!file_)
        {
            return content;
        }
        char buf[BUFSIZ];
        size_t ret = 0;
        do
        {
            ret = fread(buf, sizeof(char), BUFSIZ, file_);
            content.append(buf, ret);
        } while (ret == BUFSIZ);
        return content;
    }
    bool FileHandler::is_dir(const std::string& path)
    {
#ifdef WIN32
        return PathIsDirectoryA(path.c_str()) ? true : false;
#else
        DIR* pdir = opendir(path.c_str());
        if (pdir == nullptr)
        {
            return false;
        }
        else
        {
            closedir(pdir);
            pdir = nullptr;
            return true;
        }
#endif
    }
    bool FileHandler::is_file(const std::string& path)
    {
#ifdef WIN32
        return ::_access(path.c_str(), 0) == 0;
#else
        return ::access(path.c_str(), F_OK) == 0;
#endif
    }
    bool FileHandler::create_dir(const std::string& path)
    {
        if (path.length() == 0)
        {
            return true;
        }
        std::string sub;
        std::string::size_type pos = path.find('/');
        while (pos != std::string::npos)
        {
            std::string cur = path.substr(0, pos - 0);
            if (cur.length() > 0 && !is_dir(cur))
            {
                bool ret = false;
#ifdef WIN32
                ret = CreateDirectoryA(cur.c_str(), nullptr) ? true : false;
#else
                ret = (mkdir(cur.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
                if (!ret)
                {
                    return false;
                }
            }
            pos = path.find('/', pos + 1);
        }
        return true;
    }
    std::string FileHandler::process_id()
    {
        std::string pid = "0";
        char buf[260] = { 0 };
#ifdef WIN32
        DWORD winPID = GetCurrentProcessId();
        sprintf(buf, "%06u", winPID);
        pid = buf;
#else
        sprintf(buf, "%06d", getpid());
        pid = buf;
#endif
        return pid;
    }
    std::string FileHandler::process_name()
	{
		std::string name = "process";
		char buf[260] = { 0 };
#ifdef WIN32
		if (GetModuleFileNameA(nullptr, buf, 259) > 0)
		{
			name = buf;
		}
		std::string::size_type pos = name.rfind("\\");
		if (pos != std::string::npos)
		{
			name = name.substr(pos + 1, std::string::npos);
		}
		pos = name.rfind(".");
		if (pos != std::string::npos)
		{
			name = name.substr(0, pos - 0);
		}
#elif defined(__APPLE__)
		proc_name(getpid(), buf, 260);
		name = buf;
		return name;;
#else
		sprintf(buf, "/proc/%d/cmdline", (int)getpid()); //linux 从/proc/pid/cmdline文件中读取信息
		FileHandler i;
		struct stat file_stat;
		i.open(buf, "rb", file_stat);
		if (!i.is_open())
		{
			return name;
		}
		name = i.read_line();
		i.close();
		std::string::size_type pos = name.rfind("/");
		if (pos != std::string::npos)
		{
			name = name.substr(pos + 1, std::string::npos);	// 只返回名称，不包含路径
		}
#endif
		return name;
	}
    bool FileHandler::remove_file(const std::string & path)
    {
        return ::remove(path.c_str()) == 0;
    }
    struct tm FileHandler::time_to_tm(time_t t)
    {
#ifdef WIN32
#if _MSC_VER < 1400 //VS2003
        return *localtime(&t);
#else //vs2005->vs2013->
        struct tm tt = { 0 };
        localtime_s(&tt, &t);
        return tt;
#endif
#else //linux
        struct tm tt; /*= { 0 }; */
		memset(&tt,0,sizeof(tt));
        localtime_r(&t, &tt);
        return tt;
#endif
    }
    bool FileHandler::rollback(const std::string& path, int depth, int max_depth)
    {
        if (!is_file(path))
        {
            return true;
        }
        if (depth > max_depth)
        {
            return remove_file(path);
        }
        std::string next_path = path;
        size_t pos = path.find_last_not_of("0123456789");
        if (pos != std::string::npos && path.at(pos) == '.')
        {
            next_path = path.substr(0, pos - 0);
        }
        next_path += ".";
        next_path += std::to_string(depth);
        rollback(next_path, depth + 1, max_depth);
        int ret = ::rename(path.c_str(), next_path.c_str());
        (void)ret;
        return true;
    }
}
