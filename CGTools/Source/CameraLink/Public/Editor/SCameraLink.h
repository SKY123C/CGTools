#pragma once
#include "Widgets/SCompoundWidget.h"
#include "Editor/TreeViewData.h"
#include "Widgets/Views/STreeView.h"
#include "Editor/CameraLinker.h"

class ACineCameraActor;

class CAMERALINK_API SCameraLink : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCameraLink)
	{ }
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FCameraLinker> CameraLinkerPtr);
	void AddShownObjects();
	void AddSourceObjects();
	TSharedPtr<SWidget> TreeView_OnContextMenuOpening();
	TSharedPtr<STreeView< TSharedPtr<FLeafData> >> TreeView;

	void GetChildrenForTree(TSharedPtr<FLeafData> InItem, TArray<TSharedPtr<FLeafData>>& OutChildren);
	TSharedRef<ITableRow> OnGenerateRowTreeView(TSharedPtr<FLeafData> RowData, const TSharedRef<STableViewBase>& Table);
	TArray<TSharedPtr<FLeafData>> Items;
	TSharedRef<SWidget> MakeToolBar();

	//interface
	TWeakPtr<FCameraLinker> CameraLinkerPtr;

	FSlateBrush CameraLinkBrush;
	
	void ClearTree();
	void StartService();
	void CloseService();
	void InitSetting();

public:
	TAttribute < FSlateIcon > ServiceBtnIcon;
};