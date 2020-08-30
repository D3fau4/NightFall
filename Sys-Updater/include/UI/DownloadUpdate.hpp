#pragma once

#include <borealis.hpp>
#include "json.hpp"

using json = nlohmann::json;

class DownloadUpdatePage : public brls::View
{
  private:
    brls::StagedAppletFrame* frame;
    brls::ProgressDisplay* progressDisp;
    brls::Label* label;
    brls::Label* label1;
    int progressValue = 0;

  public:
    DownloadUpdatePage(brls::StagedAppletFrame* frame);
    ~DownloadUpdatePage();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
    void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
    void DownloadUpdate(void);
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;
};