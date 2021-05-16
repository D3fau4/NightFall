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

#include "setsys/setsys.hpp"

namespace setsys
{
    std::string GetFirmwareVersion()
    {
        Result ret = 0;
        SetSysFirmwareVersion ver;
        if (R_FAILED(ret = setsysGetFirmwareVersion(&ver)))
        {
            return "0.0.0";
        }
        return ver.display_version;
    }

    Result SetSleepOff()
    {
        Result ret = 0;
        SetSysSleepSettings set;
        if (R_FAILED(ret = setsysGetSleepSettings(&set)))
        {
            return ret;
        }

        set.console_sleep_plan = 5;  // Never sleep
        set.handheld_sleep_plan = 5; // Never sleep

        if (R_FAILED(ret = setsysSetSleepSettings(&set)))
        {
            return ret;
        }
        return ret;
    }

    SetSysSleepSettings GetSleepConfig()
    {
        Result ret = 0;
        SetSysSleepSettings set;
        if (R_FAILED(ret = setsysGetSleepSettings(&set)))
        {
            return set;
        }
        return set;
    }

    Result SetSleepConfig(SetSysSleepSettings *set)
    {
        Result ret = 0;
        if (R_FAILED(ret = setsysSetSleepSettings(set)))
        {
            return ret;
        }
        return ret;
    }
} // namespace setsys