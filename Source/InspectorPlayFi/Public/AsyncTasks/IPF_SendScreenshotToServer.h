// PlayFi data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "IPF_SendScreenshotToServer.generated.h"

UCLASS()
class INSPECTORPLAYFI_API UIPF_SendScreenshotToServer : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSendScreenshotToServerCompleted);

public:
    UPROPERTY(BlueprintAssignable)
    FOnSendScreenshotToServerCompleted OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnSendScreenshotToServerCompleted OnFailed;

public:
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "PlayFi Send Screenshot To Server", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
        Category = "InspectorPlayFi")
    static UIPF_SendScreenshotToServer* SendScreenshotToServer(const UObject* WorldContextObject);

    virtual void Activate() override;
    FORCEINLINE void SetWorldWhereTaskWasCalled(UWorld* InWorld);

protected:
    void OnBackBufferReady_RenderThread(SWindow& Window, const TRefCountPtr<FRHITexture>& ScreenTexture);

    void TaskFailedWithReason(const FString& Reason);

private:
    UPROPERTY()
    UWorld* WorldWhereTaskWasCalled;

    FSlateApplication* SlateApplication = nullptr;
};
