#pragma once

#include <borealis.hpp>
#include "json.hpp"
#include "amssu/amssu.h"
#include <thread>

using json = nlohmann::json;

namespace BackGround
{
  class BackgroundTasks
  {
  public:
    BackgroundTasks();
    ~BackgroundTasks();
    bool m_Download = false;
    bool m_InstallUpdate = false;
    int m_DownloadProgress = 0;
    int m_InstallProgress = 0;

    AmsSuUpdateInformation m_update_info;

  private:
    bool m_running = false;
    void DownloadUpdate();
    std::thread m_downloadUpdateThread;
  };
} // namespace BackGround
