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

    // Save the resulting World to retrieve the client's Viewport later during screen image creation.
    Action->WorldWhereTaskWasCalled = WorldContextObject->GetWorld();

    // Register a Task to manually monitor its deletion after it is completed.
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

    /*
     * After successful receipt of SlateApplication, we subscribe to the OnBackBufferReadyToPresent delegate,
     * which when Broadcast returns the texture ready for rendering on the screen,
     * from which we will get the image for the future screenshot.
     */
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

    /*
     * Get the current list of render commands that are queued.
     * Let's use this to get the data for the current rendering of the image on the screen.
     */
    FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
    const FIntRect CaptureRect(0, 0, ScreenTexture->GetSizeX(), ScreenTexture->GetSizeY());

    /** Read into the FColor array the data for the current image renderer. */
    TArray<FColor> OutColors;
    RHICmdList.ReadSurfaceData(ScreenTexture, CaptureRect, OutColors, FReadSurfaceDataFlags());

    /*
     * Clear the OnBackBufferReadyToPresent delegate to ignore the following render ready messages.
     * We are interested in only 1 call, as we take 1 screenshot and that's the end of the task.
     */
    SlateApplication->GetRenderer()->OnBackBufferReadyToPresent().RemoveAll(this);

    const auto InspectorSettings = GetInspectorSettings();
    if (InspectorSettings.bSaveImageToDisk)
    {
        /** The process of saving an image to disk, if the inspector settings allow it. */
        const TArray<uint8>& Bytes = GetImageBytesFromColors(OutColors, CaptureRect.Width(), CaptureRect.Height());
        const FString FilePath = FPaths::ProjectDir() / InspectorSettings.SaveImageToDiskPath;
        const bool bSavedSuccess = FFileHelper::SaveArrayToFile(Bytes, *FilePath);

        if (!bSavedSuccess)
        {
            UE_LOG(LogInspectorAsyncTask, Error, TEXT("Failed to save image in path: %s"), *FilePath);
        }
    }

    /** The task is completed, we can prepare the object for destruction. */
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
