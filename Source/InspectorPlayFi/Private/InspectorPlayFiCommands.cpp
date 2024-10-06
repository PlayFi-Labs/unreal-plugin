// Copyright Epic Games, Inc. All Rights Reserved.

#include "InspectorPlayFiCommands.h"

#define LOCTEXT_NAMESPACE "FInspectorPlayFiModule"

void FInspectorPlayFiCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "InspectorPlayFi", "Bring up PlayFi Inspector settings", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
