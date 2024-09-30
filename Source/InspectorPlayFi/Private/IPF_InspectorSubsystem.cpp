// PlayFi data Inspector. All rights reserved.

#include "IPF_InspectorSubsystem.h"
#include "AsyncTasks/IPF_SendScreenshotToServer.h"

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

    UIPF_SendScreenshotToServer* AsyncTask = UIPF_SendScreenshotToServer::SendScreenshotToServer(GetWorld());
    if (!AsyncTask) return;

    AsyncTask->Activate();
}
