// Copyright Epic Games, Inc. All Rights Reserved.

#include "InspectorPlayFiStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FInspectorPlayFiStyle::StyleInstance = nullptr;

void FInspectorPlayFiStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FInspectorPlayFiStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FInspectorPlayFiStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("InspectorPlayFiStyle"));
	return StyleSetName;
}

const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FInspectorPlayFiStyle::Create()
{
    TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("InspectorPlayFiStyle"));
    Style->SetContentRoot(IPluginManager::Get().FindPlugin("InspectorPlayFi")->GetBaseDir() / TEXT("Resources"));

    Style->Set("InspectorPlayFi.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("Icon20"), Icon20x20));

    return Style;
}

void FInspectorPlayFiStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FInspectorPlayFiStyle::Get()
{
	return *StyleInstance;
}
