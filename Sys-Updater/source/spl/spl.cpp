#include <stdio.h>
#include <switch.h>

#include "spl/spl.hpp"

namespace spl
{
    char *GetHardwareType(void)
    {
        Result ret = 0;
        u64 hardware_type = 4;

        if (R_FAILED(rc = splGetConfig(SplConfigItem_HardwareType, &hardware_type)))
        {
            //ChangeMenu(std::make_shared<ErrorMenu>("An error has occurred", "Failed to get hardware type.", rc));
            return;
        }
    }
} // namespace spl