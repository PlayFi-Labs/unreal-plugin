// PlayFi data Inspector. All rights reserved.

#include "AsyncTasks/IPF_TakeScreenshotAsyncTask.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogTakeScreenshotAsyncTask, All, All)

UIPF_TakeScreenshotAsyncTask* UIPF_TakeScreenshotAsyncTask::TakeScreenshotAsyncTask(const UObject* WorldContextObject)
{
    UIPF_TakeScreenshotAsyncTask* Action = NewObject<UIPF_TakeScreenshotAsyncTask>();
    if (!Action || !WorldContextObject)
    {
        UE_LOG(LogTakeScreenshotAsyncTask, Error, TEXT("Failed to create TakeScreenshot async task!"));
        return nullptr;
    }

    Action->WorldWhereTaskWasCalled = WorldContextObject->GetWorld();
    Action->RegisterWithGameInstance(WorldContextObject);
    return Action;
}

void UIPF_TakeScreenshotAsyncTask::Activate()
{
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

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

    if (ImageWrapper->SetRaw(OutColors.GetData(), OutColors.Num() * OutColors.GetTypeSize(), CaptureRect.Width(), CaptureRect.Height(), ERGBFormat::BGRA, 8))
    {
        const TArray64<uint8>& Bytes = ImageWrapper->GetCompressed(0);
        const TArray<uint8> NormalizedArray(Bytes.GetData(), static_cast<int32>(Bytes.Num()));

        const FString FilePath = FPaths::ProjectDir() / TEXT("ScreenshotAsync.jpeg");
        const bool bIsScreenshotSaved = FFileHelper::SaveArrayToFile(Bytes, *FilePath);
        if (!bIsScreenshotSaved)
        {
            UE_LOG(LogTemp, Error, TEXT("Screenshot Save fail"));
        }
        SetReadyToDestroy();
        OnSuccess.Broadcast("Success", NormalizedArray);
    }
    else
    {
        TaskFailedWithReason("Failed to set raw image data for ImageWrapper!");
    }
}

void UIPF_TakeScreenshotAsyncTask::TaskFailedWithReason(const FString& Reason)
{
    UE_LOG(LogTakeScreenshotAsyncTask, Error, TEXT("%s"), *Reason);

    if (SlateApplication)
    {
        SlateApplication->GetRenderer()->OnBackBufferReadyToPresent().RemoveAll(this);
    }
    SetReadyToDestroy();
    OnFailed.Broadcast(Reason, TArray<uint8>());
}
