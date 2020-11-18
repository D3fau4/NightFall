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
    InstallUpdate(brls::StagedAppletFrame *frame, std::string label, std::string path = "/switch/NightFall/temp/");
    ~InstallUpdate();

    void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) override;
    void layout(NVGcontext *vg, brls::Style *style, brls::FontStash *stash) override;
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;
};