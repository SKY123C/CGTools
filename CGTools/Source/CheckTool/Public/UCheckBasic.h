#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HAL/Runnable.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderingThread.h"
#include "HighResScreenshot.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "UCheckBasic.generated.h"

UCLASS(abstract, BlueprintType)
class CHECKTOOL_API UCheckBasic : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	FName GetDisplayFieldName();

	UFUNCTION(BlueprintCallable)
	void SetDisplayFieldName(const FName FieldName);

	UPROPERTY(BlueprintReadWrite, BlueprintGetter=GetDisplayFieldName, BlueprintSetter=SetDisplayFieldName)
	FName DisplayFieldName = "";

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Operation();
	virtual bool Operation_Implementation(){return true;}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Repair();
	virtual bool Repair_Implementation() { return true; }
};

UCLASS(BlueprintType)
class CHECKTOOL_API UCheckTest : public UCheckBasic
{
	GENERATED_UCLASS_BODY()

	virtual bool Operation_Implementation(){
		UE_LOG(LogTemp, Display, TEXT("####CheckTest###"));
		return true;
	}

	
	
};