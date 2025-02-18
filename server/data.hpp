#ifndef __MY_DATA__
#define __MY_DATA__
#include <unordered_map>
#include "util.hpp"
#include "config.hpp"
#include "pthread.h"
namespace Cloud
{
    typedef struct BackUpInfo
    {
        bool pack_flag;
        time_t atime;
        time_t mtime;
        size_t fsize;
        std::string real_path;
        std::string url;
        std::string pack_path;
        bool NewBackUpInfo(const std::string &realpath)
        {
            Cloud::FileUtil fu(realpath);
            if (fu.Exists() == false)
            {
                std::cout << "new backupinfo: file not exists!\n";
                return false;
            }
            this->pack_flag = false;
            this->atime = fu.LastATime();
            this->mtime = fu.LastMTime();
            this->fsize = fu.FileSize();
            this->real_path = realpath;

            Config *config = Config::GetInstance();
            std::string download_prefix = config->GetUrlPrefix();
            std::string filename = fu.FileName();
            std::string arc_suffix = config->GetArcSuffix();
            std::string packdir = config->GetPackDir();

            // backdir/a.txt -> /download/a.txt
            this->url = download_prefix + filename;
            // backdir/a.txt -> /packdir/a.txt.lz
            this->pack_path = packdir + filename + arc_suffix;
            return true;
        }
    } BackUpInfo;

    class DataManager
    {
    private:
        std::string _manager_file; // 持久化存储文件
        pthread_rwlock_t _rwlock;
        std::unordered_map<std::string, BackUpInfo> _table; // string - BackUpInfo
    public:
        DataManager()
        {
            _manager_file = Config::GetInstance()->GetManagerFile(); // 备份信息存放文件
            pthread_rwlock_init(&_rwlock, nullptr);
            InitLoad();
        }

        ~DataManager()
        {
            pthread_rwlock_destroy(&_rwlock);
        }

        bool Insert(const BackUpInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            std::string url = info.url;
            _table[url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }

        bool Update(const BackUpInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            std::string url = info.url;
            _table[url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }
        // /download/bundle.h
        bool GetOneByURL(const std::string &url, BackUpInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            // 因为url是key值，所以直接通过find进行查找
            std::unordered_map<std::string, BackUpInfo>::iterator it = _table.find(url);
            if (it == _table.end())
            {
                pthread_rwlock_unlock(&_rwlock);
                return false;
            }

            *info = it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }

        bool GetOneByRealPath(const std::string &realpath, BackUpInfo *info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.begin();
            for (; it != _table.end(); ++it)
            {
                if (it->second.real_path == realpath)
                {
                    *info = it->second;
                    pthread_rwlock_unlock(&_rwlock);
                    return true;
                }
            }
            pthread_rwlock_unlock(&_rwlock);
            return false;
        }
        bool GetAll(std::vector<BackUpInfo> *array)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.begin();
            for (; it != _table.end(); ++it)
            {
                array->push_back(it->second);
            }
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }

        bool Storage()
        {
            // 1.读取所有数据
            std::vector<BackUpInfo> array;
            this->GetAll(&array);
            // 2.添加到json::value中
            Json::Value root;
            for (int i = 0; i < array.size(); ++i)
            {
                Json::Value item;
                item["pack_flag"] = array[i].pack_flag;
                item["fsize"] = (Json::Int64)array[i].fsize;
                item["atime"] = (Json::Int64)array[i].atime;
                item["mtime"] = (Json::Int64)array[i].mtime;
                item["pack_path"] = array[i].pack_path;
                item["real_path"] = array[i].real_path;
                item["url"] = array[i].url;
                root.append(item);
            }
            // 3.对json::value进行序列化
            std::string body;
            JsonUtil::Serialize(root, &body);
            // 4.写文件
            FileUtil fu(_manager_file);
            fu.SetContent(body);
            return true;
        }

        bool InitLoad()
        {
            // 1.将数据文件中的数据读取出来
            FileUtil fu(_manager_file);
            if (fu.Exists() == false)
            {
                return false;
            }
            std::string body;
            fu.GetContent(&body);

            // 2.反序列化
            Json::Value root;
            JsonUtil::UnSerialize(body, &root);

            // 3.将反序列化得到的Json::Value中的数据添加到table中
            for (int i = 0; i < root.size(); ++i)
            {
                BackUpInfo info;
                info.pack_flag = root[i]["pack_flag"].asBool();
                info.fsize = root[i]["fsize"].asInt64();
                info.atime = root[i]["atime"].asInt64();
                info.mtime = root[i]["mtime"].asInt64();
                info.pack_path = root[i]["pack_path"].asString();
                info.real_path = root[i]["real_path"].asString();
                info.url = root[i]["url"].asString();
                Insert(info);
            }
            return true;
        }
    };
};
#endif