// PlayFi data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "IPF_SendImageToServerAsyncTask.generated.h"

class IHttpRequest;
class IHttpResponse;

UCLASS()
class INSPECTORPLAYFIRUNTIME_API UIPF_SendImageToServerAsyncTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSendImageToServerCompleted, int32, ResponseCode, FString, ResponseContent);

public:
    UPROPERTY(BlueprintAssignable)
    FOnSendImageToServerCompleted OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnSendImageToServerCompleted OnFailed;

public:
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "PlayFi Send Image To Server", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
        Category = "InspectorPlayFi")
    static UIPF_SendImageToServerAsyncTask* SendScreenshotToServerAsyncTask(const UObject* WorldContextObject, const TArray<uint8>& ImageBytes);
    
    virtual void Activate() override;

protected:
    void OnResponseReceived(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bConnectedSuccessfully);

private:
    UPROPERTY()
    TArray<uint8> ImageBytes;

private:
    static TArray<uint8> FStringToUint8(const FString& InString);
};
