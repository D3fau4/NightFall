#pragma once

#include <borealis.hpp>
#include "thread.hpp"

class InstallUpdate : public brls::View
{
private:
    brls::StagedAppletFrame *frame;
    brls::ProgressDisplay *progressDisp;
    brls::Label *label;
    brls::Label *label1;
    int progressValue = 0;

public:
    InstallUpdate(brls::StagedAppletFrame *frame);
    ~InstallUpdate();

    void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) override;
    void layout(NVGcontext *vg, brls::Style *style, brls::FontStash *stash) override;
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;
};