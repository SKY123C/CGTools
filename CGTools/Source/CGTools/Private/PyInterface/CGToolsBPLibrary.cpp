// Copyright Epic Games, Inc. All Rights Reserved.

#include "PyInterface/CGToolsBPLibrary.h"
#include "Misc/Parse.h"
#include "CGTools.h"

#define LOCTEXT_NAMESPACE "CGToolBPLirary"

void UCGToolsBPLibrary::CGToolsSampleFunction(FString Param)
{
	uint32 test = FParse::HexNumber(*Param);
	int a = 5;
}

int UCGToolsBPLibrary::test(const FString& Param)
{
	uint32 test = FParse::HexNumber(*Param);
	FString::ToHexBlob()
	return a*5;
}

#undef LOCTEXT_NAMESPACE