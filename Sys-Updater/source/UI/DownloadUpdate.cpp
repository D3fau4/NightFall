#include "UI/DownloadUpdate.hpp"

#include <math.h>
#include <fstream>
#include <string>
#include <iostream>
#include "Types.hpp"
#include "net/net.hpp"

json jso1n;

DownloadUpdatePage::DownloadUpdatePage(brls::StagedAppletFrame *frame)
    : frame(frame)
{
    std::ifstream i("/switch/Sys-Updater/temp.json");
    i >> jso1n;
    // Label
    this->progressDisp = new brls::ProgressDisplay();
    this->progressDisp->setProgress(this->progressValue, 1000);
    this->progressDisp->setParent(this);
    this->label = new brls::Label(brls::LabelStyle::DIALOG, "Downloading Update data...", true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
    this->label1 = new brls::Label(brls::LabelStyle::DESCRIPTION, "Firmware: " + jso1n["fw_info"]["version"].get<std::string>());
    this->label1->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label1->setParent(this);
}

void DownloadUpdatePage::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx)
{
    if (progressValue == jso1n["fw_info"]["files"].get<int>())
        this->frame->nextStage();
    this->progressDisp->setProgress(this->progressValue, jso1n["fw_info"]["files"].get<int>());
    this->progressDisp->frame(ctx);
    this->label->frame(ctx);
    this->label1->frame(ctx);
}

void DownloadUpdatePage::DownloadUpdate(void)
{
    Network::Net net = Network::Net();
    auto v7 = jso1n["titleids"].get<std::vector<std::string>>();
    int n = v7.size();
    for (int i = 0; i < n; i++)
    {
        if (jso1n["programid"][v7[i]].contains("Program") == true)
        {
            std::string download = "http://192.168.1.128/c/c/" + jso1n["programid"][v7[i]]["Program"].get<std::string>();
            brls::Logger::debug(download);
            std::string out = "/switch/Sys-Updater/temp/" + jso1n["programid"][v7[i]]["Program"].get<std::string>() + ".nca";
            net.Download(download, out);
        }
        else if (jso1n["programid"][v7[i]].contains("Data") == true)
        {
            std::string download = "http://192.168.1.128/c/c/" + jso1n["programid"][v7[i]]["Data"].get<std::string>();
            brls::Logger::debug(download);
            std::string out = "/switch/Sys-Updater/temp/" + jso1n["programid"][v7[i]]["Data"].get<std::string>() + ".nca";
            net.Download(download, out);
        }
        else if (jso1n["programid"][v7[i]].contains("PublicData") == true)
        {
            std::string download = "http://192.168.1.128/c/c/" + jso1n["programid"][v7[i]]["PublicData"].get<std::string>();
            brls::Logger::debug(download);
            std::string out = "/switch/Sys-Updater/temp/" + jso1n["programid"][v7[i]]["PublicData"].get<std::string>() + ".nca";
            net.Download(download, out);
        }
        if (jso1n["programid"][v7[i]].contains("Meta") == true)
        {
            std::string download = "http://192.168.1.128/c/a/" + jso1n["programid"][v7[i]]["Meta"].get<std::string>();
            brls::Logger::debug(download);
            std::string out = "/switch/Sys-Updater/temp/" + jso1n["programid"][v7[i]]["Meta"].get<std::string>() + ".cnmt.nca";
            net.Download(download, out);
        }
    }
}

void DownloadUpdatePage::layout(NVGcontext *vg, brls::Style *style, brls::FontStash *stash)
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

void DownloadUpdatePage::willAppear(bool resetState)
{
    this->progressDisp->willAppear(resetState);
}

void DownloadUpdatePage::willDisappear(bool resetState)
{
    this->progressDisp->willDisappear(resetState);
}

DownloadUpdatePage::~DownloadUpdatePage()
{
    delete this->progressDisp;
    delete this->label;
}