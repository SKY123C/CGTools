// Copyright Epic Games, Inc. All Rights Reserved.

#include "CGTools.h"
#include "CGCommands.h"
#include "ToolMenus.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Brushes/SlateImageBrush.h"
#include "ToolMenuSection.h"
#include "Interfaces/IPluginManager.h"
#define LOCTEXT_NAMESPACE "FCGToolsModule"

void FCGToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	BindGlobalLevelEditorCommands();
	RegisterSlateStyle();
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	{
		FToolMenuSection& ToolSection = Menu->FindOrAddSection("CGTools");
		FToolMenuEntry CGToolEntry = FToolMenuEntry::InitToolBarButton(
			FCGCommands::Get().OpenCGToolsCommand,
			TAttribute<FText>(),
			TAttribute<FText>(),
			FSlateIcon(TEXT("CGToolStyle"), "CGToolStyle.MenuIcon"),
			NAME_None,
			FName("SaveAllLevels")
		);
		CGToolEntry.SetCommandList(PluginCommandList);

		ToolSection.AddEntry(CGToolEntry);
	}
}

void FCGToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

void FCGToolsModule::BindGlobalLevelEditorCommands()
{
	FCGCommands::Register();
	const FCGCommands& Commands = FCGCommands::Get();
	PluginCommandList = MakeShareable(new FUICommandList);
	PluginCommandList->MapAction(
		Commands.OpenCGToolsCommand,
		FExecuteAction::CreateRaw(this, &FCGToolsModule::CommandAAction),
		FCanExecuteAction()
	);
}

void FCGToolsModule::RegisterSlateStyle()
{
	Style = MakeShareable(new FSlateStyleSet("CGToolStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("CGTools"))->GetBaseDir() / TEXT("Resources"));
	FString a = Style->RootToContentDir("main", TEXT(".png"));
	Style->Set("CGToolStyle.MenuIcon", new FSlateImageBrush(Style->RootToContentDir("main", TEXT(".png")), FVector2D(128, 128)));


	FSlateStyleRegistry::RegisterSlateStyle(*Style);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCGToolsModule, CGTools)