// PlayFi data Inspector. All rights reserved.

#include "AsyncTasks/IPF_TakeScreenshotAsyncTask.h"
#include "Settings/IPF_SettingsContainer.h"

UIPF_TakeScreenshotAsyncTask* UIPF_TakeScreenshotAsyncTask::TakeScreenshotAsyncTask(const UObject* WorldContextObject)
{
    UIPF_TakeScreenshotAsyncTask* Action = NewObject<UIPF_TakeScreenshotAsyncTask>();
    if (!Action || !WorldContextObject)
    {
        UE_LOG(LogInspectorAsyncTask, Error, TEXT("Failed to create TakeScreenshot async task!"));
        return nullptr;
    }

    Action->WorldWhereTaskWasCalled = WorldContextObject->GetWorld();
    Action->RegisterWithGameInstance(WorldContextObject);
    return Action;
}

void UIPF_TakeScreenshotAsyncTask::Activate()
{
    Super::Activate();

    SlateApplication = &FSlateApplication::Get();
    if (!SlateApplication)
    {
        TaskFailedWithReason("Failed to get current application instance!");
        return;
    }
    if (!SlateApplication->GetRenderer())
    {
        TaskFailedWithReason("Failed to get application renderer!");
        return;
    }

    SlateApplication->GetRenderer()->OnBackBufferReadyToPresent().AddUObject(this, &UIPF_TakeScreenshotAsyncTask::OnBackBufferReady_RenderThread);
}

void UIPF_TakeScreenshotAsyncTask::OnBackBufferReady_RenderThread(SWindow& Window, const TRefCountPtr<FRHITexture>& ScreenTexture)
{
    if (!WorldWhereTaskWasCalled)
    {
        TaskFailedWithReason("Trying to send screenshot from invalid World!");
        return;
    }

    if (!ScreenTexture.IsValid())
    {
        TaskFailedWithReason("Invalid screen texture that returned from buffer!");
        return;
    }

    const UGameViewportClient* ViewportClient = WorldWhereTaskWasCalled->GetGameViewport();
    if (!ViewportClient || !ViewportClient->Viewport)
    {
        TaskFailedWithReason("Invalid client viewport!");
        return;
    }

    FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
    const FIntRect CaptureRect(0, 0, ScreenTexture->GetSizeX(), ScreenTexture->GetSizeY());

    TArray<FColor> OutColors;
    RHICmdList.ReadSurfaceData(ScreenTexture, CaptureRect, OutColors, FReadSurfaceDataFlags());

    SlateApplication->GetRenderer()->OnBackBufferReadyToPresent().RemoveAll(this);

    const auto InspectorSettings = GetInspectorSettings();
    if (InspectorSettings.bSaveImageToDisk)
    {
        const TArray<uint8>& Bytes = GetImageBytesFromColors(OutColors, CaptureRect.Width(), CaptureRect.Height());
        const FString FilePath = FPaths::ProjectDir() / InspectorSettings.SaveImageToDiskPath;
        const bool bSavedSuccess = FFileHelper::SaveArrayToFile(Bytes, *FilePath);

        if (!bSavedSuccess)
        {
            UE_LOG(LogInspectorAsyncTask, Error, TEXT("Failed to save image in path: %s"), *FilePath);
        }
    }

    SetReadyToDestroy();
    OnSuccess.Broadcast("Success", OutColors, CaptureRect.Width(), CaptureRect.Height());
}

void UIPF_TakeScreenshotAsyncTask::TaskFailedWithReason(const FString& Reason)
{
    if (SlateApplication)
    {
        SlateApplication->GetRenderer()->OnBackBufferReadyToPresent().RemoveAll(this);
    }

    LogMessageIfDebugEnabled(Reason, true);
    OnFailed.Broadcast(Reason, TArray<FColor>(), 0, 0);
    SetReadyToDestroy();
}
