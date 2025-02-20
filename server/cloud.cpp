#include "util.hpp"
#include "config.hpp"
#include "data.hpp"
#include "hot.hpp"
#include "service.hpp"
#include <thread>

void FileUtilTest(const std::string &filename)
{
	/*
		cloud::FileUtil fu(filename);
		std::cout << fu.FileSize() << std::endl;
		std::cout << fu.LastMTime() << std::endl;
		std::cout << fu.LastATime() << std::endl;
		std::cout << fu.FileName() << std::endl;
		cloud::FileUtil fu(filename);
		std::string body;
		fu.GetContent(&body);

		cloud::FileUtil nfu("./hello.txt");
		nfu.SetContent(body);
		std::string packname = filename + ".lz";
		cloud::FileUtil fu(filename);
		fu.Compress(packname);
		cloud::FileUtil pfu(packname);
		pfu.UnCompress("./hello.txt");
	*/
	cloud::FileUtil fu(filename);
	fu.CreateDirectory();
	std::vector<std::string> arry;
	fu.ScanDirectory(&arry);
	for (auto &a : arry)
	{
		std::cout << a << std::endl;
	}
	return;
}
void JsonUtilTest()
{
	const char *name = "小明";
	int age = 19;
	float score[] = {85, 88.5, 99};
	Json::Value root;
	root["姓名"] = name;
	root["年龄"] = age;
	root["成绩"].append(score[0]);
	root["成绩"].append(score[1]);
	root["成绩"].append(score[2]);
	std::string json_str;
	cloud::JsonUtil::Serialize(root, &json_str);
	std::cout << json_str << std::endl;

	Json::Value val;
	cloud::JsonUtil::UnSerialize(json_str, &val);
	std::cout << val["姓名"].asString() << std::endl;
	std::cout << val["年龄"].asInt() << std::endl;
	for (int i = 0; i < val["成绩"].size(); i++)
	{
		std::cout << val["成绩"][i].asFloat() << std::endl;
	}
}

void ConfigTest()
{
	cloud::Config *config = cloud::Config::GetInstance();
	std::cout << config->GetHotTime() << std::endl;
	std::cout << config->GetServerPort() << std::endl;
	std::cout << config->GetServerIp() << std::endl;
	std::cout << config->GetDownloadPrefix() << std::endl;
	std::cout << config->GetPackFileSuffix() << std::endl;
	std::cout << config->GetPackDir() << std::endl;
	std::cout << config->GetBackDir() << std::endl;
	std::cout << config->GetBackupFile() << std::endl;
}

void DataTest(const std::string &filename)
{
	cloud::DataManager data;
	std::vector<cloud::BackupInfo> arry;
	data.GetAll(&arry);
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

	// cloud::BackupInfo info;
	// info.NewBackupInfo(filename);
	// cloud::DataManager data;
	// std::cout << "-----------insert and GetOneByURL--------\n";
	// data.Insert(info);
	// cloud::BackupInfo tmp;
	// data.GetOneByURL("/download/bundle.h", &tmp);
	// std::cout << tmp.pack_flag << std::endl;
	// std::cout << tmp.fsize << std::endl;
	// std::cout << tmp.mtime << std::endl;
	// std::cout << tmp.atime << std::endl;
	// std::cout << tmp.real_path << std::endl;
	// std::cout << tmp.pack_path << std::endl;
	// std::cout << tmp.url << std::endl;

	// std::cout << "-----------update and getall--------\n";
	// info.pack_flag = true;
	// data.Update(info);
	// std::vector<cloud::BackupInfo> arry;
	// data.GetAll(&arry);
	// for (auto &a : arry)
	// {
	// 	std::cout << a.pack_flag << std::endl;
	// 	std::cout << a.fsize << std::endl;
	// 	std::cout << a.mtime << std::endl;
	// 	std::cout << a.atime << std::endl;
	// 	std::cout << a.real_path << std::endl;
	// 	std::cout << a.pack_path << std::endl;
	// 	std::cout << a.url << std::endl;
	// }
	// std::cout << "-----------GetOneByRealPath--------\n";
	// data.GetOneByRealPath(filename, &tmp);
	// std::cout << tmp.pack_flag << std::endl;
	// std::cout << tmp.fsize << std::endl;
	// std::cout << tmp.mtime << std::endl;
	// std::cout << tmp.atime << std::endl;
	// std::cout << tmp.real_path << std::endl;
	// std::cout << tmp.pack_path << std::endl;
	// std::cout << tmp.url << std::endl;
}
cloud::DataManager *_data;
void HotTest()
{
	cloud::HotManager hot;
	hot.RunModule();
}

void ServiceTest()
{
	cloud::Service srv;
	srv.RunModule();
}
int main(int argc, char *argv[])
{
	_data = new cloud::DataManager();
	// FileUtilTest(argv[1]);
	// JsonUtilTest();
	// ConfigTest();
	// DataTest(argv[1]);
	// HotTest();
	ServiceTest();
	//  std::thread thread_hot_manager(HotTest);
	//  std::thread thread_service(ServiceTest);
	//  thread_hot_manager.join();
	//  thread_service.join();
	return 0;
}
