// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "InspectorPlayFiStyle.h"

class FInspectorPlayFiCommands : public TCommands<FInspectorPlayFiCommands>
{
public:

	FInspectorPlayFiCommands()
		: TCommands<FInspectorPlayFiCommands>(TEXT("InspectorPlayFi"), NSLOCTEXT("Contexts", "InspectorPlayFi", "InspectorPlayFi Plugin"), NAME_None, FInspectorPlayFiStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};
