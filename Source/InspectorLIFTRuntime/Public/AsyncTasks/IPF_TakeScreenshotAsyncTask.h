// LIFT data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPF_BaseAsyncTask.h"
#include "IPF_TakeScreenshotAsyncTask.generated.h"

UCLASS()
class INSPECTORLIFTRUNTIME_API UIPF_TakeScreenshotAsyncTask : public UIPF_BaseAsyncTask
{
    GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnTakeScreenshotCompleted, const FString&, ResponseMessage, const TArray<FColor>&, ImageColors, const int32&, Width, const int32&, Height);

public:
    UPROPERTY(BlueprintAssignable)
    FOnTakeScreenshotCompleted OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnTakeScreenshotCompleted OnFailed;

public:
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "LIFT Take Screenshot", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
        Category = "InspectorLIFT")
    static UIPF_TakeScreenshotAsyncTask* TakeScreenshotAsyncTask(const UObject* WorldContextObject);

    virtual void Activate() override;

protected:
    void OnBackBufferReady_RenderThread(SWindow& Window, const TRefCountPtr<FRHITexture>& ScreenTexture);

    void TaskFailedWithReason(const FString& Reason);

private:
    FSlateApplication* SlateApplication = nullptr;
};
