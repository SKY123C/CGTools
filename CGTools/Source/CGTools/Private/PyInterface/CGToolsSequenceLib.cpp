// Copyright Epic Games, Inc. All Rights Reserved.

#include "PyInterface/CGToolsSequenceLib.h"
#include "LevelSequence.h"
#include "EditorStyleSet.h"
#include "Misc/Guid.h"
#include "Containers/ArrayView.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "IMovieScenePlayer.h"
#if ENGINE_MAJOR_VERSION <5
#include "CGCommands.h"
#include "CGTools.h"
#else
#include "Styling/AppStyle.h"
#endif
#define LOCTEXT_NAMESPACE "CGToolsSequenceBPLibrary"

TWeakPtr<ISequencer> UCGToolsSequenceLib::SequencerPtr = nullptr;

void UCGToolsSequenceLib::Execute(const FSequenceToolbarDelegate& ToolbarDelegate)
{
	FCGToolsModule& CGToolsModule = FModuleManager::LoadModuleChecked<FCGToolsModule>("CGTools");
	if (SequencerPtr.Pin().IsValid())
	{
		ULevelSequence* Object = Cast<ULevelSequence>(SequencerPtr.Pin()->GetFocusedMovieSceneSequence()->GetMovieScene()->GetOuter());
		if (Object)
		{
			ToolbarDelegate.ExecuteIfBound(Object);
			SequencerPtr.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::RefreshAllImmediately);
		}

	}
}

UCGToolsSequenceLib::UCGToolsSequenceLib(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ISequencerModule& SequencerModule = FModuleManager::GetModuleChecked<ISequencerModule>("Sequencer");
	SequencerModule.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateLambda([](TSharedRef<ISequencer> InSequencer) {
		SequencerPtr = InSequencer;
		}));
}

void UCGToolsSequenceLib::RegisterSequenceToolbar(const FSequenceToolbarDelegate& ToolbarDelegate)
{
	static bool bInit = false;
	static FSequenceToolbarDelegate TempDelegate;
	if (bInit)
		return;
	TempDelegate = ToolbarDelegate;
	ISequencerModule& SequencerModule = FModuleManager::GetModuleChecked<ISequencerModule>("Sequencer");
	FCGToolsModule& CGToolsModule = FModuleManager::LoadModuleChecked<FCGToolsModule>("CGTools");
	const FSequenceCommands& Commands = FSequenceCommands::Get();
	CGToolsModule.GetSequenceCommand()->MapAction(
		FSequenceCommands::Get().UpdateTWCommand,
		FExecuteAction::CreateLambda([&]() {
			UE_LOG(LogTemp, Display, TEXT("UpdateTeamWorkAssets"));
			Execute(TempDelegate);
			})
	);
#if ENGINE_MAJOR_VERSION < 5
	//FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.Save")
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
	MenuExtender->AddToolBarExtension(
		TEXT("Curve Editor"),
		EExtensionHook::After,
		CGToolsModule.GetSequenceCommand(),
		FToolBarExtensionDelegate::CreateLambda([&](FToolBarBuilder& ToolbarBuilder) {
			ToolbarBuilder.AddSeparator();
			ToolbarBuilder.BeginSection("CGTeamWork");
			{
				ToolbarBuilder.AddToolBarButton(
					FSequenceCommands::Get().UpdateTWCommand,
					NAME_None,
					TAttribute<FText>(),
					TAttribute<FText>(),
					FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.RestoreAnimatedState")
				);
			}
			ToolbarBuilder.AddSeparator();
			ToolbarBuilder.EndSection();
			})
	);
	SequencerModule.GetToolBarExtensibilityManager()->AddExtender(MenuExtender);
#else
	FName MenuName = "Sequencer.MainToolBar";
	UToolMenu* ToolMenu = UToolMenus::Get()->ExtendMenu(MenuName);
	const FToolMenuInsert SectionInsertLocation("CurveEditor", EToolMenuInsertType::After);
	{
		if (ToolMenu)
		{
			ToolMenu->AddDynamicSection("CGTeamWorkDynamic", FNewToolMenuDelegate::CreateLambda([&](UToolMenu* InMenu)
				{
					FToolMenuEntry UpdateContextEntry = FToolMenuEntry::InitToolBarButton(
						"CGTeamWorkUpdate",
						FUIAction(FExecuteAction::CreateLambda([&](){Execute(TempDelegate);})),
						LOCTEXT("UpdateButton", "Update"),
						LOCTEXT("UpdateButtonTooltip", "Update Sequence CGTeamWork Asset"),
						FSlateIcon(CGToolsModule.GetStyleSetName(), "CGTool.UpdateTWCommand")
					);
					FToolMenuSection& Section = InMenu->FindOrAddSection("CGTeamWork");
					UpdateContextEntry.StyleNameOverride = "SequencerToolbar";
					Section.AddEntry(UpdateContextEntry);

				}), SectionInsertLocation);
		}
	}
#endif
	bInit = true;
}

TArray<UObject*> UCGToolsSequenceLib::GetBoundObjects(ULevelSequence* Sequence, const FGuid& Id)
{
	TArray<UObject*> Objects;
	FCGToolsModule& CGToolsModule = FModuleManager::LoadModuleChecked<FCGToolsModule>("CGTools");
	if (SequencerPtr.Pin().IsValid())
	{
		
		TArrayView<TWeakObjectPtr<>> BoundObjectArray = SequencerPtr.Pin()->FindObjectsInCurrentSequence(Id);
		for (TWeakObjectPtr<> Ptr : BoundObjectArray)
		{
			Objects.Add(Ptr.Get());
		}
	}
	return Objects;
}

void UCGToolsSequenceLib::UpdateSequence(ULevelSequence* Sequence)
{
	if (SequencerPtr.Pin().IsValid() && CheckIsOpen(Sequence))
	{
		SequencerPtr.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::RefreshAllImmediately);
	}
}

bool UCGToolsSequenceLib::CheckIsOpen(ULevelSequence* Sequence)
{
	bool Result = false;
	if (SequencerPtr.Pin().IsValid())
	{
		ULevelSequence* Object = Cast<ULevelSequence>(SequencerPtr.Pin()->GetFocusedMovieSceneSequence()->GetMovieScene()->GetOuter());
		if (Object && Sequence && Object == Sequence)
		{
			Result = true;
		}
	}
	return Result;
}

#undef LOCTEXT_NAMESPACE