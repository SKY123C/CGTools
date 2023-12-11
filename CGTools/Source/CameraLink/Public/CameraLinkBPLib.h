#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Blueprint.h"
#include "CameraLinkBPLib.generated.h"



UCLASS()
class UCameraLinkBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, meta = (Keywords = "CameraLink Editor"), Category = "CameraLinkEditor")
	static void SetRenderTargetData(UTextureRenderTarget2D* RenderTargetTexture, TArray<uint8> RawData, int32 RawDataWidth, int32 RawDataHeight, int32 RawDataChannelNum
			, bool bUseSRGB = false, int32 TextureFilterValue = -1, bool bBGR = false, bool bFlipY = true);

	static void CopyTextureToRenderTargetTexture(UTexture* SourceTexture, UTextureRenderTarget2D* RenderTargetTexture, ERHIFeatureLevel::Type FeatureLevel);
	static UTexture2D* CreateTexture2DFromRaw(TArray<uint8> RawData, int32 Width, int32 Height, int32 ChannelNum, bool bUseSRGB = false, int32 TextureFilterValue = -1, bool bBGR = false, bool FlipY = false);

	UFUNCTION(BlueprintCallable, meta = (Keywords = "CameraLink Editor"), Category = "CameraLinkEditor")
	static void OpenCameraLinkEditor();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "CameraLink Editor"), Category = "CameraLinkEditor")
	static void StartClient();
};
