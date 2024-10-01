// PlayFi data Inspector. All rights reserved.

#include "Subsystem/IPF_InspectorSubsystem.h"
#include "AsyncTasks/IPF_TakeScreenshotAsyncTask.h"

void UIPF_InspectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const auto GameInstance = GetGameInstance();
    if (!GameInstance) return;

    GameInstance->GetTimerManager().SetTimer(ScreenshotTimerHandle, this, &UIPF_InspectorSubsystem::OnScreenshotTimerUpdate, ScreenshotTimerRate, true);
}

void UIPF_InspectorSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UIPF_InspectorSubsystem::OnScreenshotTimerUpdate()
{
    if (!GetWorld()) return;

    UIPF_TakeScreenshotAsyncTask* AsyncTask = UIPF_TakeScreenshotAsyncTask::TakeScreenshotAsyncTask(GetWorld());
    if (!AsyncTask) return;

    AsyncTask->Activate();
}
