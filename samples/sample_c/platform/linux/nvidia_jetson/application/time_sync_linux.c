#include "time_sync_linux.h"
#include "dji_platform.h"
#include "dji_logger.h"

T_DjiReturnCode DjiTest_PpsSignalResponseInit(void)
{
    // Jetson: no hardware PPS wiring by default; use OSAL time as fallback
    USER_LOG_INFO("Time sync: using OSAL local time as PPS fallback on Jetson.");
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_GetNewestPpsTriggerLocalTimeUs(uint64_t *localTimeUs)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    if (osalHandler == NULL || localTimeUs == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    return osalHandler->GetTimeUs(localTimeUs);
}