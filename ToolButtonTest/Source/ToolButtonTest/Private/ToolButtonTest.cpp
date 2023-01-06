// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToolButtonTest.h"
#include "ToolButtonTestStyle.h"
#include "ToolButtonTestCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName ToolButtonTestTabName("ToolButtonTest");

#define LOCTEXT_NAMESPACE "FToolButtonTestModule"

void FToolButtonTestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FToolButtonTestStyle::Initialize();
	FToolButtonTestStyle::ReloadTextures();

	FToolButtonTestCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FToolButtonTestCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FToolButtonTestModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FToolButtonTestModule::RegisterMenus));
}

void FToolButtonTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FToolButtonTestStyle::Shutdown();

	FToolButtonTestCommands::Unregister();
}

void FToolButtonTestModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FToolButtonTestModule::PluginButtonClicked()")),
							FText::FromString(TEXT("ToolButtonTest.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FToolButtonTestModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FToolButtonTestCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FToolButtonTestCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FToolButtonTestModule, ToolButtonTest)