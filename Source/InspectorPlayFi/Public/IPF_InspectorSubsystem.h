// PlayFi data Inspector. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IPF_InspectorSubsystem.generated.h"

UCLASS()
class INSPECTORPLAYFI_API UIPF_InspectorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

protected:
    float ScreenshotTimerRate = 5.f;

protected:
    void OnScreenshotTimerUpdate();

private:
    FTimerHandle ScreenshotTimerHandle;
};
