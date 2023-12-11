#include "Editor/TreeViewData.h"

FLeafData::FLeafData(AActor* DstActor) : Actor(DstActor)
{
	Label = DstActor->GetActorLabel();
	Type = DstActor->GetClass()->GetName();
}

void FLeafData::AddChildren(TArray<AActor*>& Actors)
{
	TArray<AActor*> ChildrenActors;
	for (auto& Item: Children)
	{
		ChildrenActors.Add(Item->Actor);
	}
	for (AActor* ShownActor : Actors)
	{
		if (ChildrenActors.Num() > 0)
		{
			if (!ChildrenActors.Find(ShownActor))
			{
				TSharedPtr<FLeafData> Item = MakeShareable(new FLeafData(ShownActor));
				Children.Add(Item);
			}
		}
		else
		{
			TSharedPtr<FLeafData> Item = MakeShareable(new FLeafData(ShownActor));
			Children.Add(Item);
		}
	}

}
