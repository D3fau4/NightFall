#include <math.h>
#include "UI/InstallUpdate.hpp"

#include <math.h>

InstallUpdatePage::InstallUpdatePage(brls::StagedAppletFrame *frame, std::string label)
{
    // Label
    this->button = (new brls::Button(brls::ButtonStyle::BORDERLESS))->setLabel(label);
    this->button->setParent(this);
    this->button->getClickEvent()->subscribe([frame](View *view) {
        if (frame->isLastStage())
            brls::Application::popView();
        else
        {
            frame->nextStage();
            //DownloadUpdatePage::meme();
        }
    });
    this->label = new brls::Label(brls::LabelStyle::DIALOG, "Start Download?", true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
}

void InstallUpdatePage::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx)
{
    this->label->frame(ctx);
    this->button->frame(ctx);
}

brls::View *InstallUpdatePage::getDefaultFocus()
{
    return this->button;
}

void InstallUpdatePage::layout(NVGcontext *vg, brls::Style *style, brls::FontStash *stash)
{
    this->label->setWidth(roundf((float)this->width * style->CrashFrame.labelWidth));
    this->label->invalidate(true);

    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - style->AppletFrame.footerHeight) / 2,
        this->label->getWidth(),
        this->label->getHeight());

    this->button->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2,
        this->y + this->height / 2 + style->CrashFrame.buttonHeight,
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight);
    this->button->invalidate();
}

InstallUpdatePage::~InstallUpdatePage()
{
    delete this->label;
    delete this->button;
}