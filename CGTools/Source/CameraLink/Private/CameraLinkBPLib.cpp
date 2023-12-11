#include "CameraLinkBPLib.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequenceActor.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "CineCameraComponent.h"
#include "CineCameraActor.h"
#include "ActorEditorUtils.h"
#include "CameraLink.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/TextureRenderTargetFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "CameraLinkTransfromClient.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Materials/Material.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialExpressionIf.h"
#include "Materials/MaterialExpressionTextureSample.h"
#if ENGINE_MAJOR_VERSION >=5
#include "Subsystems/EditorActorSubsystem.h"
#endif
#include "Editor.h"



UTexture2D* UCameraLinkBPLib::CreateTexture2DFromRaw(TArray<uint8> RawData, int32 Width, int32 Height, int32 ChannelNum, bool bUseSRGB, int32 TextureFilterValue, bool bBGR, bool bFlipY)
{

	// Decompress PNG image

	if (Width <= 0 || Height <= 0)
		return nullptr;
	if (RawData.Num() != Width * Height * ChannelNum)
	{
		UE_LOG(LogTemp, Warning, TEXT("RawData.Num(%d) != Width(%d) * Height(%d) * ChannelNum(%d)"), RawData.Num(), Width, Height, ChannelNum);
		return nullptr;
	}


	// Fill in the base mip for the texture we created
	UTexture2D* NewTexture2D = UTexture2D::CreateTransient(Width, Height, EPixelFormat::PF_B8G8R8A8);

	//NewTexture2D->Source.Init(Width, Height, 1, 1, ETextureSourceFormat::TSF_BGRA8, nullptr);
	//NewTexture2D->UpdateResource();
	// Fill in the base mip for the texture we created
#if ENGINE_MAJOR_VERSION == 5
	uint8* MipData = NewTexture2D->Source.LockMip(0);
#else
	uint8* MipData = (uint8*)NewTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
#endif
	for (int32 y = 0; y < Height; y++)
	{
		uint8* DestPtr = bFlipY ? &MipData[y * Width * sizeof(FColor)] : &MipData[(Height - 1 - y) * Width * sizeof(FColor)];
		//const FColor* SrcPtr = &((FColor*)(RawData.GetData()))[(Height - 1 - y) * Width];

		for (int32 x = 0; x < Width; x++)
		{
			int32 Index = (x + Width * y) * ChannelNum;
			uint8 r = RawData[Index];
			uint8 g = 0, b = 0, a = 0;
			if (ChannelNum == 1)
			{
				g = b = r;
				a = 255;
			}
			else if (ChannelNum == 2)
			{
				g = b = r;
				a = RawData[Index + 1];
			}
			else if (ChannelNum == 3)
			{
				g = RawData[Index + 1];
				b = RawData[Index + 2];
				a = 255;
			}
			else if (ChannelNum == 4) {
				g = RawData[Index + 1];
				b = RawData[Index + 2];
				a = RawData[Index + 3];
			}
			*DestPtr++ = bBGR ? r : b;
			*DestPtr++ = g;
			*DestPtr++ = bBGR ? b : r;
			*DestPtr++ = a;

		}
	}
#if ENGINE_MAJOR_VERSION == 5
	NewTexture2D->Source.UnlockMip(0);
#else
	NewTexture2D->PlatformData->Mips[0].BulkData.Unlock();
#endif

	// Set options
	NewTexture2D->SRGB = bUseSRGB;
	NewTexture2D->CompressionNone = true;
	NewTexture2D->MipGenSettings = TMGS_NoMipmaps;
	NewTexture2D->CompressionSettings = TC_Default;
	NewTexture2D->AddressX = TextureAddress::TA_Clamp;
	NewTexture2D->AddressY = TextureAddress::TA_Clamp;
	NewTexture2D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

	NewTexture2D->Filter = TF_Default;
	if (0 <= TextureFilterValue && TextureFilterValue < TF_MAX) {
		NewTexture2D->Filter = (TextureFilter)(TextureFilterValue);
	}


	// Update the remote texture data
	NewTexture2D->UpdateResource();
	NewTexture2D->PostEditChange();
	//FTextureCompilingManager::Get().FinishCompilation({ NewTexture2D });



	return NewTexture2D;

}

void UCameraLinkBPLib::OpenCameraLinkEditor()
{
	FCameraLinkModule& CameraLinkModule = FModuleManager::LoadModuleChecked<FCameraLinkModule>("CameraLink");
	CameraLinkModule.OpenEditor();
}

void UCameraLinkBPLib::CopyTextureToRenderTargetTexture(UTexture* SourceTexture, UTextureRenderTarget2D* RenderTargetTexture, ERHIFeatureLevel::Type FeatureLevel)
{
	//TexturePaintToolset.h

	check(SourceTexture != nullptr);
	check(RenderTargetTexture != nullptr);

	// Grab the actual render target resource from the texture.  Note that we're absolutely NOT ALLOWED to
	// dereference this pointer.  We're just passing it along to other functions that will use it on the render
	// thread.  The only thing we're allowed to do is check to see if it's nullptr or not.
	FTextureRenderTargetResource* RenderTargetResource = RenderTargetTexture->GameThread_GetRenderTargetResource();
	check(RenderTargetResource != nullptr);

	// Create a canvas for the render target and clear it to black
#if ENGINE_MAJOR_VERSION == 5
	FCanvas Canvas(RenderTargetResource, nullptr, FGameTime(), FeatureLevel);
#else
	FCanvas Canvas(RenderTargetResource, nullptr, 0, 0, 0, FeatureLevel);
#endif

	const uint32 Width = RenderTargetTexture->GetSurfaceWidth();
	const uint32 Height = RenderTargetTexture->GetSurfaceHeight();

	// @todo MeshPaint: Need full color/alpha writes enabled to get alpha
	// @todo MeshPaint: Texels need to line up perfectly to avoid bilinear artifacts
	// @todo MeshPaint: Potential gamma issues here
	// @todo MeshPaint: Probably using CLAMP address mode when reading from source (if texels line up, shouldn't matter though.)

	// @todo MeshPaint: Should use scratch texture built from original source art (when possible!)
	//		-> Current method will have compression artifacts!

	// Grab the texture resource.  We only support 2D textures and render target textures here.
	FTexture* TextureResource = nullptr;
	UTexture2D* Texture2D = Cast<UTexture2D>(SourceTexture);


	if (Texture2D != nullptr)
	{
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 27
		TextureResource = Texture2D->Resource;
#else
		TextureResource = Texture2D->GetResource();
#endif
	}
	else
	{
		UTextureRenderTarget2D* TextureRenderTarget2D = Cast<UTextureRenderTarget2D>(SourceTexture);
		TextureResource = TextureRenderTarget2D->GameThread_GetRenderTargetResource();
	}
	check(TextureResource != nullptr);



	// Draw a quad to copy the texture over to the render target
	{
		const float MinU = 0.0f;
		const float MinV = 0.0f;
		const float MaxU = 1.0f;
		const float MaxV = 1.0f;
		const float MinX = 0.0f;
		const float MinY = 0.0f;
		const float MaxX = Width;
		const float MaxY = Height;

		FCanvasUVTri Tri1;
		FCanvasUVTri Tri2;
		Tri1.V0_Pos = FVector2D(MinX, MinY);
		Tri1.V0_UV = FVector2D(MinU, MinV);
		Tri1.V1_Pos = FVector2D(MaxX, MinY);
		Tri1.V1_UV = FVector2D(MaxU, MinV);
		Tri1.V2_Pos = FVector2D(MaxX, MaxY);
		Tri1.V2_UV = FVector2D(MaxU, MaxV);

		Tri2.V0_Pos = FVector2D(MaxX, MaxY);
		Tri2.V0_UV = FVector2D(MaxU, MaxV);
		Tri2.V1_Pos = FVector2D(MinX, MaxY);
		Tri2.V1_UV = FVector2D(MinU, MaxV);
		Tri2.V2_Pos = FVector2D(MinX, MinY);
		Tri2.V2_UV = FVector2D(MinU, MinV);
		Tri1.V0_Color = Tri1.V1_Color = Tri1.V2_Color = Tri2.V0_Color = Tri2.V1_Color = Tri2.V2_Color = FLinearColor::White;
		TArray< FCanvasUVTri > List;
		List.Add(Tri1);
		List.Add(Tri2);
		FCanvasTriangleItem TriItem(List, TextureResource);
		TriItem.BlendMode = SE_BLEND_Opaque;
		Canvas.DrawItem(TriItem);
	}

	// Tell the rendering thread to draw any remaining batched elements
	Canvas.Flush_GameThread(true);

	ENQUEUE_RENDER_COMMAND(UpdateMeshPaintRTCommand)(
		[RenderTargetResource](FRHICommandListImmediate& RHICmdList)
		{
			// Copy (resolve) the rendered image from the frame buffer to its render target texture
			RHICmdList.CopyToResolveTarget(
				RenderTargetResource->GetRenderTargetTexture(),		// Source texture
				RenderTargetResource->TextureRHI,					// Dest texture
				FResolveParams());									// Resolve parameters
		});
}

void UCameraLinkBPLib::SetRenderTargetData(UTextureRenderTarget2D* RenderTargetTexture, TArray<uint8> RawData, int32 Width, int32 Height, int32 ChannelNum, bool bUseSRGB, int32 TextureFilterValue, bool bBGR, bool bFlipY)
{
	UTexture2D* NewTexture = CreateTexture2DFromRaw(RawData, Width, Height, ChannelNum, bUseSRGB, TextureFilterValue, bBGR, true);



	if (NewTexture != nullptr && RenderTargetTexture != nullptr) {
		CopyTextureToRenderTargetTexture(NewTexture, RenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
	}
	else {

	}
}

void UCameraLinkBPLib::StartClient()
{
	FCameraLinkModule& CameraLinkModule = FModuleManager::LoadModuleChecked<FCameraLinkModule>("CameraLink");
	CameraLinkModule.GetCameraLinker()->StartClient();
}