// Copyright Epic Games, Inc. All Rights Reserved.

#include "InspectorPlayFi.h"
#include "AssetViewUtils.h"
#include "EditorAssetLibrary.h"
#include "InspectorPlayFiStyle.h"
#include "InspectorPlayFiCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName InspectorPlayFiTabName("InspectorPlayFi");

#define LOCTEXT_NAMESPACE "FInspectorPlayFiModule"

void FInspectorPlayFiModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    FInspectorPlayFiStyle::Initialize();
    FInspectorPlayFiStyle::ReloadTextures();

    FInspectorPlayFiCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(FInspectorPlayFiCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FInspectorPlayFiModule::PluginButtonClicked), FCanExecuteAction());

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FInspectorPlayFiModule::RegisterMenus));
}

void FInspectorPlayFiModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FInspectorPlayFiStyle::Shutdown();
    FInspectorPlayFiCommands::Unregister();
}

void FInspectorPlayFiModule::PluginButtonClicked()
{
    UObject* SettingsBlueprint = UEditorAssetLibrary::LoadAsset(FString(TEXT("/InspectorPlayFi/Settings/Settings_InspectorPlayFi")));
    if (!SettingsBlueprint) return;

    AssetViewUtils::OpenEditorForAsset(SettingsBlueprint);
}

void FInspectorPlayFiModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
            Section.AddMenuEntryWithCommandList(FInspectorPlayFiCommands::Get().OpenPluginWindow, PluginCommands);
        }
    }

    {
        UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
        {
            FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
            {
                FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FInspectorPlayFiCommands::Get().OpenPluginWindow));
                Entry.SetCommandList(PluginCommands);
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInspectorPlayFiModule, InspectorPlayFi)
