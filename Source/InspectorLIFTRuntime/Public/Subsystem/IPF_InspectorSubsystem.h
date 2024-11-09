// LIFT data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/IPF_SettingsContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IPF_InspectorSubsystem.generated.h"

class UIPF_SettingsContainer;

/*
 * The base class of the subsystem with which communication should take place.
 * Responsible for enabling/disabling the inspector, its settings and their initialization.
 * It works autonomously at game startup and throughout the session, which means that it can be accessed from any level.
 */
UCLASS(Blueprintable, BlueprintType)
class INSPECTORLIFTRUNTIME_API UIPF_InspectorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category = "InspectorLIFT")
    FString UserID;

    /** Stores the FColor array of the last image that was successfully sent to the server. Used to check for duplicates. */
    TArray<FColor> LastImageColors;

public:
    UIPF_InspectorSubsystem();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    FInspectorSettings GetInspectorSettings() const;

    /** Allows you to enable / disable Inspector during runtime. If Inspector was completely turned off during method call - timer will start. */
    UFUNCTION(BlueprintCallable, Category = "InspectorLIFT")
    void EnableInspector(bool bEnable);

protected:
    void StartScreenshotTimer();
    void OnScreenshotTimerUpdate();
    void LogMessageIfDebugEnabled(const FString& Message, bool bError = false);

    UFUNCTION()
    void OnSuccessfullyTakenScreenshot(const FString& ResponseMessage, const TArray<FColor>& ImageColors, const int32& Width, const int32& Height);

private:
    UPROPERTY()
    UIPF_SettingsContainer* SettingsContainer = nullptr;

    TSubclassOf<UIPF_SettingsContainer> SettingsContainerClass;
    FTimerHandle ScreenshotTimerHandle;
};
