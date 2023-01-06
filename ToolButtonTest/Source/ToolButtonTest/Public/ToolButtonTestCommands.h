// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ToolButtonTestStyle.h"

class FToolButtonTestCommands : public TCommands<FToolButtonTestCommands>
{
public:

	FToolButtonTestCommands()
		: TCommands<FToolButtonTestCommands>(TEXT("ToolButtonTest"), NSLOCTEXT("Contexts", "ToolButtonTest", "ToolButtonTest Plugin"), NAME_None, FToolButtonTestStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
