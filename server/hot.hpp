#ifndef __MY_HOT__
#define __MY_HOT__
#include "data.hpp"
#include <unistd.h>

extern Cloud::DataManager *_data;

namespace Cloud
{
    class HotManager
    {
    private:
        std::string _back_dir;   // 备份目录
        std::string _pack_dir;   // 压缩目录
        std::string _arc_suffix; // 压缩后缀
        int hot_time;

    private:
        // 非热点文件-返回真；热点文件-返回假
        bool HotJudge(const std::string &filename)
        {
            FileUtil fu(filename);
            time_t last_atime = fu.LastATime();
            time_t current_time = time(nullptr);
            if (current_time - last_atime > hot_time)
                return true;
            return false;
        }

    public:
        HotManager()
        {
            Cloud::Config *config = Config::GetInstance();
            _back_dir = config->GetBackDir();
            _pack_dir = config->GetPackDir();
            _arc_suffix = config->GetArcSuffix();
            hot_time = config->GetHotTime();
            FileUtil tmp1(_back_dir);
            FileUtil tmp2(_pack_dir);
            tmp1.CreateDirectory();
            tmp2.CreateDirectory();
        }
        bool RunModule()
        {
            while (true)
            {
                // 1.遍历备份目录，获取所有文件名
                FileUtil fu(_back_dir);
                std::vector<std::string> array;
                fu.ScanDirectory(&array);
                // 2.判断文件是否为非热点文件
                for (auto &a : array)
                {
                    // 是热点文件 不处理
                    if (HotJudge(a) == false)
                        continue;

                    // 3. 获取文件的备份信息
                    BackUpInfo bi;
                    bool ret = _data->GetOneByRealPath(a, &bi);
                    if (ret == false)
                    {
                        // 文件存在，但是还没将信息写入到数据模块中
                        bi.NewBackUpInfo(a);
                    }

                    // 对非热点文件进行压缩处理
                    FileUtil tmp(a);
                    tmp.Compress(bi.pack_path);
                    // 删除源文件，修改备份信息
                    tmp.Remove();
                    bi.pack_flag = true;
                    _data->Insert(bi);
                }
                usleep(1000);
            }
        }
    };
};
#endif