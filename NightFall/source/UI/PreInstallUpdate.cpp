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

#include <math.h>
#include "UI/PreInstallUpdate.hpp"
#include "net/net.hpp"

#include <math.h>
namespace i18n = brls::i18n;	// for loadTranslations() and getStr()
using namespace i18n::literals; // for _i18n

PreInstallUpdatePage::PreInstallUpdatePage(brls::StagedAppletFrame *frame, std::string label)
{
    // Label
    this->button = (new brls::Button(brls::ButtonStyle::BORDERLESS))->setLabel(label);
    this->button->setParent(this);
    this->button->getClickEvent()->subscribe([frame](View *view) {
        if (frame->isLastStage())
            brls::Application::popView();
        else
        {
			Chain();
            frame->nextStage();
            //DownloadUpdatePage::meme();
        }
    });
    this->label = new brls::Label(brls::LabelStyle::DIALOG, "PreInstallUpdate/start"_i18n.c_str(), true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
}

void PreInstallUpdatePage::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx)
{
    this->label->frame(ctx);
    this->button->frame(ctx);
}

brls::View *PreInstallUpdatePage::getDefaultFocus()
{
    return this->button;
}

void PreInstallUpdatePage::layout(NVGcontext *vg, brls::Style *style, brls::FontStash *stash)
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

PreInstallUpdatePage::~PreInstallUpdatePage()
{
    delete this->label;
    delete this->button;
}