/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: element_name.cpp
 */

#include "abilityms_slite_client.h"

namespace OHOS {
namespace AbilitySlite {
// bool AbilityMsClient::Initialize() const
// {
//     return false;
// }

// int AbilityMsClient::StartAbility(const Want *want) const
// {
//     return -1;
// }

// int AbilityMsClient::StartAbility(const Want *want, TransitionType type, bool enableSlideBack) const
// {
//     return -1;
// }

// int AbilityMsClient::TerminateAbility(uint64_t token) const
// {
//     return -1;
// }

// int AbilityMsClient::TerminateAbility(uint64_t token, TransitionType type) const
// {
//     return -1;
// }

// int AbilityMsClient::SchedulerLifecycleDone(uint64_t token, int state) const
// {
//     return -1;
// }

// int AbilityMsClient::ForceStopBundle(uint64_t token) const
// {
//     return -1;
// }

// ElementName *AbilityMsClient::GetTopAbility() const
// {
//     return nullptr;
// }

// int AbilityMsClient::ForceStop(char *bundleName) const
// {
//     return -1;
// }

// int AbilityMsClient::ForceStop(char *bundleName, TransitionType type) const
// {
//     return -1;
// }


int AbilityMsClient::StartAbility(const Want *want, uint8_t type, bool enableSlideBack) const
{
    return -1;
}
// ohos end
int32_t AbilityMsClient::TerminateAbility(uint64_t token) const
{
    return -1;
}
// ohos begin
// ams, Adapt to nativefwk
int AbilityMsClient::TerminateAbility(uint64_t token, uint8_t type) const
{
    return -1;
}
// ohos end
int32_t AbilityMsClient::TerminateMission(uint32_t mission) const
{
    return -1;
}

int32_t AbilityMsClient::TerminateAll(const char* excludedBundleName) const
{
    return -1;
}

int32_t AbilityMsClient::SchedulerLifecycleDone(uint64_t token, int32_t state) const
{
    return -1;
}

int32_t AbilityMsClient::ForceStopBundle(uint64_t token) const
{
    return -1;
}

int32_t AbilityMsClient::ForceStop(const char *bundleName) const
{
    return -1;
}

int32_t AbilityMsClient::ForceStop(const Want *want) const
{
    return -1;
}
// ohos begin
// ams, Adapt to nativefwk
int AbilityMsClient::ForceStop(const char *bundleName, uint8_t type) const
{
    return -1;
}
// ohos end
ElementName *AbilityMsClient::GetTopAbility() const
{
    return nullptr;
}

void AbilityMsClient::SetServiceIdentity(const Identity *identity)
{
    return;
}

int32_t AbilityMsClient::AddAbilityRecordObserver(AbilityRecordObserver *observer)
{
    return -1;
}

int32_t AbilityMsClient::RemoveAbilityRecordObserver(AbilityRecordObserver *observer)
{
    return -1;
}

MissionInfoList *AbilityMsClient::GetMissionInfos(uint32_t maxNum) const
{
    return nullptr;
}

}
} //  namespace OHOS
