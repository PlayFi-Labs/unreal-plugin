// LIFT data Inspector. All rights reserved.

#include "AsyncTasks/IPF_BaseAsyncTask.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Settings/IPF_SettingsContainer.h"
#include "Subsystem/IPF_InspectorSubsystem.h"

void UIPF_BaseAsyncTask::Activate()
{
    if (!WorldWhereTaskWasCalled) return;

    const auto GameInstance = WorldWhereTaskWasCalled->GetGameInstance();
    if (!GameInstance) return;

    InspectorSubsystem = GameInstance->GetSubsystem<UIPF_InspectorSubsystem>();
}

FInspectorSettings UIPF_BaseAsyncTask::GetInspectorSettings() const
{
    if (!InspectorSubsystem) return FInspectorSettings();
    return InspectorSubsystem->GetInspectorSettings();
}

TArray<uint8> UIPF_BaseAsyncTask::GetImageBytesFromColors(const TArray<FColor>& FromColors, const int32& Width, const int32& Height)
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

    if (!ImageWrapper->SetRaw(FromColors.GetData(), FromColors.Num() * FromColors.GetTypeSize(), Width, Height, ERGBFormat::BGRA, 8)) return TArray<uint8>();

    const TArray64<uint8>& Bytes = ImageWrapper->GetCompressed(0);
    return TArray(Bytes.GetData(), static_cast<int32>(Bytes.Num()));
}

void UIPF_BaseAsyncTask::LogMessageIfDebugEnabled(const FString& Message, bool bError)
{
    const auto LogEnabled = GetInspectorSettings().bEnableDebugLog;
    if (!LogEnabled) return;

    if (bError)
    {
        UE_LOG(LogInspectorAsyncTask, Error, TEXT("%s"), *Message);
    }
    else
    {
        UE_LOG(LogInspectorAsyncTask, Display, TEXT("%s"), *Message);
    }
}
