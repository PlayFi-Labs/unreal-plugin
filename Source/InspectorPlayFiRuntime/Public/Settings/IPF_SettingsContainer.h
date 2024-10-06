// PlayFi data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPF_SettingsContainer.generated.h"

USTRUCT()
struct FInspectorSettings
{
    GENERATED_BODY()

    /** Whether Inspector will be enabled at the start of a game session. */
    UPROPERTY(EditDefaultsOnly)
    bool bEnableInspector = false;

    /** Whether logs will be output to console responsible for Inspector status / errors. */
    UPROPERTY(EditDefaultsOnly)
    bool bEnableDebugLog = false;

    /** Whether each screenshot taken will be saved to the client's computer disk. */
    UPROPERTY(EditDefaultsOnly)
    bool bSaveImageToDisk = false;

    /** The frequency at which Inspector will take screenshots of the client screen. */
    UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1.f, ClampMax = 3600.f))
    float ScreenshotTimerRate = 5.f;

    /**
     * The percentage of screenshot similarity that will count as a duplicate. The screenshot that was taken just now and the last screenshot successfully
     * sent to the server are compared. For example, if the percentage of similarity of screenshots is 98.5%, then these screenshots will be considered
     * as duplicates and the newly made screenshot will not be sent to the server.
     */
    UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.1f, ClampMax = 1.f))
    float DuplicateThresholdPercentage = 0.97f;

    /**
     * Maximum allowable threshold for naming a pixel in a color in which the pixel will be considered a duplicate.
     * During image rendering, pixels have properties to change their color slightly, so we need to take this fact into account and allow pixels to change
     * their color slightly while checking for duplicate images.
     * 
     * For example, there are set colors to check for duplicate images:
     * RGB (100, 100, 100) and RGB (108, 100, 100). The red color is set to 8 units, but if the threshold is equal to the default value of 10,
     * these two colors will be considered duplicates.
     */
    UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1.f, ClampMax = 1000.f))
    float MaxAllowedThresholdForDuplicatedPixel = 10.f;

    /** The path on the client disk where all screenshots will be saved. The path is built relative to the folder with the application. */
    UPROPERTY(EditDefaultsOnly)
    FString SaveImageToDiskPath = TEXT("Saved/InspectorScreenshot.jpeg");

    /** Endpoint where the request to send screenshots will be made. */
    UPROPERTY(EditDefaultsOnly)
    FString Endpoint = TEXT("https://processing.dev-aimt.playfi.ai/requests");

    /** The Boundary to be used when constructing a multipart/form-data request. */
    UPROPERTY(EditDefaultsOnly)
    FString Boundary = TEXT("e243322543af456f9a3273049ca02519");

    /** Value of X-API-KEY header when sending a request to the server. */
    UPROPERTY(EditDefaultsOnly)
    FString X_API_Key =
        TEXT("1P7dopmUdC2Hobg1oRo75oVqLDzj9io5Cp6Iws0XMkCNXTJIoYM8UV5UxdY9EbKfe75bD6hsIt4T16JZhdkRp6lMSL4g3MKGDRp2sOYmltYNviduhge9pJaPCdIK9Aa7");

    /** Value of model_id parameter when sending a request to the server. */
    UPROPERTY(EditDefaultsOnly)
    FString ModelID = TEXT("ac167a82-653c-4cc9-be52-31c9e6a2fa41");

    /** Value of config_name parameter when sending a request to the server. */
    UPROPERTY(EditDefaultsOnly)
    FString ConfigName = TEXT("");

    /** Value of payload parameter when sending a request to the server. */
    UPROPERTY(EditDefaultsOnly)
    FString Payload = TEXT("{}");
};

/** An object that spawns an IPF_InspectorSubsystem to set custom settings. */
UCLASS(Blueprintable, BlueprintType)
class INSPECTORPLAYFIRUNTIME_API UIPF_SettingsContainer : public UObject
{
    GENERATED_BODY()

public:
    /** General Inspector settings. */
    UPROPERTY(EditDefaultsOnly, Category = "PlayFi")
    FInspectorSettings InspectorSettings;
};
