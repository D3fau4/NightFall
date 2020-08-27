#include "sm/sm.hpp"

namespace sm
{
    bool isRunning(const char *serviceName)
    {
        u8 tmp = 0;
        SmServiceName service_name = smEncodeName(serviceName);
        Result rc = serviceDispatchInOut(smGetServiceSession(), 65100, service_name, tmp);
        if (R_SUCCEEDED(rc) && tmp & 1)
            return true;
    }
}