#include "UI/InstallUpdate.hpp"
#include <math.h>
#include <fstream>
#include <string>

BackGround::BackgroundTasks Install;
json v2;
InstallUpdate::InstallUpdate(brls::StagedAppletFrame *frame)
    : frame(frame)
{
    // Label
    this->progressDisp = new brls::ProgressDisplay();
    this->progressDisp->setProgress(0, 100);
    this->progressDisp->setParent(this);
    this->label = new brls::Label(brls::LabelStyle::DIALOG, "Prepare Update data...", true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
    std::string version_str = std::to_string((Install.m_update_info.version >> 26) & 0x1F) + "." + std::to_string((Install.m_update_info.version >> 20) & 0x1F) + "." + std::to_string((Install.m_update_info.version >> 16) & 0xF);
    this->label1 = new brls::Label(brls::LabelStyle::DESCRIPTION, version_str);
    this->label1->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label1->setParent(this);
}

InstallUpdate::~InstallUpdate()
{
    delete this->progressDisp;
    delete this->label;
    delete this->label1;
}

void InstallUpdate::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx)
{
    if (Install.m_InstallProgress == 5)
    {
        Install.m_InstallUpdate = false;
        this->frame->nextStage();
    }
    else if (Install.m_InstallUpdate != true)
    {
        Install.m_InstallUpdate = true;
    }
    this->progressDisp->setProgress(Install.m_InstallProgress, 5);
    this->progressDisp->frame(ctx);
    this->label->frame(ctx);
    this->label1->frame(ctx);
}

void InstallUpdate::layout(NVGcontext *vg, brls::Style *style, brls::FontStash *stash)
{
    this->label->setWidth(roundf((float)this->width * style->CrashFrame.labelWidth));
    this->label->invalidate(true);

    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - style->AppletFrame.footerHeight) / 2,
        this->label->getWidth(),
        this->label->getHeight());

    this->label1->setWidth(roundf((float)this->width * style->CrashFrame.labelWidth));
    this->label->invalidate(true);

    this->label1->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 9.2,
        this->y + (this->height - style->AppletFrame.footerHeight) / 1.7,
        this->label1->getWidth(),
        this->label1->getHeight());

    this->progressDisp->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth,
        this->y + this->height / 2,
        style->CrashFrame.buttonWidth * 2,
        style->CrashFrame.buttonHeight);
}

void InstallUpdate::willAppear(bool resetState)
{
    this->progressDisp->willAppear(resetState);
}

void InstallUpdate::willDisappear(bool resetState)
{
    this->progressDisp->willDisappear(resetState);
}