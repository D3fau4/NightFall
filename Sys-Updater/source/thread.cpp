#include "thread.hpp"

#include <math.h>
#include <fstream>
#include <string>
#include <iostream>
#include "net/net.hpp"

#include <chrono>
using namespace std::literals::chrono_literals;

json V1;

namespace BackGround
{
    BackgroundTasks::BackgroundTasks()
    {
        brls::Logger::debug("thread arrancado (?)");
        this->m_running = true;
        this->m_Download == false;

        this->m_downloadUpdateThread = std::thread(&BackgroundTasks::DownloadUpdate, this);
    }

    BackgroundTasks::~BackgroundTasks()
    {
        this->m_running = false;

        this->m_downloadUpdateThread.join();
    }

    void BackgroundTasks::DownloadUpdate()
    {
        Network::Net net = Network::Net();
        while (this->m_running)
        {
            if (this->m_Download == true)
            {
                std::ifstream i("/switch/Sys-Updater/temp.json");
                i >> V1;
                auto v7 = V1["titleids"].get<std::vector<std::string>>();
                int n = v7.size();
                for (int i = 0; i < n; i++)
                {
                    if (V1["programid"][v7[i]].contains("Program") == true)
                    {
                        std::string download = "http://192.168.1.128/c/c/" + V1["programid"][v7[i]]["Program"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["Program"].get<std::string>() + ".nca";
                        net.Download(download, out);
                        this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                    else if (V1["programid"][v7[i]].contains("Data") == true)
                    {
                        std::string download = "http://192.168.1.128/c/c/" + V1["programid"][v7[i]]["Data"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["Data"].get<std::string>() + ".nca";
                        net.Download(download, out);
                        this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                    else if (V1["programid"][v7[i]].contains("PublicData") == true)
                    {
                        std::string download = "http://192.168.1.128/c/c/" + V1["programid"][v7[i]]["PublicData"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["PublicData"].get<std::string>() + ".nca";
                        net.Download(download, out);
                        this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                    if (V1["programid"][v7[i]].contains("Meta") == true)
                    {
                        std::string download = "http://192.168.1.128/c/a/" + V1["programid"][v7[i]]["Meta"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["Meta"].get<std::string>() + ".cnmt.nca";
                        net.Download(download, out);
                        this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                }
            }
        }
    }

} // namespace BackGround

/*void BackGround::Download(bool download)
{
    Network::Net net = Network::Net();

    while (this->m_running)
    {
        if (download == true)
        {
            auto v7 = V1["titleids"].get<std::vector<std::string>>();
            int n = v7.size();
            for (int i = 0; i < n; i++)
            {
                if (V1["programid"][v7[i]].contains("Program") == true)
                {
                    std::string download = "http://192.168.1.128/c/c/" + V1["programid"][v7[i]]["Program"].get<std::string>();
                    brls::Logger::debug(download);
                    std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["Program"].get<std::string>() + ".nca";
                    net.Download(download, out);
                }
                else if (V1["programid"][v7[i]].contains("Data") == true)
                {
                    std::string download = "http://192.168.1.128/c/c/" + V1["programid"][v7[i]]["Data"].get<std::string>();
                    brls::Logger::debug(download);
                    std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["Data"].get<std::string>() + ".nca";
                    net.Download(download, out);
                }
                else if (V1["programid"][v7[i]].contains("PublicData") == true)
                {
                    std::string download = "http://192.168.1.128/c/c/" + V1["programid"][v7[i]]["PublicData"].get<std::string>();
                    brls::Logger::debug(download);
                    std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["PublicData"].get<std::string>() + ".nca";
                    net.Download(download, out);
                }
                if (V1["programid"][v7[i]].contains("Meta") == true)
                {
                    std::string download = "http://192.168.1.128/c/a/" + V1["programid"][v7[i]]["Meta"].get<std::string>();
                    brls::Logger::debug(download);
                    std::string out = "/switch/Sys-Updater/temp/" + V1["programid"][v7[i]]["Meta"].get<std::string>() + ".cnmt.nca";
                    net.Download(download, out);
                }
            }
            download = false;
        }
    }
}*/