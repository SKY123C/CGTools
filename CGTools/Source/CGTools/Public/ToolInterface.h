#pragma once
#include "Templates/SharedPointer.h"
#include "ToolsBase.h"

class FToolsManager: public TSharedFromThis<FToolsManager>
{

public:
	static const TSharedRef<FToolsManager>& Get();
	void RegisterTools(UToolsBase* Tool) {
		ToolArray.Add(Tool);
	};

private:
	TArray<UToolsBase*> ToolArray;
};
