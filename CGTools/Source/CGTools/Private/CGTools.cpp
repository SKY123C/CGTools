// Copyright Epic Games, Inc. All Rights Reserved.

#include "CGTools.h"
#include "CGCommands.h"
#include "ToolMenus.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Brushes/SlateImageBrush.h"
#include "ToolMenuSection.h"
#include "Interfaces/IPluginManager.h"
#include "Widgets/Docking/SDockTab.h"
#define LOCTEXT_NAMESPACE "FCGToolsModule"
IMPLEMENT_MODULE(FCGToolsModule, CGTools)
void FCGToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	RegisterSlateStyle();
	BindGlobalLevelEditorCommands();
}

void FCGToolsModule::ShutdownModule()
{
	//FSlateStyleRegistry::UnRegisterSlateStyle(*Style);
	//ensure(Style.IsUnique());
	//Style.Reset();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}
 
void FCGToolsModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	
	
}

void FCGToolsModule::BindGlobalLevelEditorCommands()
{
	FCGCommands::Register();
	FSequenceCommands::Register();
	SequenceCommandList = MakeShareable(new FUICommandList);
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
	Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	FString IconPath = IPluginManager::Get().FindPlugin(TEXT("CGTools"))->GetBaseDir() / TEXT("Resources");
	Style->SetContentRoot(IconPath);
	if (IFileManager::Get().DirectoryExists(*IconPath))
	{
		FString TempPath = Style->RootToContentDir("update_24", TEXT(".png"));
		UE_LOG(LogTemp, Display, TEXT("%s"), *TempPath);
	}
	FString a = Style->RootToContentDir("main", TEXT(".png"));
	Style->Set("CGToolStyle.MenuIcon", new FSlateImageBrush(Style->RootToContentDir("main", TEXT(".png")), FVector2D(24, 24)));
	Style->Set("CGToolStyle.Unknown", new FSlateImageBrush(Style->RootToContentDir("Unknown", TEXT(".png")), FVector2D(128, 128)));
	Style->Set("CGTool.UpdateTWCommand", new FSlateImageBrush(Style->RootToContentDir("Update_16", TEXT(".png")), FVector2D(16, 16)));

	FSlateStyleRegistry::RegisterSlateStyle(*Style);
}

#undef LOCTEXT_NAMESPACE
	
