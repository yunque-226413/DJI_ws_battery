/**
 ********************************************************************
 * @file    time_sync_linux.h
 * @brief   Linux (Jetson) time sync handler using OSAL time source.
 *********************************************************************
 */

#ifndef TIME_SYNC_LINUX_H
#define TIME_SYNC_LINUX_H

#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

T_DjiReturnCode DjiTest_PpsSignalResponseInit(void);
T_DjiReturnCode DjiTest_GetNewestPpsTriggerLocalTimeUs(uint64_t *localTimeUs);

#ifdef __cplusplus
}
#endif

#endif // TIME_SYNC_LINUX_H

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/