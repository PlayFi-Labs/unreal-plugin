// PlayFi data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Settings/IPF_SettingsContainer.h"
#include "IPF_BaseAsyncTask.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogInspectorAsyncTask, All, All)

class UIPF_InspectorSubsystem;

UCLASS()
class INSPECTORPLAYFIRUNTIME_API UIPF_BaseAsyncTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    virtual void Activate() override;

protected:
    UPROPERTY()
    UWorld* WorldWhereTaskWasCalled;

    UPROPERTY()
    UIPF_InspectorSubsystem* InspectorSubsystem;

protected:
    FInspectorSettings GetInspectorSettings() const;
    void LogMessageIfDebugEnabled(const FString& Message, bool bError = false);
    
    static TArray<uint8> GetImageBytesFromColors(const TArray<FColor>& FromColors, const int32& Width, const int32& Height);
};
