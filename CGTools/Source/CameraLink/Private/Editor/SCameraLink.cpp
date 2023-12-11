#include "Editor/SCameraLink.h"
#include "Editor/TreeViewData.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Selection.h"
#include "CineCameraActor.h"
#include "Widgets/Layout//SWrapBox.h"
#include "CineCameraActor.h"
#include "Widgets/Images/SImage.h"
#define LOCTEXT_NAMESPACE "SCameraLink"


void SCameraLink::Construct(const FArguments& InArgs, TSharedRef<FCameraLinker> Ptr)
{
	CameraLinkerPtr = Ptr;
	TreeView = SNew(STreeView< TSharedPtr<FLeafData> >)
		.ItemHeight(50)
		.OnContextMenuOpening(this, &SCameraLink::TreeView_OnContextMenuOpening)
		.SelectionMode(ESelectionMode::Single)
		.OnGetChildren(this, &SCameraLink::GetChildrenForTree)
		//.OnMouseButtonClick(this, &SCameraLink::SelectActor)
		.OnGenerateRow(this, &SCameraLink::OnGenerateRowTreeView)
		.TreeItemsSource(&Items)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column("Label").DefaultLabel(FText::FromString(TEXT("Label")))
		);
	ChildSlot[
		SNew(SSplitter)
			.Orientation(EOrientation::Orient_Horizontal)
			+ SSplitter::Slot()
			.Value(0.5f)
			[
				SNew(SImage)
				.Image(&CameraLinkBrush)
			]
			+ SSplitter::Slot()
			.Value(0.5f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
					[
						MakeToolBar()
					]
				]
			+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					TreeView.ToSharedRef()
				]
			]
	];
}

void SCameraLink::AddShownObjects()
{
	TArray<AActor*> SelectedActors;
	for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		if (Actor->GetClass() != ACineCameraActor::StaticClass())
			SelectedActors.Add(Actor);

	}
	for (auto Item : Items)
	{
		if (Item->GetActor()->GetClass() == ACineCameraActor::StaticClass())
		{
			Item->AddChildren(SelectedActors);
			break;
		}
	}
	TreeView->RequestTreeRefresh();
	CameraLinkerPtr.Pin()->AddShownActors(SelectedActors);

}

void SCameraLink::AddSourceObjects()
{
	ACineCameraActor* Camera = nullptr;
	TArray<AActor*> ShownActors;
	for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter)
	{	
		AActor* Actor = Cast<AActor>(*Iter);
		if (!Camera)
		{
			if (Actor->GetClass() == ACineCameraActor::StaticClass())
				Camera = Cast<ACineCameraActor>(Actor);
		}
		else
		{
			ShownActors.Add(Actor);
		}
	}
	if (Camera)
	{
		Items.Empty();
		TreeView->RequestTreeRefresh();
		TSharedPtr<FLeafData> Item = MakeShareable(new FLeafData(Camera));
		if (ShownActors.Num() > 0)
		{
			Item->AddChildren(ShownActors);
		}
		Items.Add(Item);
		TreeView->RequestTreeRefresh();
		StartService();
	}

}

TSharedPtr<SWidget> SCameraLink::TreeView_OnContextMenuOpening()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	if (TreeView->GetNumItemsSelected() <= 0)
	{
		MenuBuilder.AddMenuEntry(
			FText::FromString("Add Source Objects"),
			FText::FromString("Camera is mandatory, Actor is optional"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SCameraLink::AddSourceObjects),
				FCanExecuteAction::CreateLambda([this] {return GEditor->GetSelectedActors()->Num() > 0;})
			)
		);
	}
	else {
		MenuBuilder.AddMenuEntry(
			FText::FromString("Add Shown Objects"),
			FText::FromString("Control the display of objects"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SCameraLink::AddShownObjects),
				FCanExecuteAction::CreateLambda([this] {return GEditor->GetSelectedActors()->Num() > 0; })
			)
		);
	}

	return MenuBuilder.MakeWidget();
}

void SCameraLink::GetChildrenForTree(TSharedPtr<FLeafData> InItem, TArray<TSharedPtr<FLeafData>>& OutChildren)
{
	OutChildren.Append(InItem->Children);
}

TSharedRef<ITableRow> SCameraLink::OnGenerateRowTreeView(TSharedPtr<FLeafData> RowData, const TSharedRef<STableViewBase>& Table)
{
	
	return SNew(STableRow<TSharedPtr<FLeafData>>, Table)
		.Content()
		[
			SNew(STextBlock)
			.Text(FText::FromString(RowData->Label))

		];
}

TSharedRef<SWidget> SCameraLink::MakeToolBar()
{
	//ServiceBtnIcon.Bind(TAttribute<FSlateIcon>::FGetter::CreateLambda([&] {
	//	if (CameraLinkerPtr.Pin()->bHasConnectState)
	//	{
	//		return FSlateIcon(TEXT("CameraLinkSlateStyle"), "CameraLinkSlateStyle.CloseServiceIcon");
	//	}
	//	else
	//	{
	//		return FSlateIcon(TEXT("CameraLinkSlateStyle"), "CameraLinkSlateStyle.StartServiceIcon");
	//	}
	//	
	//	}
	//)
	//);
	FToolBarBuilder ToolBarBuilder(TSharedPtr<FUICommandList>(), FMultiBoxCustomization::None);
	ToolBarBuilder.BeginSection(TEXT("Edit"));
	{
		ToolBarBuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateRaw(this, &SCameraLink::CloseService)
			),
			NAME_None,
			FText::FromString(" Close"),
			FText::FromString("Close"),
			FSlateIcon(TEXT("CameraLinkSlateStyle"), "CameraLinkSlateStyle.CloseServiceIcon"),
			EUserInterfaceActionType::ToggleButton
			);

	};
	ToolBarBuilder.EndSection();
	ToolBarBuilder.BeginSection(TEXT("Setting"));
	{
		ToolBarBuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateRaw(this, &SCameraLink::InitSetting)
			),
			NAME_None,
			FText::FromString(" Setting"),
			FText::FromString("CameraLink Setting"),
			FSlateIcon(TEXT("CameraLinkSlateStyle"), "CameraLinkSlateStyle.SettingIcon_32"),
			EUserInterfaceActionType::ToggleButton
		);

	};
	ToolBarBuilder.EndSection();
	return ToolBarBuilder.MakeWidget();
}

void SCameraLink::StartService()
{
	for (auto Item : Items)
	{
		if (Item->Actor->GetClass() == ACineCameraActor::StaticClass())
		{
			TArray<AActor*> Actors;
			for (auto child : Item->Children)
			{
				Actors.Add(child->Actor);
			}
			ACineCameraActor* CameraActor = Cast<ACineCameraActor>(Item->Actor);
			CameraLinkerPtr.Pin()->StartService(CameraActor, Actors, 0, 0);
			CameraLinkBrush.SetResourceObject(CameraLinkerPtr.Pin()->GetMaterialBrush());
			break;
		}
	}
}

void SCameraLink::CloseService()
{
	CameraLinkerPtr.Pin()->CloseService();
	ClearTree();
}
void SCameraLink::ClearTree()
{
	Items.Empty();
	TreeView->RequestTreeRefresh();
}

void SCameraLink::InitSetting()
{

}


#undef LOCTEXT_NAMESPACE