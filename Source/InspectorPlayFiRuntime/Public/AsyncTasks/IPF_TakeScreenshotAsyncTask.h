// PlayFi data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "IPF_TakeScreenshotAsyncTask.generated.h"

UCLASS()
class INSPECTORPLAYFIRUNTIME_API UIPF_TakeScreenshotAsyncTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeScreenshotCompleted, const FString&, ResponceMessage, const TArray<uint8>&, ImageBytes);

public:
    UPROPERTY(BlueprintAssignable)
    FOnTakeScreenshotCompleted OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnTakeScreenshotCompleted OnFailed;

public:
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "PlayFi Take Screenshot", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
        Category = "InspectorPlayFi")
    static UIPF_TakeScreenshotAsyncTask* TakeScreenshotAsyncTask(const UObject* WorldContextObject);

    virtual void Activate() override;

protected:
    void OnBackBufferReady_RenderThread(SWindow& Window, const TRefCountPtr<FRHITexture>& ScreenTexture);

    void TaskFailedWithReason(const FString& Reason);

private:
    UPROPERTY()
    UWorld* WorldWhereTaskWasCalled;

    FSlateApplication* SlateApplication = nullptr;
};
