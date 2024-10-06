// PlayFi data Inspector. All rights reserved.

#include "Subsystem/IPF_InspectorSubsystem.h"
#include "AsyncTasks/IPF_SendImageToServerAsyncTask.h"
#include "AsyncTasks/IPF_TakeScreenshotAsyncTask.h"
#include "Settings/IPF_SettingsContainer.h"

DEFINE_LOG_CATEGORY_STATIC(LogInspectorSubsystem, All, All)

UIPF_InspectorSubsystem::UIPF_InspectorSubsystem()
{
    // We are interested in the settings that were specified in the Blueprint, so we need to find out the Blueprint class.

    ConstructorHelpers::FClassFinder<UObject> SettingsContainerBlueprint(TEXT("/InspectorPlayFi/Settings/Settings_InspectorPlayFi"));
    if (!SettingsContainerBlueprint.Succeeded()) return;

    SettingsContainerClass = SettingsContainerBlueprint.Class;
}

void UIPF_InspectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (SettingsContainerClass)
    {
        // If we successfully found Settings Blueprint - we use it for create container with custom user settings.
        const UClass* Class = SettingsContainerClass;
        SettingsContainer = NewObject<UIPF_SettingsContainer>(this, Class);
    }
    else
    {
        // If Settings Blueprint search was unsuccessful, creates a container with default settings.
        SettingsContainer = NewObject<UIPF_SettingsContainer>();
    }
    StartScreenshotTimer();
}

FInspectorSettings UIPF_InspectorSubsystem::GetInspectorSettings() const
{
    if (!SettingsContainer) return FInspectorSettings();
    return SettingsContainer->InspectorSettings;
}

void UIPF_InspectorSubsystem::EnableInspector(const bool bEnable)
{
    if (!SettingsContainer) return;
    SettingsContainer->InspectorSettings.bEnableInspector = bEnable;

    if (bEnable)
    {
        // Handle situation when Inspector was disabled at start.
        const auto GameInstance = GetGameInstance();
        if (!GameInstance || GameInstance->GetTimerManager().IsTimerActive(ScreenshotTimerHandle)) return;

        StartScreenshotTimer();
    }
}

void UIPF_InspectorSubsystem::StartScreenshotTimer()
{
    const auto InspectorSettings = GetInspectorSettings();
    const auto GameInstance = GetGameInstance();

    if (!GameInstance || !InspectorSettings.bEnableInspector) return;

    const FString Msg = FString::Printf(TEXT("Screenshot timer successfully started in rate: %.2f seconds."), InspectorSettings.ScreenshotTimerRate);
    LogMessageIfDebugEnabled(Msg);

    GameInstance->GetTimerManager().SetTimer(ScreenshotTimerHandle, this, &UIPF_InspectorSubsystem::OnScreenshotTimerUpdate,
        InspectorSettings.ScreenshotTimerRate, true);
}

void UIPF_InspectorSubsystem::OnScreenshotTimerUpdate()
{
    if (!GetWorld() || !GetInspectorSettings().bEnableInspector) return;

    UIPF_TakeScreenshotAsyncTask* AsyncTask = UIPF_TakeScreenshotAsyncTask::TakeScreenshotAsyncTask(GetWorld());
    if (!AsyncTask) return;

    AsyncTask->OnSuccess.AddDynamic(this, &UIPF_InspectorSubsystem::OnSuccessfullyTakenScreenshot);
    AsyncTask->Activate();
}

void UIPF_InspectorSubsystem::OnSuccessfullyTakenScreenshot(const FString& ResponseMessage, const TArray<FColor>& ImageColors, const int32& Width,
    const int32& Height)
{
    const auto InspectorSettings = GetInspectorSettings();
    if (!GetWorld() || !InspectorSettings.bEnableInspector || ImageColors.IsEmpty()) return;

    UIPF_SendImageToServerAsyncTask* AsyncTask = UIPF_SendImageToServerAsyncTask::SendScreenshotToServerAsyncTask(GetWorld(), ImageColors, Width, Height,
        InspectorSettings.Endpoint, InspectorSettings.X_API_Key, InspectorSettings.ModelID, InspectorSettings.ConfigName, InspectorSettings.Payload);
    if (!AsyncTask) return;

    AsyncTask->Activate();
}

void UIPF_InspectorSubsystem::LogMessageIfDebugEnabled(const FString& Message, bool bError)
{
    const auto LogEnabled = GetInspectorSettings().bEnableDebugLog;
    if (!LogEnabled) return;

    if (bError)
    {
        UE_LOG(LogInspectorSubsystem, Error, TEXT("%s"), *Message);
    }
    else
    {
        UE_LOG(LogInspectorSubsystem, Display, TEXT("%s"), *Message);
    }
}
