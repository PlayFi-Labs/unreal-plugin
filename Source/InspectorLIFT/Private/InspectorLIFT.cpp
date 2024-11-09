// Copyright Epic Games, Inc. All Rights Reserved.

#include "InspectorLIFT.h"
#include "AssetViewUtils.h"
#include "EditorAssetLibrary.h"
#include "InspectorLIFTStyle.h"
#include "InspectorLIFTCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName InspectorLIFTTabName("InspectorLIFT");

#define LOCTEXT_NAMESPACE "FInspectorLIFTModule"

void FInspectorLIFTModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    FInspectorLIFTStyle::Initialize();
    FInspectorLIFTStyle::ReloadTextures();

    FInspectorLIFTCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(FInspectorLIFTCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FInspectorLIFTModule::PluginButtonClicked), FCanExecuteAction());

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FInspectorLIFTModule::RegisterMenus));
}

void FInspectorLIFTModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FInspectorLIFTStyle::Shutdown();
    FInspectorLIFTCommands::Unregister();
}

void FInspectorLIFTModule::PluginButtonClicked()
{
    UObject* SettingsBlueprint = UEditorAssetLibrary::LoadAsset(FString(TEXT("/InspectorLIFT/Settings/Settings_InspectorLIFT")));
    if (!SettingsBlueprint) return;

    AssetViewUtils::OpenEditorForAsset(SettingsBlueprint);
}

void FInspectorLIFTModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
            Section.AddMenuEntryWithCommandList(FInspectorLIFTCommands::Get().OpenPluginWindow, PluginCommands);
        }
    }

    {
        UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
        {
            FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
            {
                FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FInspectorLIFTCommands::Get().OpenPluginWindow));
                Entry.SetCommandList(PluginCommands);
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInspectorLIFTModule, InspectorLIFT)
