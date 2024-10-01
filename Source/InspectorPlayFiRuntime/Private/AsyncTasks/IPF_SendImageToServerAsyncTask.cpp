// PlayFi data Inspector. All rights reserved.

#include "AsyncTasks/IPF_SendImageToServerAsyncTask.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

DEFINE_LOG_CATEGORY_STATIC(LogSendScreenshotToServerAsyncTask, All, All)

UIPF_SendImageToServerAsyncTask* UIPF_SendImageToServerAsyncTask::SendScreenshotToServerAsyncTask(const UObject* WorldContextObject,
    const TArray<uint8>& ImageBytes)
{
    UIPF_SendImageToServerAsyncTask* Action = NewObject<UIPF_SendImageToServerAsyncTask>();
    if (!Action)
    {
        UE_LOG(LogSendScreenshotToServerAsyncTask, Error, TEXT("Failed to create SendScreenshotToServer async task!"));
        return nullptr;
    }

    Action->ImageBytes = ImageBytes;
    Action->RegisterWithGameInstance(WorldContextObject);
    return Action;
}

void UIPF_SendImageToServerAsyncTask::Activate()
{
    const FString URL = TEXT("https://processing.dev-aimt.playfi.ai/requests");
    const FString APIKey =
        TEXT("1P7dopmUdC2Hobg1oRo75oVqLDzj9io5Cp6Iws0XMkCNXTJIoYM8UV5UxdY9EbKfe75bD6hsIt4T16JZhdkRp6lMSL4g3MKGDRp2sOYmltYNviduhge9pJaPCdIK9Aa7");
    const FString ModelID = TEXT("ac167a82-653c-4cc9-be52-31c9e6a2fa41");
    const FString Boundary = TEXT("----WebKitFormBoundary7MA4YWxkTrZu0gW");

    const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=" + Boundary));
    HttpRequest->SetHeader(TEXT("X-API-KEY"), APIKey);
    
    TArray<uint8> Content;
    
    const FString ModelIDData = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"model_id\"\r\n\r\n%s\r\n"), *Boundary, *ModelID);
    Content.Append(FStringToUint8(ModelIDData));
    
    const FString ImageData = FString::Printf(
        TEXT("--%s\r\nContent-Disposition: form-data; name=\"image\"; filename=\"screenshot.jpeg\"\r\nContent-Type: image/jpeg\r\n\r\n"), *Boundary);
    Content.Append(FStringToUint8(ImageData));
    Content.Append(ImageBytes);
    
    const FString EndRequestData = FString::Printf(TEXT("\r\n--%s--\r\n"), *Boundary);
    Content.Append(FStringToUint8(EndRequestData));
    
    HttpRequest->SetContent(Content);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UIPF_SendImageToServerAsyncTask::OnResponseReceived);
    HttpRequest->ProcessRequest();
}

void UIPF_SendImageToServerAsyncTask::OnResponseReceived(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bConnectedSuccessfully)
{
    if (Response.IsValid())
    {
        if (bConnectedSuccessfully)
        {
            OnSuccess.Broadcast(Response->GetResponseCode(), Response->GetContentAsString());
        }
        else
        {
            OnFailed.Broadcast(Response->GetResponseCode(), Response->GetContentAsString());
        }
    }
    SetReadyToDestroy();
}

TArray<uint8> UIPF_SendImageToServerAsyncTask::FStringToUint8(const FString& InString)
{
    TArray<uint8> Output;

    if (InString.Len() > 0)
    {
        const FTCHARToUTF8 Converted(*InString);
        Output.Append(reinterpret_cast<const uint8*>(Converted.Get()), Converted.Length());
    }

    return Output;
}
