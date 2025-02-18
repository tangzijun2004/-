#ifndef __MY_CONFIG__
#define __MY_CONFIG__
#include <mutex>
#include "util.hpp"

namespace Cloud
{
#define CONFIG_FILE "./cloud.conf"
	class Config
	{
	private:
		Config()
		{
			ReadConfigFile();
		}
		static Config *_instance;
		static std::mutex _mutex;

	private:
		int _hot_time;			   // 开发原始文件
		int _server_port;		   // port
		std::string _server_ip;	   // ip
		std::string _url_prefix;   // 下载URL前缀路径
		std::string _arc_suffix;   // 压缩包后缀名称
		std::string _packdir;	   // 上传文件存放路径
		std::string _backdir;	   // 压缩文件存放路径
		std::string _manager_file; // 备份信息存放文件

		bool ReadConfigFile()
		{
			FileUtil fu(CONFIG_FILE);
			std::string body;
			if (fu.GetContent(&body) == false)
			{
				std::cout << "load config file failed!\n";
				return false;
			}

			Json::Value root;
			if (JsonUtil::UnSerialize(body, &root) == false)
			{
				std::cout << "parse config file failed!\n";
				return false;
			}
			_hot_time = root["hot_time"].asInt();
			_server_port = root["server_port"].asInt();
			_server_ip = root["server_ip"].asString();
			_url_prefix = root["url_prefix"].asString();
			_arc_suffix = root["arc_suffix"].asString();
			_packdir = root["packdir"].asString();
			_backdir = root["backdir"].asString();
			_manager_file = root["manager_file"].asString();
			return true;
		}

	public:
		static Config *GetInstance()
		{
			if (_instance == NULL)
			{
				_mutex.lock();
				if (_instance == NULL)
				{
					_instance = new Config();
				}
				_mutex.unlock();
			}
			return _instance;
		}
		int GetHotTime()
		{
			return _hot_time;
		}
		int GetServerPort()
		{
			return _server_port;
		}
		std::string GetServerIp()
		{
			return _server_ip;
		}
		std::string GetUrlPrefix()
		{
			return _url_prefix;
		}
		std::string GetArcSuffix()
		{
			return _arc_suffix;
		}
		std::string GetPackDir()
		{
			return _packdir;
		}
		std::string GetBackDir()
		{
			return _backdir;
		}
		std::string GetManagerFile()
		{
			return _manager_file;
		}
	};
	Config *Config::_instance = NULL;
	std::mutex Config::_mutex;
};
#endif
