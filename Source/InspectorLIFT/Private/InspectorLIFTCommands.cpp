// Copyright Epic Games, Inc. All Rights Reserved.

#include "InspectorLIFTCommands.h"

#define LOCTEXT_NAMESPACE "FInspectorLIFTModule"

void FInspectorLIFTCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "InspectorLIFT", "Bring up LIFT Inspector settings", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
