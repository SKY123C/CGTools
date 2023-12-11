#pragma once

class AActor;

struct FLeafData : TSharedFromThis<FLeafData>
{
public:
	FLeafData(AActor* DstActor);
	FString Label;
	AActor* Actor = nullptr;
	FString Type;
	TArray< TSharedPtr<FLeafData> > Children;
	void AddChildren(TArray<AActor*>& Actors);
	AActor* GetActor() {
		return Actor;
	}
};

