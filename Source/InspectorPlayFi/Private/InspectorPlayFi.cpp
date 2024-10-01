// Copyright Epic Games, Inc. All Rights Reserved.

#include "InspectorPlayFi.h"
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

    FGlobalTabmanager::Get()
        ->RegisterNomadTabSpawner(InspectorPlayFiTabName, FOnSpawnTab::CreateRaw(this, &FInspectorPlayFiModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("FOutfitBridgeEditorTabTitle", "Outfit Bridge"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
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
    // Put your "OnButtonClicked" stuff here
    FText DialogText = FText::Format(
        LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
        FText::FromString(TEXT("FInspectorPlayFiModule::PluginButtonClicked()")),
        FText::FromString(TEXT("InspectorPlayFi.cpp"))
        );
    FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

TSharedRef<SDockTab> FInspectorPlayFiModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab);
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
