这段代码以1Hz频率订阅电池电量信息
1. 电池订阅配置
在 DjiTest_FcSubscriptionStartService() 函数中，可以看到明确的1Hz电池订阅：
c
// 以1Hz频率订阅电池单体信息（电池1）
djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX1,
                                           DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, NULL);
// 以1Hz频率订阅电池单体信息（电池2）  
djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX2,
                                           DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, NULL);
// 以1Hz频率订阅整体电池信息
djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_INFO,
                                           DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, NULL);
2. 电池数据获取和显示
在 UserFcSubscription_Task 任务中，每秒获取并显示电池信息：

2.1 电池1信息获取
c
djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX1,
                                                  (uint8_t *) &singleBatteryInfo,
                                                  sizeof(T_DjiFcSubscriptionSingleBatteryInfo),
                                                  &timestamp);
if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    USER_LOG_ERROR("get value of topic battery single info index1 error.");
} else if (s_userFcSubscriptionDataShow == true) {
    USER_LOG_INFO("battery1: capacity %ld%% voltage %ldV temperature %.2f°C.",
                  singleBatteryInfo.batteryCapacityPercent,
                  singleBatteryInfo.currentVoltage / 1000,
                  (dji_f32_t) singleBatteryInfo.batteryTemperature / 10);
}
2.2 电池2信息获取
c
djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX2,
                                                  (uint8_t *) &singleBatteryInfo,
                                                  sizeof(T_DjiFcSubscriptionSingleBatteryInfo),
                                                  &timestamp);
if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    USER_LOG_DEBUG("get value of topic battery single info index2 error or unavailable.");
} else if (s_userFcSubscriptionDataShow == true) {
    USER_LOG_INFO("battery2: capacity %ld%% voltage %ldV temperature %.2f°C.",
                  singleBatteryInfo

这段代码通过多种日志输出机制将电池消息推送到终端。

1. 主要的日志输出函数
1.1 USER_LOG_INFO - 主要推送函数
c
USER_LOG_INFO("battery1: capacity %ld%% voltage %ldV temperature %.2f°C.",
              singleBatteryInfo.batteryCapacityPercent,
              singleBatteryInfo.currentVoltage / 1000,
              (dji_f32_t) singleBatteryInfo.batteryTemperature / 10);
1.2 USER_LOG_ERROR - 错误信息推送
c
USER_LOG_ERROR("get value of topic battery single info index1 error.");
1.3 USER_LOG_DEBUG - 调试信息推送
c
USER_LOG_DEBUG("get value of topic battery single info index2 error or unavailable.");
2. 日志系统的底层实现
这些 USER_LOG_xxx 宏最终调用到在 main.c 中注册的日志处理程序：

2.1 控制台输出处理程序
c
static T_DjiReturnCode DjiUser_PrintConsole(const uint8_t *data, uint16_t dataLen)
{
    USER_UTIL_UNUSED(dataLen);

    printf("%s", data);  // 这里实际输出到终端

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
2.2 文件日志处理程序
c
static T_DjiReturnCode DjiUser_LocalWrite(const uint8_t *data, uint16_t dataLen)
{
    uint32_t realLen;

    if (s_djiLogFile == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    realLen = fwrite(data, 1, dataLen, s_djiLogFile);  // 写入日志文件
    fflush(s_djiLogFile);
    
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
3. 日志系统注册
在 DjiUser_PrepareSystemEnvironment() 中注册了两个日志输出通道：

c
T_DjiLoggerConsole printConsole = {
    .func = DjiUser_PrintConsole,           // 控制台输出
    .consoleLevel = DJI_LOGGER_CONSOLE_LOG_LEVEL_INFO,
    .isSupportColor = true,
};

T_DjiLoggerConsole localRecordConsole = {
    .consoleLevel = DJI_LOGGER_CONSOLE_LOG_LEVEL_DEBUG,
    .func = DjiUser_LocalWrite,            // 文件日志
    .isSupportColor = true,
};

returnCode = DjiLogger_AddConsole(&printConsole);
returnCode = DjiLogger_AddConsole(&localRecordConsole);
4. 完整的消息推送流程
4.1 电池数据获取周期
c
// 每秒执行一次
while (1) {
    osalHandler->TaskSleepMs(1000 / FC_SUBSCRIPTION_TASK_FREQ);  // 休眠1秒
    
    // 获取电池数据
    djiStat = DjiFcSubscription_GetLatestValueOfTopic(...);
    
    // 推送消息到终端
    if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && s_userFcSubscriptionDataShow == true) {
        USER_LOG_INFO("battery1: capacity %ld%% voltage %ldV temperature %.2f°C.", ...);
    }
}
4.2 消息显示控制
通过 s_userFcSubscriptionDataShow 变量控制是否显示数据：

c
static bool s_userFcSubscriptionDataShow = false;

// 默认启用数据显示
s_userFcSubscriptionDataShow = true;
USER_LOG_INFO("Fc subscription data show enabled.");
5. 实际的终端输出
当程序运行时，终端会看到类似这样的输出：

text
[INFO] Fc subscription data show enabled.
[INFO] battery1: capacity 85% voltage 16.8V temperature 25.5°C.
[INFO] battery2: capacity 83% voltage 16.7V temperature 26.2°C.
[INFO] battery whole: capacity 4500mAh voltage 16800mV current 1200mA percent 84%.
[INFO] battery1: capacity 84% voltage 16.7V temperature 25.8°C.
[INFO] battery2: capacity 82% voltage 16.6V temperature 26.0°C.
...
6. 日志级别控制
代码支持不同级别的日志输出：

INFO级别: 电池数据、状态信息

ERROR级别: 错误和异常情况

DEBUG级别: 调试信息（通常在生产中关闭）

7. 多目标输出
消息同时推送到：

终端/控制台 - 通过 printf 实时显示

日志文件 - 通过 fwrite 保存到文件，文件路径在 DJI_LOG_PATH

8. 时间戳和格式化
DJI日志系统会自动为每条消息添加：

时间戳

日志级别标签（[INFO]、[ERROR]、[DEBUG]）

颜色支持（如果终端支持）

总结： 电池数据通过 USER_LOG_INFO 函数调用，经过DJI日志系统处理，最终通过 printf 输出到终端，同时也会保存到日志文件中，实现每秒一次的实时推送显示。

