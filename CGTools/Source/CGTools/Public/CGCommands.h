#pragma once
#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"


class CGTOOLS_API FCGCommands : public TCommands<FCGCommands>
{
public:
	FCGCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenCGToolsCommand;
};

class CGTOOLS_API FSequenceCommands : public TCommands<FSequenceCommands>
{
public:
	FSequenceCommands();
	virtual void RegisterCommands() override;
public:
	TSharedPtr< FUICommandInfo > UpdateTWCommand;
};