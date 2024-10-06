// PlayFi data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPF_BaseAsyncTask.h"
#include "IPF_SendImageToServerAsyncTask.generated.h"

class IHttpRequest;
class IHttpResponse;

UCLASS()
class INSPECTORPLAYFIRUNTIME_API UIPF_SendImageToServerAsyncTask : public UIPF_BaseAsyncTask
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
    static UIPF_SendImageToServerAsyncTask* SendScreenshotToServerAsyncTask( //
        const UObject* WorldContextObject,                                   //
        const TArray<FColor>& ImageColors,                                   //
        const int32& ImageWidth,                                             //
        const int32& ImageHeight,                                            //
        const FString& Endpoint,                                             //
        const FString& X_API_Key,                                            //
        const FString& ModelID,                                              //
        const FString& ConfigName,                                           //
        const FString& Payload                                               //
        );

    virtual void Activate() override;

protected:
    void OnResponseReceived(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bConnectedSuccessfully);

    /** Checks the current ImageColors with the previous one that was successfully sent to the server. Returns true if the duplicate is confirmed. */
    bool DuplicateCheck();
    
    void TaskFailed(const TSharedPtr<IHttpResponse>& Response);
    void LogHttpRequest(const TArray<uint8>& ImageBytes);

private:
    UPROPERTY()
    TArray<FColor> ImageColors;

    int32 ImageWidth = 0;
    int32 ImageHeight = 0;

    FString Endpoint;
    FString X_API_Key;
    FString ModelID;
    FString ConfigName;
    FString Payload;

private:
    static TArray<uint8> FStringToUint8(const FString& InString);
};
