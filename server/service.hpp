#ifndef __MY_SERVICE__
#define __MY_SERVICE__
#include "data.hpp"
#include "httplib.h"

extern Cloud::DataManager *_data;
namespace Cloud
{
    class Service
    {
    private:
        int _server_port;
        std::string _server_ip;
        std::string _url_prefix;
        httplib::Server _server;

    private:
        static void UpLoad(const httplib::Request &req, httplib::Response &res)
        {
            // post /upload  文件数据在正文中（正文并不全是文件数据）

            // 1.判断文件是否上传成功
            auto ret = req.has_file("file"); // 判断有没有上传的文件
            if (ret == false)
            {
                std::cout << "not file upload\n";
                res.status = 400;
                return;
            }

            // 2.将客户端上传的文件上传到指定的服务器备份目录下
            const auto &file = req.get_file_value("file");
            // file.filename//文件名称    file.content//文件数据
            std::string filename = file.filename; // 要上传的文件名
            std::string back_dir = Config::GetInstance()->GetBackDir();
            std::string realpath = back_dir + FileUtil(filename).FileName(); // 要上传该文件的实际路径
            FileUtil fu(realpath);
            std::string body = file.content;
            fu.SetContent(body); // 将数据写入文件

            // 3.组织备份信息
            BackUpInfo info;
            info.NewBackUpInfo(realpath);
            _data->Insert(info);
            return;
        }
        static std::string TimeToStr(time_t time)
        {
            return ctime(&time);
        }

        static void Beauty(std::stringstream &ss)
        {
            ss << "<html><head>"
               << "<meta charset='UTF-8'>"
               << "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
               << "<title>Download</title>"
               << "<style>"
               << "body {"
               << "    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
               << "    background-color: #f4f4f9;"
               << "    margin: 0;"
               << "    padding: 0;"
               << "    display: flex;"
               << "    justify-content: center;"
               << "    align-items: center;"
               << "    height: 100vh;"
               << "}"
               << ".container {"
               << "    background-color: #fff;"
               << "    padding: 2rem 3rem;"
               << "    border-radius: 12px;"
               << "    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.1);"
               << "    width: 80%;"
               << "    max-width: 800px;"
               << "    overflow-x: auto;"
               << "}"
               << "h1 {"
               << "    font-size: 2rem;"
               << "    text-align: center;"
               << "    margin-bottom: 2rem;"
               << "    color: #333;"
               << "}"
               << "table {"
               << "    width: 100%;"
               << "    border-collapse: collapse;"
               << "    margin-top: 1rem;"
               << "}"
               << "th, td {"
               << "    padding: 1rem;"
               << "    text-align: left;"
               << "    border-bottom: 1px solid #ddd;"
               << "}"
               << "th {"
               << "    background-color: #f7f7f7;"
               << "    font-weight: bold;"
               << "}"
               << "tr:hover {"
               << "    background-color: #f1f1f1;"
               << "}"
               << "a {"
               << "    text-decoration: none;"
               << "    color: #007bff;"
               << "    font-weight: bold;"
               << "}"
               << "a:hover {"
               << "    color: #0056b3;"
               << "}"
               << ".file-size, .file-time {"
               << "    text-align: right;"
               << "    font-size: 1rem;"
               << "    color: #555;"
               << "}"
               << "</style>"
               << "</head><body>"
               << "<div class='container'>"
               << "<h1>Download Files</h1>"
               << "<table>"
               << "<thead>"
               << "<tr><th>Filename</th><th>Last Accessed</th><th>File Size</th></tr>"
               << "</thead><tbody>";
        }

        static void ListShow(const httplib::Request &req, httplib::Response &res)
        {
            // 1. 获取所有的文件备份信息
            std::vector<BackUpInfo> array;
            _data->GetAll(&array);
            // 2. 根据所有备份信息，组织html文件数据

            // version 1
            /*
            std::stringstream ss;
            ss << "<html><head><meta charset='UTF-8'><title>Download</title></head><body><h1>Download</h1><table>";
            for (auto &x : array)
            {
                ss << "<tr><td><a href='";
                std::string filename = FileUtil(x.real_path).FileName();
                ss << x.url << "'>" << filename << "</a></td>";

                std::string time = TimeToStr(x.atime);
                ss << "<td align='right'> " << time << " </td>";

                ss << "<td align='right'> " << x.fsize / 1024 << "k </td></tr>";
            }
            ss << "	</table></body></html> ";
            // 将数据发送给客户端
            res.body = ss.str();
            res.set_header("Content-Type", "text/html; charset=utf-8");
            res.status = 200;
            return;
            */

            // version 2 优化页面
            std::stringstream ss;
            Beauty(ss);
            for (auto &x : array)
            {
                std::string filename = FileUtil(x.real_path).FileName();
                std::string time = TimeToStr(x.atime);
                std::string fileSize = std::to_string(x.fsize / 1024) + "k"; // Convert to KB

                ss << "<tr>"
                   << "<td><a href='" << x.url << "'>" << filename << "</a></td>"
                   << "<td class='file-time'>" << time << "</td>"
                   << "<td class='file-size'>" << fileSize << "</td>"
                   << "</tr>";
            }

            ss << "</tbody></table></div></body></html>";

            // 将数据发送给客户端
            res.body = ss.str();
            res.set_header("Content-Type", "text/html; charset=utf-8");
            res.status = 200;
            return;
        }
        // 这里参数不使用string filename,因为mtime会改变
        static std::string GetETag(const BackUpInfo &info)
        {
            // etag : filename - fsize - mtime
            FileUtil fu(info.real_path);
            std::string etag = fu.FileName();
            etag += "-";
            etag += std::to_string(info.fsize);
            etag += "-";
            etag += std::to_string(info.mtime);
            return etag;
        }
        static void DownLoad(const httplib::Request &req, httplib::Response &res)
        {
            ////1. 获取客户端请求的资源路径path   req.path
            std::string down_path = req.path;
            // 2. 根据资源路径，获取文件备份信息
            BackUpInfo info;
            _data->GetOneByURL(down_path, &info);
            // 3. 判断文件是否被压缩，如果被压缩，要先解压缩,
            if (info.pack_flag == true)
            {
                // /packdir/a.txt.lz
                FileUtil fu(info.pack_path);
                // ./backup/a.txt
                fu.UnCompress(info.real_path);
                // 4. 删除压缩包，修改备份信息（已经没有被压缩）
                fu.Remove();
                info.pack_flag = false;
                _data->Update(info);
            }

            bool retrans = false; // 是否需要重传
            std::string old_etag;
            if (req.has_header("If-Range"))
            {
                old_etag = req.get_header_value("If-Range");
                // 有If-Range字段且，这个字段的值与请求文件的最新etag一致则符合断点续传
                if (old_etag == GetETag(info))
                    retrans = true;
            }

            // 4. 读取文件数据，放入rsp.body中
            FileUtil fu(info.real_path);
            if (retrans == false)
            {
                fu.GetContent(&res.body);
                // 5. 设置响应头部字段： ETag， Accept-Ranges: bytes
                res.set_header("Accept-Ranges", "bytes");
                // 前面解压可能将文件的mtime修改，故这里不传info.real_path，而是传info保持mtime前后解压前与解压后一直
                res.set_header("ETag", GetETag(info));
                res.set_header("Content-Type", "application/octet-stream");
                res.status = 200;
            }
            else
            {
                // httplib内部实现了对于区间请求也就是断点续传请求的处理
                // 只需要我们用户将文件所有数据读取到rsp.body中，它内部会自动根据请求
                // 区间，从body中取出指定区间数据进行响应
                //  std::string  range = req.get_header_val("Range"); bytes=start-end

                fu.GetContent(&res.body);
                res.set_header("Accept-Ranges", "bytes");
                res.set_header("ETag", GetETag(info));
                res.set_header("Content-Type", "application/octet-stream");
                // rsp.set_header("Content-Range", "bytes start-end/fsize");
                res.status = 206; // 区间请求响应的是206*****
            }
            return;
        }

    public:
        Service()
        {
            Config *config = Config::GetInstance();
            _server_ip = config->GetServerIp();
            _server_port = config->GetServerPort();
            _url_prefix = config->GetUrlPrefix();
        }

        bool RunModule()
        {
            _server.Post("/upload", UpLoad);
            _server.Get("/listshow", ListShow);
            _server.Get("/", ListShow);
            std::string download_url = _url_prefix + "(.*)"; // 正则表达式 .表示出了\r,\n的任意一个字符,*表示多个字符
            _server.Get(download_url, DownLoad);
            _server.listen(_server_ip.c_str(), _server_port);
            return true;
        }
    };
};
#endif