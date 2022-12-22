#include "fn_parse.h"
#include <arpa/inet.h>
#pragma GCC diagnostic ignored "-Wclass-memaccess"
namespace FNLog
{
	ReseveKey ParseReserve(const char* begin, const char* end)
	{
		if (end - begin < 2)
		{
			return RK_NULL;
		}
		switch (*begin)
		{
			case 'c':
				if (*(begin + 1) == 'h')
				{
					return RK_CHANNEL;
				}
				else if (*(begin + 1) == 'a')
				{
					if (end - begin > (int)sizeof("category_e") - 1)
					{
						return *(begin + 9) == 'e' ? RK_CATEGORY_EXTEND : RK_CATEGORY_FILTER;
					}
					else
					{
						return RK_CATEGORY;
					}
				}
				break;
			case 'd':
				if (*(begin+1) == 'e') //当单词为de的时候，认为是device key
				{
					return RK_DEVICE;
				}
				else if (*(begin + 1) == 'i')
				{
					return RK_DISABLE;
				}
				break;
			case  'f':
				return RK_FILE;
			case 'h':
				return RK_HOT_UPDATE;
			case 'i':
				if (end - begin > (int)sizeof("identify_e") - 1)
				{
					return *(begin + 9) == 'e' ? RK_IDENTIFY_EXTEND : RK_IDENTIFY_FILTER;
				}
				else
				{
					return RK_IDENTIFY;
				}
				break;
			case 'l':
				if (*(begin + 1) == 'i')
				{
					return RK_LIMIT_SIZE;
				}
				else if (end - begin > 8)
				{
					if (*(begin + 7) == 'n')
					{
						return RK_LOGGER_NAME;
					}
					if (*(begin + 7) == 'd')
					{
						return PK_LOGGER_DESC;
					}
				}
				break;
			case 'p':
				if (*(begin + 1) == 'r')
				{
					return RK_PRIORITY;
				}
				else if (*(begin + 1) == 'a')
				{
					return RK_PATH;
				}
				break;
			case 'r':
				return RK_ROLLBACK;
			case 'o':
				return RK_OUT_TYPE;
			case 's':
				if (*(begin+1) == 'y')
				{
					return RK_SYNC;
				}
				else if (*(begin + 1) == 't')
				{
					return RK_FILE_STUFF_UP;
				}
				return RK_SHM_KEY;
			case 'u':
				return RK_UDP_ADDR;
			default:
				break;
		}
		return RK_NULL;
	}
	LogPriority ParsePriority(const char* begin, const char* end)
	{
		if (end <= begin)
		{
			return PRIORITY_TRACE;
		}
		switch (*begin)
		{
			case 't':case 'T':
			case 'n':case 'N':
				return PRIORITY_TRACE;
			case 'd':case 'D':
				return PRIORITY_DEBUG;
			case 'i':case 'I':
				return PRIORITY_INFO;
			case 'w':case 'W':
				return PRIORITY_WARN;
			case 'e':case 'E':
				return PRIORITY_ERROR;
			case 'a':case 'A':
				return PRIORITY_ALARM;
			case 'f':case 'F':
				return PRIORITY_FATAL;
		}
		return PRIORITY_TRACE;
	}
	bool ParseBool(const char* begin, const char* end)
	{
		if (end <= begin)
		{
			return false;
		}
		if (*begin == '0' || *begin == 'f')
		{
			return false;
		}
		return true;
	}
	long long ParseNumber(const char* begin, const char* end)
	{
		if (end <= begin)
		{
			return 0;
		}
		if (end - begin > 40 )
		{
			return 0;
		}
		char buff[50];
		memcpy(buff, begin, end - begin);
		buff[end - begin] = '\0';
		return strtoll(buff, NULL, 0);
	}
	bool ParseString(const char* begin, const char* end, char * buffer, int buffer_len, int& write_len)
	{
		write_len = 0;
		if (end <= begin)
		{
			return false;
		}
		write_len = buffer_len - 1;
		if (end - begin < write_len)
		{
			write_len = (int)(end - begin);
		}
		memcpy(buffer, begin, write_len);
		buffer[write_len] = '\0';
		return true;
	}
	ChannelType ParseChannelType(const char* begin, const char* end)
	{
		if (end <= begin || *begin != 's')
		{
			return CHANNEL_ASYNC;
		}
		return CHANNEL_SYNC;
	}
	DeviceOutType ParseOutType(const char* begin, const char* end)
	{
		if (end <= begin)
		{
			return DEVICE_OUT_NULL;
		}
		switch (*begin)
		{
			case 'f': case 'F':
				return DEVICE_OUT_FILE;
			case 'n': case 'N':
				return DEVICE_OUT_NULL;
			case 'u': case 'U':
				return DEVICE_OUT_UDP;
			case 's':case 'S':
				return DEVICE_OUT_SCREEN;
			case 'v':case 'V':
				return DEVICE_OUT_VIRTUAL;
		}
		return DEVICE_OUT_NULL;
	}
	std::pair<long long, const char*> ParseAddresIP(const char* begin, const char* end)
	{
		if (end <= begin)
		{
			return std::make_pair(0, end);
		}
		const char* ip_begin = begin;
		while ((*ip_begin < '1' || *ip_begin > '9') && ip_begin != end)
		{
			ip_begin++;
		}
		const char* ip_end = ip_begin;
		while (((*ip_end >= '0' && *ip_end <= '9') || *ip_end == '.') && ip_end != end)
		{
			ip_end++;
		}
		if (ip_end - ip_begin > 40)
		{
			return std::make_pair(0, end);
		}
		char buff[50];
		memcpy(buff, ip_begin, ip_end - ip_begin);
		buff[ip_end - ip_begin] = '\0';
		return std::make_pair((long long)inet_addr(buff), ip_end); 
	}
	void ParseAddres(const char* begin, const char* end, long long & ip, long long& port)
	{
		ip = 0;
		port = 0;
		if (end <= begin)
		{
			return;
		}
		auto result_ip = ParseAddresIP(begin, end); //返回pair(longlong, char *)
		const char* port_begin = result_ip.second;
		while (port_begin != end && (*port_begin < '1' || *port_begin > '9')  )
		{
			port_begin++;
		}
		if (port_begin >= end)
		{
			return;
		}
		if (end - port_begin >= 40)
		{
			return;
		}
		char buff[50];
		memcpy(buff, port_begin, end - port_begin);
		buff[end - port_begin] = '\0';
		port = htons(atoi(buff)); //字符转数字，并转为网络序
		ip = result_ip.first;
		return;
	}
	unsigned long long ParseBitArray(const char* begin, const char* end)
	{
		unsigned long long bitmap = 0;
		if (end <= begin)
		{
			return bitmap;
		}
		const char* offset = begin;
		while (offset < end)
		{
			if (*offset >= '0' && *offset <= '9')
			{
				int bit_offset = atoi(offset);
				bitmap |= (1ULL << bit_offset);
				while (offset < end && (*offset >= '0' && *offset <= '9'))
				{
					offset++;
				}
				continue;
			}
			offset++;
		}
		return bitmap;
	}
	void InitState(LexState& state)
	{
		//static_assert(std::is_trivial<LexState>::value, "");
		memset(&state, 0, sizeof(state));
	}
    int Lex_Line(LexState& ls) //词法分析1行, 定位key,val, 返回正常或错误
	{
		memset(&ls.line_, 0, sizeof(ls.line_));
		while (true)
		{
			char ch = *ls.current_++;
			if (ls.line_.block_type_ == BLOCK_CLEAN && ch != '\0' && ch != '\r' && ch != '\n')
			{
				continue; //当遇到#时，会进入BLOCK_CLEAN 模式,直到遇到\r\n或0
			}
			//preprocess, 例如当是 ':‘ 时
			if (ls.line_.block_type_ == BLOCK_KEY && (ch < 'a' || ch > 'z') && ch != '_')
			{
				ls.line_.block_type_ = BLOCK_PRE_SEP; //遇到非字符键，就是分割字符.
				ls.line_.key_end_ = ls.current_ - 1;  //设定key_end 值，并分析是哪种关键字
				ls.line_.key_ = ParseReserve(ls.line_.key_begin_, ls.line_.key_end_);
				if (ls.line_.key_ == RK_NULL)
				{
					return PEC_ILLEGAL_KEY;
				}
			}
			if (ls.line_.block_type_ == BLOCK_VAL)
			{
				switch (ch)
				{ // line_存的是val 值
					case '\0': case'\n':case '\r': case '#': case '\"':
						ls.line_.block_type_ = BLOCK_CLEAN;
						ls.line_.val_end_ = ls.current_ - 1; //保留val_end
						break;
				}
			}
			//end of line check
			switch (ch)
			{
				case '\0': case '\n':case '\r': case '#':
					if (ls.line_.block_type_ == BLOCK_NULL)
					{
						ls.line_.block_type_ = BLOCK_CLEAN;
						ls.line_.line_type_ = LINE_BLANK; //当遇到0，\r\n或#字符，表示是个空行
					}
					if (ls.line_.block_type_ != BLOCK_CLEAN)
					{
						return PEC_NOT_CLOSURE;
					}
					break;
			}
			//process
			switch (ch)
			{
				case ' ': case '\f': case '\t': case '\v': case '\"':
					if (ls.line_.block_type_ == BLOCK_NULL)
					{
						ls.line_.blank_++;
						break;
					}
					break;
				case '\n':case '\r':
					ls.line_number_++;
					if ((*ls.current_ == '\r' || *ls.current_ == '\n') && *ls.current_ != ch)
					{
						ls.current_++; //skip '\n\r' or '\r\n'
					}
					return PEC_NONE;
				case '\0':
					if (ls.line_.line_type_ != LINE_BLANK)
					{
						ls.current_--;
						return PEC_NONE;
					}
					else
					{
						ls.line_.line_type_ = LINE_EOF; //当遇到0字终结符时(并且还是LINE_BLANK)，置文件尾标志
						return PEC_NONE;
					}

				case '-':
					if (ls.line_.block_type_ == BLOCK_NULL)
					{
						ls.line_.block_type_ = BLOCK_PRE_KEY; // - 是key 的前导
						ls.line_.line_type_ = LINE_ARRAY;
						break;
					}
					else if (ls.line_.block_type_ != BLOCK_VAL)
					{
						return PEC_ILLEGAL_CHARACTER;
					}
					break;
				case ':':
					if (ls.line_.block_type_ == BLOCK_PRE_SEP)
					{
						ls.line_.block_type_ = BLOCK_PRE_VAL;	// : 是val 的前导
						break;
					}
					else if (ls.line_.block_type_ != BLOCK_VAL)
					{
						return PEC_ILLEGAL_CHARACTER;
					}
					break;
				default:
					if ((ch >= 'a' && ch <= 'z')
							|| (ch >= 'A' && ch <= 'Z')
							|| (ch >= '0' && ch <= '9')
							|| ch == '_' || ch == '-' || ch == ':' || ch == '/' || ch == '.' || ch == ',' || ch == '$' || ch == '~')
					{
						switch (ls.line_.block_type_)
						{
							case BLOCK_CLEAN: case BLOCK_KEY: case BLOCK_VAL:
								break;
							case BLOCK_NULL: case BLOCK_PRE_KEY:
								ls.line_.block_type_ = BLOCK_KEY;
								ls.line_.key_begin_ = ls.current_ - 1;	//设定key_begin_ 值
								break;
							case BLOCK_PRE_VAL:
								ls.line_.block_type_ = BLOCK_VAL;
								ls.line_.val_begin_ = ls.current_ - 1; //设定val_begin_ 值
								break;
							default:
								return PEC_ILLEGAL_CHARACTER;
						}
						break;
					}
					else if (ls.line_.block_type_ != BLOCK_CLEAN)
					{
						return PEC_ILLEGAL_CHARACTER;
					}
			}
		}
		return PEC_ERROR;
	}
	int ParseDevice(LexState& ls, Device& device, int indent)
	{
		do
		{
			const char* current = ls.current_;
            int ret = Lex_Line(ls);
			if (ret != PEC_NONE)
			{
				ls.current_ = current;
				ls.line_number_--;
				return ret;
			}
			if (ls.line_.line_type_ == LINE_EOF)
			{
				return ret;
			}
			if (ls.line_.line_type_ == LINE_BLANK)
			{
				continue;
			}
			if (ls.line_.blank_ <= indent) //要求device 的空格缩进必需大于indent,否则视为空行或新设备项
			{
				ls.current_ = current;
				ls.line_number_--;
				ls.line_.line_type_ = LINE_BLANK;
				return 0;
			}
			switch (ls.line_.key_) //根据key 类型分别处理
			{
				case RK_OUT_TYPE:
					device.out_type_ = ParseOutType(ls.line_.val_begin_, ls.line_.val_end_); //返回enum 类型
					if (device.out_type_ == DEVICE_OUT_NULL)
					{
						return PEC_UNDEFINED_DEVICE_TYPE;
					}
					break;
				case RK_DISABLE:
					device.config_fields_[DEVICE_CFG_ABLE] = !ParseBool(ls.line_.val_begin_, ls.line_.val_end_); //"disable" 为false,则ENABLE 为真
					break;
				case RK_PRIORITY:
					device.config_fields_[DEVICE_CFG_PRIORITY] = ParsePriority(ls.line_.val_begin_, ls.line_.val_end_); //字符转enum值
					break;
				case RK_CATEGORY:
					device.config_fields_[DEVICE_CFG_CATEGORY] = atoll(ls.line_.val_begin_);
					break;
				case RK_CATEGORY_EXTEND:
					device.config_fields_[DEVICE_CFG_CATEGORY_EXTEND] = atoll(ls.line_.val_begin_);
					break;
				case RK_CATEGORY_FILTER:
					device.config_fields_[DEVICE_CFG_CATEGORY_FILTER] = ParseBitArray(ls.line_.val_begin_, ls.line_.val_end_);
					break;
				case RK_IDENTIFY:
					device.config_fields_[DEVICE_CFG_IDENTIFY] = atoll(ls.line_.val_begin_);
					break;
				case RK_IDENTIFY_EXTEND:
					device.config_fields_[DEVICE_CFG_IDENTIFY_EXTEND] = atoll(ls.line_.val_begin_);
					break;
				case RK_IDENTIFY_FILTER:
					device.config_fields_[DEVICE_CFG_IDENTIFY_FILTER] = ParseBitArray(ls.line_.val_begin_, ls.line_.val_end_);
					break;
				case RK_LIMIT_SIZE:
					device.config_fields_[DEVICE_CFG_FILE_LIMIT_SIZE] = atoll(ls.line_.val_begin_) * 1000*1000;		//存入device的配置域，供以后使用！
					break;
				case RK_ROLLBACK:
					device.config_fields_[DEVICE_CFG_FILE_ROLLBACK] = atoll(ls.line_.val_begin_);
					break;
				case RK_FILE_STUFF_UP:
					device.config_fields_[DEVICE_CFG_FILE_STUFF_UP] = ParseBool(ls.line_.val_begin_, ls.line_.val_end_);  
					break;
				case RK_PATH:
					if (ls.line_.val_end_ - ls.line_.val_begin_ < Device::MAX_PATH_LEN - 1
							&& ls.line_.val_end_ - ls.line_.val_begin_ >= 1)
					{
						memcpy(device.out_path_, ls.line_.val_begin_, ls.line_.val_end_ - ls.line_.val_begin_);
						device.out_path_[ls.line_.val_end_ - ls.line_.val_begin_] = '\0';
					}
					break;
				case RK_FILE:
					if (ls.line_.val_end_ - ls.line_.val_begin_ < Device::MAX_LOGGER_NAME_LEN - 1
							&& ls.line_.val_end_ - ls.line_.val_begin_ >= 1)
					{
						memcpy(device.out_file_, ls.line_.val_begin_, ls.line_.val_end_ - ls.line_.val_begin_);
						device.out_file_[ls.line_.val_end_ - ls.line_.val_begin_] = '\0';		//copy 到device.out_file_
					}
					break;
				case RK_UDP_ADDR:
					if (true)
					{
						long long ip = 0;
						long long port = 0;
						ParseAddres(ls.line_.val_begin_, ls.line_.val_end_, ip, port);
						device.config_fields_[DEVICE_CFG_UDP_IP] = ip;
						device.config_fields_[DEVICE_CFG_UDP_PORT] = port;
					}

					if (device.config_fields_[DEVICE_CFG_UDP_IP] == 0)
					{
						return PEC_ILLEGAL_ADDR_IP;
					}
					if (device.config_fields_[DEVICE_CFG_UDP_PORT] == 0)
					{
						return PEC_ILLEGAL_ADDR_PORT;
					}
					break;
				default:
					return PEC_UNDEFINED_DEVICE_KEY;
			}
		} while (ls.line_.line_type_ != LINE_EOF);
		return 0;
	}
	int ParseChannel(LexState& ls, Channel& channel, int indent)
	{
		do
		{
			const char* current = ls.current_;
            int ret = Lex_Line(ls);
			if (ret != PEC_NONE)
			{
				ls.current_ = current;
				ls.line_number_--;
				return ret;
			}
			if (ls.line_.line_type_ == LINE_EOF)
			{
				return ret;
			}
			if (ls.line_.line_type_ == LINE_BLANK)
			{
				continue;
			}
			if (ls.line_.blank_ <= indent) //要求channel 的空格缩进必需大于indent,否则视为空行或新的channel
			{
				ls.current_ = current;
				ls.line_number_--;
				ls.line_.line_type_ = LINE_BLANK;
				return 0;
			}
			switch (ls.line_.key_)
			{ //以下为语意分析. 得到同步类型,优先级,ID 等
				case RK_SYNC:
					channel.channel_type_ = ParseChannelType(ls.line_.val_begin_, ls.line_.val_end_);
					break;
				case RK_PRIORITY:
					channel.config_fields_[CHANNEL_CFG_PRIORITY] = ParsePriority(ls.line_.val_begin_, ls.line_.val_end_);
					break;
				case RK_CATEGORY:
					channel.config_fields_[CHANNEL_CFG_CATEGORY] = atoi(ls.line_.val_begin_);
					break;            
				case RK_CATEGORY_EXTEND:
					channel.config_fields_[CHANNEL_CFG_CATEGORY_EXTEND] = atoi(ls.line_.val_begin_);
					break;
				case RK_CATEGORY_FILTER:
					channel.config_fields_[CHANNEL_CFG_CATEGORY_FILTER] = ParseBitArray(ls.line_.val_begin_, ls.line_.val_end_);
					break;
				case RK_IDENTIFY:
					channel.config_fields_[CHANNEL_CFG_IDENTIFY] = atoi(ls.line_.val_begin_);
					break;
				case RK_IDENTIFY_EXTEND:
					channel.config_fields_[CHANNEL_CFG_IDENTIFY_EXTEND] = atoi(ls.line_.val_begin_);
					break;
				case RK_IDENTIFY_FILTER:
					channel.config_fields_[CHANNEL_CFG_IDENTIFY_FILTER] = ParseBitArray(ls.line_.val_begin_, ls.line_.val_end_);
					break;
				case RK_DEVICE: //channel 包含DEVICE
					if (ls.line_.line_type_ != LINE_ARRAY)
					{
						return PEC_DEVICE_NOT_ARRAY;
					}
					else
					{
						int device_id = atoi(ls.line_.val_begin_); //获得device_id
						if (channel.device_size_ >= Channel::MAX_DEVICE_SIZE)
						{
							return PEC_DEVICE_INDEX_OUT_MAX;
						}
						if (device_id != channel.device_size_)
						{
							return PEC_DEVICE_INDEX_NOT_SEQUENCE;
						}
						Device& device = channel.devices_[channel.device_size_++];
						memset(&device, 0, sizeof(device));
						device.device_id_ = device_id;
						ret = ParseDevice(ls, device, ls.line_.blank_); //分析device, 传入blank值,后面参数缩进只会大于blank
						if (device.out_type_ == DEVICE_OUT_VIRTUAL)
						{
							channel.virtual_device_id_ = device.device_id_;
						}
						if (ret != PEC_NONE || ls.line_.line_type_ == LINE_EOF)
						{
							return ret;
						}
					}
					break;
				default:
					return PEC_UNDEFINED_CHANNEL_KEY;
			}
		} while (ls.line_.line_type_ != LINE_EOF);
		return 0;
	}
	int ParseLogger(LexState& ls, const std::string& text)
	{
		//UTF8 BOM 
		const char* first = &text[0];
		if (text.size() >= 3)
		{
			if ((unsigned char)text[0] == 0xEF && (unsigned char)text[1] == 0xBB && (unsigned char)text[2] == 0xBF)
			{
				first += 3;
			}
		}
		ls.first_ = first;
		ls.end_ = first + text.length();
		memset(&ls.channels_, 0, sizeof(ls.channels_));
		ls.channel_size_ = 0;
		ls.hot_update_ = false;
		ls.current_ = ls.first_;
		ls.line_.line_type_ = LINE_NULL;
		ls.line_number_ = 1;
		ls.desc_len_ = 0;
		ls.name_len_ = 0;
		do
		{
			const char* current = ls.current_;
            int ret = Lex_Line(ls); //分析一行, 取到key,val指针位置或行状态
			if (ret != PEC_NONE)
			{
				ls.current_ = current;
				ls.line_number_--;
				return ret;
			}
			if (ls.line_.line_type_ == LINE_EOF) //这一行的类型为结束，退出
			{
				break;
			}
			if (ls.line_.line_type_ == LINE_BLANK)//这一行类型为空行，继续
			{
				continue;
			}
			switch (ls.line_.key_)
			{
				case RK_HOT_UPDATE:
					ls.hot_update_ = ParseBool(ls.line_.val_begin_, ls.line_.val_end_);//"disable"
					break;
				case RK_LOGGER_NAME:
					ParseString(ls.line_.val_begin_, ls.line_.val_end_, ls.name_, Logger::MAX_LOGGER_NAME_LEN, ls.name_len_);
					break;
				case PK_LOGGER_DESC:
					ParseString(ls.line_.val_begin_, ls.line_.val_end_, ls.desc_, Logger::MAX_LOGGER_DESC_LEN, ls.desc_len_);
					break;
				case RK_SHM_KEY:
					ls.shm_key_ = ParseNumber(ls.line_.val_begin_, ls.line_.val_end_);
					break;
				case RK_CHANNEL:	// logger 包含channel
					if (ls.line_.line_type_ != LINE_ARRAY)
					{
						return PEC_CHANNEL_NOT_ARRAY;
					}
					else
					{
						int channel_id = atoi(ls.line_.val_begin_);
						if (ls.channel_size_ >= Logger::MAX_CHANNEL_SIZE)
						{
							return PEC_CHANNEL_INDEX_OUT_MAX;
						}
						if (ls.channel_size_ != channel_id)
						{
							return PEC_CHANNEL_INDEX_NOT_SEQUENCE;
						}
						Channel& channel = ls.channels_[ls.channel_size_++];
						memset(&channel, 0, sizeof(channel));
						channel.channel_id_ = channel_id;
						ret = ParseChannel(ls, channel, ls.line_.blank_);
						if (ret != 0)
						{
							return ret;
						}
					}
					break;
				default:
					return PEC_UNDEFINED_GLOBAL_KEY;
			}
		} while (ls.line_.line_type_ != LINE_EOF);
		if (ls.channel_size_ == 0)
		{
			return PEC_NO_ANY_CHANNEL;
		}
		return PEC_NONE;
	}
}
