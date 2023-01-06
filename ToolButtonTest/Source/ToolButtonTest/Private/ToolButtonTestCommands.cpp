// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToolButtonTestCommands.h"

#define LOCTEXT_NAMESPACE "FToolButtonTestModule"

void FToolButtonTestCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ToolButtonTest", "Execute ToolButtonTest action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
