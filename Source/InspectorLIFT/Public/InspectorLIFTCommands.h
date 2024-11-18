// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "InspectorLIFTStyle.h"

class FInspectorLIFTCommands : public TCommands<FInspectorLIFTCommands>
{
public:

	FInspectorLIFTCommands()
		: TCommands<FInspectorLIFTCommands>(TEXT("InspectorLIFT"), NSLOCTEXT("Contexts", "InspectorLIFT", "InspectorLIFT Plugin"), NAME_None, FInspectorLIFTStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};
