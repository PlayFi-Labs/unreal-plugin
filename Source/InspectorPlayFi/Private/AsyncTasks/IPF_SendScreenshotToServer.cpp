// PlayFi data Inspector. All rights reserved.

#include "AsyncTasks/IPF_SendScreenshotToServer.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogSendScreenshotToServerAsyncTask, All, All)

UIPF_SendScreenshotToServer* UIPF_SendScreenshotToServer::SendScreenshotToServer(const UObject* WorldContextObject)
{
    UIPF_SendScreenshotToServer* Action = NewObject<UIPF_SendScreenshotToServer>();
    if (!Action || !WorldContextObject)
    {
        UE_LOG(LogSendScreenshotToServerAsyncTask, Error, TEXT("Failed to create SendScreenshotToServer async task!"));
        return nullptr;
    }

    Action->SetWorldWhereTaskWasCalled(WorldContextObject->GetWorld());
    Action->RegisterWithGameInstance(WorldContextObject);
    return Action;
}

void UIPF_SendScreenshotToServer::SetWorldWhereTaskWasCalled(UWorld* InWorld)
{
    if (!InWorld) return;
    WorldWhereTaskWasCalled = InWorld;
}

void UIPF_SendScreenshotToServer::Activate()
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

    SlateApplication->GetRenderer()->OnBackBufferReadyToPresent().AddUObject(this, &UIPF_SendScreenshotToServer::OnBackBufferReady_RenderThread);
}

void UIPF_SendScreenshotToServer::OnBackBufferReady_RenderThread(SWindow& Window, const TRefCountPtr<FRHITexture>& ScreenTexture)
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

        const FString FilePath = FPaths::ProjectDir() / TEXT("ScreenshotAsync.png");
        const bool bIsScreenshotSaved = FFileHelper::SaveArrayToFile(Bytes, *FilePath);
        if (!bIsScreenshotSaved)
        {
            UE_LOG(LogTemp, Error, TEXT("Screenshot Save fail"));
        }
        SetReadyToDestroy();
        OnSuccess.Broadcast();
    }
    else
    {
        TaskFailedWithReason("Failed to set raw image data for ImageWrapper!");
    }
}

void UIPF_SendScreenshotToServer::TaskFailedWithReason(const FString& Reason)
{
    UE_LOG(LogSendScreenshotToServerAsyncTask, Error, TEXT("%s"), *Reason);

    if (SlateApplication)
    {
        SlateApplication->GetRenderer()->OnBackBufferReadyToPresent().RemoveAll(this);
    }
    SetReadyToDestroy();
    OnFailed.Broadcast();
}
