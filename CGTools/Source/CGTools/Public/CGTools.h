// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Misc/MessageDialog.h"

class FCGToolsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


public:
	TSharedPtr<class FUICommandList> PluginCommandList;


public:
	void CommandAAction()
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Execute CommandA"));
	}
private:
	void BindGlobalLevelEditorCommands();
	void RegisterSlateStyle();

private:
	TSharedPtr<class FSlateStyleSet> Style;

};
