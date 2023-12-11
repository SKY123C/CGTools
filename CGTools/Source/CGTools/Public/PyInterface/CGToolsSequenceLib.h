// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CineCameraActor.h"
#include "ISequencerModule.h"
#include "CGToolsSequenceLib.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
DECLARE_DYNAMIC_DELEGATE_OneParam(FSequenceToolbarDelegate, ULevelSequence*, Sequence);

UCLASS()
class UCGToolsSequenceLib : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RegisterSequenceToolbar", Keywords = "RegisterSequenceToolbar"), Category = "CGTools")
	static void RegisterSequenceToolbar(const FSequenceToolbarDelegate& ToolbarDelegate);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CGTool GetBoundObjects", Keywords = "CGTool GetBoundObjects"), Category = "CGTools")
	static TArray<UObject*> GetBoundObjects(ULevelSequence* Sequence, const FGuid& Id);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CGTool UpdateSequence", Keywords = "CGTool UpdateSequence"), Category = "CGTools")
	static void UpdateSequence(ULevelSequence* Sequence);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CGTool CheckIsOpen", Keywords = "CGTool CheckIsOpen"), Category = "CGTools")
	static bool CheckIsOpen(ULevelSequence* Sequence);

public:
	static TWeakPtr<ISequencer> SequencerPtr;
	static TWeakPtr<ISequencer> GetSequencer() {
		return SequencerPtr;
	}
	static void Execute(const FSequenceToolbarDelegate& ToolbarDelegate);
};
