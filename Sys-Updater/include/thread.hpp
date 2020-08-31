#pragma once

#include <borealis.hpp>
#include "json.hpp"
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
    int m_DownloadProgress = 0;
  private:
    bool m_running = false;
    void DownloadUpdate();
    std::thread m_downloadUpdateThread;
  };
} // namespace BackGround
