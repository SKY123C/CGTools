#include "ToolInterface.h"
#include "Templates/SharedPointer.h"

#define LOCTEXT_NAMESPACE "CGToolsManager"
const TSharedRef<FToolsManager>& FToolsManager::Get()
{
	static const TSharedRef<FToolsManager> Instance = MakeShareable(new FToolsManager());
	// @todo: Never Destroy the Global Tab Manager because it has hooks into a bunch of different modules.
	//        All those modules are unloaded first, so unbinding the delegates will cause a problem.
	static const TSharedRef<FToolsManager>* NeverDestroyGlobalTabManager = new TSharedRef<FToolsManager>(Instance);
	return Instance;
}

#undef LOCTEXT_NAMESPACE