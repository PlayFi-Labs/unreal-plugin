// PlayFi data Inspector. All rights reserved.

#include "AsyncTasks/IPF_SendImageToServerAsyncTask.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/IPF_SettingsContainer.h"
#include "Subsystem/IPF_InspectorSubsystem.h"

UIPF_SendImageToServerAsyncTask* UIPF_SendImageToServerAsyncTask::SendScreenshotToServerAsyncTask(const UObject* WorldContextObject,
    const TArray<FColor>& ImageColors, const int32& ImageWidth, const int32& ImageHeight, const FString& Endpoint, const FString& X_API_Key,
    const FString& ModelID, const FString& ConfigName, const FString& Payload)
{
    UIPF_SendImageToServerAsyncTask* Action = NewObject<UIPF_SendImageToServerAsyncTask>();
    if (!Action)
    {
        UE_LOG(LogInspectorAsyncTask, Error, TEXT("Failed to create SendScreenshotToServer async task!"));
        return nullptr;
    }

    Action->ImageColors = ImageColors;
    Action->ImageWidth = ImageWidth;
    Action->ImageHeight = ImageHeight;
    Action->Endpoint = Endpoint;
    Action->X_API_Key = X_API_Key;
    Action->ModelID = ModelID;
    Action->ConfigName = ConfigName;
    Action->Payload = Payload;

    Action->WorldWhereTaskWasCalled = WorldContextObject->GetWorld();
    Action->RegisterWithGameInstance(WorldContextObject);
    return Action;
}

void UIPF_SendImageToServerAsyncTask::Activate()
{
    Super::Activate();

    if (DuplicateCheck())
    {
        LogMessageIfDebugEnabled("Duplicate image. Request canceled.");

        OnFailed.Broadcast(0, "Duplicate image");
        SetReadyToDestroy();
        return;
    }

    auto ImageBytes = GetImageBytesFromColors(ImageColors, ImageWidth, ImageHeight);

    const auto HttpRequest = FHttpModule::Get().CreateRequest();
    const auto InspectorSettings = GetInspectorSettings();
    const FString Boundary = InspectorSettings.Boundary;

    Endpoint.IsEmpty() ? HttpRequest->SetURL(InspectorSettings.Endpoint) : HttpRequest->SetURL(Endpoint);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=" + Boundary));
    HttpRequest->SetHeader(TEXT("X-API-KEY"), X_API_Key.IsEmpty() ? InspectorSettings.X_API_Key : X_API_Key);

    LogHttpRequest(ImageBytes);

    TArray<uint8> Content;

    const FString ModelIDData = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"model_id\"\r\n\r\n%s\r\n"), *Boundary,
        ModelID.IsEmpty() ? *InspectorSettings.ModelID : *ModelID);
    Content.Append(FStringToUint8(ModelIDData));

    const FString ConfigNameData = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"config_name\"\r\n\r\n%s\r\n"), *Boundary,
        ConfigName.IsEmpty() ? *InspectorSettings.ConfigName : *ConfigName);
    Content.Append(FStringToUint8(ConfigNameData));

    const FString PayloadData = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"payload\"\r\n\r\n%s\r\n"), *Boundary,
        Payload.IsEmpty() ? *InspectorSettings.Payload : *Payload);
    Content.Append(FStringToUint8(PayloadData));

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
    if (!Response.IsValid())
    {
        LogMessageIfDebugEnabled("Invalid response from server!", true);

        OnFailed.Broadcast(0, "Invalid response");
        SetReadyToDestroy();
        return;
    }

    const FString Msg = FString::Printf(TEXT("HttpRequest response code: %i, Content: %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    LogMessageIfDebugEnabled(Msg);

    if (!bConnectedSuccessfully || !InspectorSubsystem)
    {
        TaskFailed(Response);
        return;
    }

    InspectorSubsystem->LastImageColors = ImageColors;

    OnSuccess.Broadcast(Response->GetResponseCode(), Response->GetContentAsString());
    SetReadyToDestroy();
}

bool UIPF_SendImageToServerAsyncTask::DuplicateCheck()
{
    if (ImageColors.IsEmpty() || !InspectorSubsystem) return false;

    if (ImageColors.Num() != InspectorSubsystem->LastImageColors.Num()) return false;

    const auto InspectorSettings = GetInspectorSettings();
    const float MaxAllowedThresholdForDuplicatedPixel = InspectorSettings.MaxAllowedThresholdForDuplicatedPixel;
    const int32 TotalPixels = ImageColors.Num();
    int32 MatchingPixels = 0;

    for (int32 Index = 0; Index < ImageColors.Num(); ++Index)
    {
        const FColor& ColorA = ImageColors[Index];
        const FColor& ColorB = InspectorSubsystem->LastImageColors[Index];

        if (FMath::Abs(ColorA.R - ColorB.R) < MaxAllowedThresholdForDuplicatedPixel &&
            FMath::Abs(ColorA.G - ColorB.G) < MaxAllowedThresholdForDuplicatedPixel &&
            FMath::Abs(ColorA.B - ColorB.B) < MaxAllowedThresholdForDuplicatedPixel &&
            FMath::Abs(ColorA.A - ColorB.A) < MaxAllowedThresholdForDuplicatedPixel)
        {
            MatchingPixels++;
        }
    }
    const float Similarity = static_cast<float>(MatchingPixels) / static_cast<float>(TotalPixels);
    return Similarity >= InspectorSettings.DuplicateThresholdPercentage;
}

void UIPF_SendImageToServerAsyncTask::TaskFailed(const TSharedPtr<IHttpResponse>& Response)
{
    OnFailed.Broadcast(Response->GetResponseCode(), Response->GetContentAsString());
    SetReadyToDestroy();
}

void UIPF_SendImageToServerAsyncTask::LogHttpRequest(const TArray<uint8>& ImageBytes)
{
    const auto InspectorSettings = GetInspectorSettings();
    FString DebugMsg = "HttpRequest request details:\n";

    DebugMsg.Append("Endpoint: " + Endpoint.IsEmpty() ? InspectorSettings.Endpoint : Endpoint);
    DebugMsg.Append("Verb: POST");
    DebugMsg.Append("Content-Type: multipart/form-data; boundary=" + InspectorSettings.Boundary);
    DebugMsg.Append("X-API-KEY: " + X_API_Key.IsEmpty() ? InspectorSettings.X_API_Key : X_API_Key);
    DebugMsg.Append("model_id: " + ModelID.IsEmpty() ? *InspectorSettings.ModelID : *ModelID);
    DebugMsg.Append("config_name: " + ConfigName.IsEmpty() ? *InspectorSettings.ConfigName : *ConfigName);
    DebugMsg.Append("payload: " + Payload.IsEmpty() ? *InspectorSettings.Payload : *Payload);
    DebugMsg.Append("image: " + FString::FormatAsNumber(ImageBytes.Num()) + " bytes");

    LogMessageIfDebugEnabled(DebugMsg);
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
