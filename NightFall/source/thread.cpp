/*Copyright (c) 2020 D3fau4

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "thread.hpp"

#include <math.h>
#include <fstream>
#include <string>
#include <iostream>
#include "net/net.hpp"
#include "FS/FS.hpp"

/* Var */
json V1;
json config;
static constexpr size_t UpdateTaskBufferSize = 0x100000;
AsyncResult m_prepare_result;

std::string Updatepath;
UpdateState m_UpdateState = UpdateState::NeedsValidate;
AmsSuUpdateValidationInfo m_validateinfo;

namespace BackGround
{
    BackgroundTasks::BackgroundTasks()
    {
        brls::Logger::debug("thread arrancado (?)");
        this->m_running = true;
        this->m_Download = false;
        this->m_InstallUpdate = false;

        this->m_downloadUpdateThread = std::thread(&BackgroundTasks::DownloadUpdate, this);
    }

    BackgroundTasks::~BackgroundTasks()
    {
        this->m_running = false;

        this->m_downloadUpdateThread.join();
    }

    void BackgroundTasks::DownloadUpdate()
    {
        std::ifstream o("/switch/NightFall/config.json");
        o >> config;
        Network::Net net = Network::Net(config["UseMemory"].get<int>());
        while (this->m_running)
        {
            if (this->m_Download == true)
            {
                std::ifstream i("/switch/NightFall/temp.json");
                i >> V1;
                auto v7 = V1["titleids"].get<std::vector<std::string>>();
                int n = v7.size();
                for (int i = 0; i < n; i++)
                {
                    if (V1["programid"][v7[i]].contains("Program") == true)
                    {
                        std::string download = config["URL"].get<std::string>() + "c/c/" + V1["programid"][v7[i]]["Program"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/NightFall/temp/" + V1["programid"][v7[i]]["Program"].get<std::string>() + ".nca";
                        if (net.Download(download, out) == true)
                        {
                            brls::Logger::error("error");
                        }
                        else
                            this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                    else if (V1["programid"][v7[i]].contains("Data") == true)
                    {
                        std::string download = config["URL"].get<std::string>() + "c/c/" + V1["programid"][v7[i]]["Data"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/NightFall/temp/" + V1["programid"][v7[i]]["Data"].get<std::string>() + ".nca";
                        if (net.Download(download, out) == true)
                        {
                            brls::Logger::error("error");
                        }
                        else
                            this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                    else if (V1["programid"][v7[i]].contains("PublicData") == true)
                    {
                        std::string download = config["URL"].get<std::string>() + "c/c/" + V1["programid"][v7[i]]["PublicData"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/NightFall/temp/" + V1["programid"][v7[i]]["PublicData"].get<std::string>() + ".nca";
                        if (net.Download(download, out) == true)
                        {
                            brls::Logger::error("error");
                        }
                        else
                            this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                    if (V1["programid"][v7[i]].contains("Meta") == true)
                    {
                        std::string download = config["URL"].get<std::string>() + "c/a/" + V1["programid"][v7[i]]["Meta"].get<std::string>();
                        brls::Logger::debug(download);
                        std::string out = "/switch/NightFall/temp/" + V1["programid"][v7[i]]["Meta"].get<std::string>() + ".cnmt.nca";
                        if (net.Download(download, out) == true)
                        {
                            brls::Logger::error("error");
                        }
                        else
                            this->m_DownloadProgress = this->m_DownloadProgress + 1;
                    }
                }
                o.close();
                this->m_Download = false;
            }
            if (this->m_InstallUpdate == true)
            {
                /* Prevent the home button from being pressed during installation. */
                hiddbgDeactivateHomeButton();
                std::ifstream o("/switch/NightFall/config.json");
                o >> config;
                if (m_UpdateState == UpdateState::NeedsValidate)
                {
                    brls::Logger::debug(this->m_path.c_str());
                    if (R_FAILED(amssuValidateUpdate(&m_validateinfo, this->m_path.c_str())))
                    {
                        brls::Application::crash("No se pudo validar");
                    }
                    if (R_SUCCEEDED(m_validateinfo.result))
                    {
                        brls::Logger::debug("m_UpdateState == UpdateState::NeedsValidate");
                        m_UpdateState = UpdateState::NeedsSetup;
                        this->m_InstallProgress = 1;
                    }
                }
                if (m_UpdateState == UpdateState::NeedsSetup)
                {
                    bool WantExfat = config["Exfat"].get<int>();
                    if (WantExfat == true)
                        brls::Logger::debug("Update Exfat");
                    else
                        brls::Logger::debug("Update No Exfat");
                    if (R_FAILED(amssuSetupUpdate(nullptr, UpdateTaskBufferSize, this->m_path.c_str(), WantExfat)))
                    {
                        brls::Application::crash("Fallo al hacer el setup");
                        //return EXIT_FAILURE;
                    }
                    else
                    {
                        brls::Logger::debug("m_UpdateState == UpdateState::NeedsSetup");
                        m_UpdateState = UpdateState::NeedsPrepare;
                        this->m_InstallProgress = 2;
                    }
                }
                else if (m_UpdateState == UpdateState::NeedsPrepare)
                {
                    if (R_FAILED(amssuRequestPrepareUpdate(&m_prepare_result)))
                    {
                        brls::Application::crash("Fallo al preperar el update");
                        //return EXIT_FAILURE;
                    }
                    brls::Logger::debug("m_UpdateState == UpdateState::NeedsPrepare");
                    m_UpdateState = UpdateState::AwaitingPrepare;
                    this->m_InstallProgress = 3;
                }
                else if (m_UpdateState == UpdateState::AwaitingPrepare)
                {
                    if (R_FAILED(asyncResultWait(&m_prepare_result, 0)))
                    {
                        //brls::Application::crash("Fallo al esperar un resultado");
                    }
                    else if (R_FAILED(asyncResultGet(&m_prepare_result)))
                    {
                        //brls::Application::crash("Fallo al obtener un resultado");
                    }
                    this->m_InstallProgress = 4;
                }

                bool prepared;
                if (R_FAILED(amssuHasPreparedUpdate(&prepared)))
                {
                    brls::Application::crash("Fallo al comprobar que está el update preparado");
                }

                if (prepared)
                {
                    brls::Logger::debug("if (prepared)");
                    m_UpdateState = UpdateState::NeedsApply;
                }

                NsSystemUpdateProgress update_progress = {};
                /*this->m_currentbarsize = static_cast<int>(update_progress.current_size);
                this->m_progressbarsize = static_cast<int>(update_progress.total_size);*/
                if (R_FAILED(amssuGetPrepareUpdateProgress(&update_progress)))
                {
                    brls::Application::crash("Fallo al hacer el setup");
                }

                if (m_UpdateState == UpdateState::NeedsApply)
                {
                    if (R_FAILED(amssuApplyPreparedUpdate()))
                    {
                        brls::Application::crash("Fallo al instalar");
                        // Reboot
                    }
                    brls::Logger::debug("amssuApplyPreparedUpdate()");
                    this->m_InstallProgress = 5;
                    m_UpdateState = UpdateState::AwaitingReboot;
                }
                if (m_UpdateState == UpdateState::AwaitingReboot)
                {
                    if (config["DeleteFolder"].get<int>() == 1)
                        FS::DeleteDir(this->m_path.c_str());
                    brls::Logger::debug("Preparado: Reinicio en 3s");
                    std::this_thread::sleep_for(3s);
                    bpcInitialize();
                    bpcRebootSystem();
                    bpcExit();
                }
            }
        }
    }
} // namespace BackGround