#include "UCheckBasic.h"

FName UCheckBasic::GetDisplayFieldName()
{
	return DisplayFieldName;
}

void UCheckBasic::SetDisplayFieldName(const FName FieldName)
{
	DisplayFieldName = FieldName;
}

UCheckTest::UCheckTest(const FObjectInitializer& ObjectInitializer)
{
	SetDisplayFieldName("CheckTest");
}
