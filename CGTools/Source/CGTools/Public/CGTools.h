// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Misc/MessageDialog.h"
#include "ISequencer.h"

class FCGToolsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


public:
	TSharedPtr<FUICommandList> PluginCommandList;
	FDelegateHandle OnSequencerCreatedHandle;

public:
	void OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
	void CommandAAction(){};
	FName GetStyleSetName(){return FName("CGToolStyle");};

private:
	void BindGlobalLevelEditorCommands();
	void RegisterSlateStyle();
	TSharedPtr<class FUICommandList> SequenceCommandList;

public:
	TSharedPtr<FUICommandList> GetSequenceCommand()
	{
		return SequenceCommandList;
	};
private:
	TSharedPtr<class FSlateStyleSet> Style;

};
