// Copyright Epic Games, Inc. All Rights Reserved.

#include "InspectorLIFTStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FInspectorLIFTStyle::StyleInstance = nullptr;

void FInspectorLIFTStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FInspectorLIFTStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FInspectorLIFTStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("InspectorLIFTStyle"));
	return StyleSetName;
}

const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FInspectorLIFTStyle::Create()
{
    TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("InspectorLIFTStyle"));
    Style->SetContentRoot(IPluginManager::Get().FindPlugin("InspectorLIFT")->GetBaseDir() / TEXT("Resources"));

    Style->Set("InspectorLIFT.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("Icon20"), Icon20x20));

    return Style;
}

void FInspectorLIFTStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FInspectorLIFTStyle::Get()
{
	return *StyleInstance;
}
