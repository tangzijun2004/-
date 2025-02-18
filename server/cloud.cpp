#include "util.hpp"
#include "config.hpp"
#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"
#include <thread>
void FileUtilTest(const std::string filename)
{
    // Cloud::FileUtil fu(filename);
    // std::cout << fu.FileSize() << std::endl;
    // std::cout << fu.LastATime() << std::endl;
    // std::cout << fu.LastMTime() << std::endl;
    // std::cout << fu.FileName() << std::endl;

    // std::string body;
    // fu.GetContent(&body);
    // std::cout << body << std::endl;
    // fu.GetPosLen(&body, 4, 8);
    // std::cout << body << std::endl;

    // Cloud::FileUtil fu2("./h.txt");
    // fu2.SetContent(body);

    // std::string packname = filename + ".lz";
    // Cloud::FileUtil fu(filename);
    // fu.Compress(packname);
    // Cloud::FileUtil pfu(packname);
    // pfu.UnCompress("./hello.txt");

    Cloud::FileUtil fu(filename);
    fu.CreateDirectory();
    std::vector<std::string> arry;
    fu.ScanDirectory(&arry);
    for (auto &a : arry)
    {
        std::cout << a << std::endl;
    }
}

void JosnUtilTest()
{
    const char *name = "小明";
    int age = 18;
    float score[] = {85, 88.5, 99};
    Json::Value root;
    root["姓名"] = name;
    root["年龄"] = age;
    root["成绩"].append(score[0]);
    root["成绩"].append(score[1]);
    root["成绩"].append(score[2]);

    std::string json_str;
    Cloud::JsonUtil::Serialize(root, &json_str);
    std::cout << json_str << std::endl;

    Json::Value val;
    Cloud::JsonUtil::UnSerialize(json_str, &val);
    std::cout << val["姓名"].asString() << std::endl;
    std::cout << val["年龄"].asInt() << std::endl;
    for (int i = 0; i < val["成绩"].size(); ++i)
    {
        std::cout << val["成绩"][i].asFloat() << std::endl;
    }
}

void ConfigTest()
{
    Cloud::Config *confg = Cloud::Config::GetInstance();
    std::cout << confg->GetHotTime() << std::endl;
    std::cout << confg->GetServerPort() << std::endl;
    std::cout << confg->GetServerIp() << std::endl;
    std::cout << confg->GetUrlPrefix() << std::endl;
    std::cout << confg->GetArcSuffix() << std::endl;
    std::cout << confg->GetPackDir() << std::endl;
    std::cout << confg->GetBackDir() << std::endl;
    std::cout << confg->GetManagerFile() << std::endl;
}

void DataTest(const std::string &filename)
{

    Cloud::DataManager data;
    std::vector<Cloud::BackUpInfo> arry;
    data.GetAll(&arry);
    std::cout << arry.size() << std::endl;
    for (auto &a : arry)
    {
        std::cout << a.pack_flag << std::endl;
        std::cout << a.fsize << std::endl;
        std::cout << a.mtime << std::endl;
        std::cout << a.atime << std::endl;
        std::cout << a.real_path << std::endl;
        std::cout << a.pack_path << std::endl;
        std::cout << a.url << std::endl;
    }

    // Cloud::BackUpInfo info;
    // info.NewBackUpInfo(filename);
    // std::cout << info.pack_flag << std::endl;
    // std::cout << info.atime << std::endl;
    // std::cout << info.mtime << std::endl;
    // std::cout << info.fsize << std::endl;
    // std::cout << info.real_path << std::endl;
    // std::cout << info.pack_path << std::endl;
    // std::cout << info.url << std::endl;

    // // Cloud::BackUpInfo info;
    // // info.NewBackUpInfo(filename);
    // Cloud::DataManager data;
    // std::cout << "-----------insert and GetOneByURL--------\n";
    // Cloud::BackUpInfo tmp;
    // data.Insert(info);
    // data.GetOneByURL("/download/bundle.h", &tmp);
    // std::cout << tmp.pack_flag << std::endl;
    // std::cout << tmp.atime << std::endl;
    // std::cout << tmp.mtime << std::endl;
    // std::cout << tmp.fsize << std::endl;
    // std::cout << tmp.real_path << std::endl;
    // std::cout << tmp.pack_path << std::endl;
    // std::cout << tmp.url << std::endl;

    // std::cout << "-----------update and getall--------\n";
    // info.pack_flag = true;
    // data.Update(info);
    // std::vector<Cloud::BackUpInfo> array;
    // data.GetAll(&array);
    // for (auto x : array)
    // {
    //     std::cout << x.pack_flag << std::endl;
    //     std::cout << x.atime << std::endl;
    //     std::cout << x.mtime << std::endl;
    //     std::cout << x.fsize << std::endl;
    //     std::cout << x.real_path << std::endl;
    //     std::cout << x.pack_path << std::endl;
    //     std::cout << x.url << std::endl;
    // }

    // std::cout << "-----------GetOneByRealPath--------\n";
    // Cloud::BackUpInfo tmp2;
    // data.GetOneByRealPath(filename, &tmp2);
    // std::cout << tmp2.pack_flag << std::endl;
    // std::cout << tmp2.atime << std::endl;
    // std::cout << tmp2.mtime << std::endl;
    // std::cout << tmp2.fsize << std::endl;
    // std::cout << tmp2.real_path << std::endl;
    // std::cout << tmp2.pack_path << std::endl;
    // std::cout << tmp2.url << std::endl;
}

Cloud::DataManager *_data;
void HotTest()
{
    Cloud::HotManager hot;
    hot.RunModule();
}

void ServiceTest()
{
    Cloud::Service sv;
    sv.RunModule();
}
int main(int argc, char *argv[])
{
    _data = new Cloud::DataManager();
    // FileUtilTest(argv[1]);
    // JosnUtilTest();
    // ConfigTest();
    // DataTest(argv[1]);
    // HotTest();
    // ServiceTest();
    std::thread thread_hot_manager(HotTest);
    std::thread thread_service_manager(ServiceTest);
    thread_hot_manager.join();
    thread_service_manager.join();
    return 0;
}