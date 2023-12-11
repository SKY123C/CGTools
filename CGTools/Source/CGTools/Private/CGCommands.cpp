#include "CGCommands.h"
#include "Framework/Commands/UICommandInfo.h"
#define LOCTEXT_NAMESPACE "FCGToolsModule"

FCGCommands::FCGCommands() : TCommands<FCGCommands>(
	"FCGCommands",
	NSLOCTEXT("Contexts", "FCGCommands", "CGTools Plugin"),
	NAME_None,
	FName(*FString("todo")))
{
}


void FCGCommands::RegisterCommands()
{
	UI_COMMAND(OpenCGToolsCommand, "OpenCGToolsCommand", "CGTools", EUserInterfaceActionType::Button, FInputChord());
}

FSequenceCommands::FSequenceCommands()
	: TCommands<FSequenceCommands>(
		TEXT("SequenceCommands"),
		NSLOCTEXT("Contexts", "SequenceCommands", "SequenceCommands Test"),
		NAME_None,
		FName("CGToolStyle"))
{

};

void FSequenceCommands::RegisterCommands()
{
	UI_COMMAND(UpdateTWCommand, "UpdateTWCommand", "Update Sequence CGTeamWork Asset", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
