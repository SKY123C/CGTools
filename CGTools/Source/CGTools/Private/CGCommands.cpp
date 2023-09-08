#include "CGCommands.h"
#include "Framework/Commands/UICommandInfo.h"
#define LOCTEXT_NAMESPACE "CGCommands"

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

#undef LOCTEXT_NAMESPACE