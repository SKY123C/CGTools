// Copyright Epic Games, Inc. All Rights Reserved.

#include "PyInterface/CGToolsBPLibrary.h"

#define LOCTEXT_NAMESPACE "CGToolBPLirary"

void UCGToolsBPLibrary::CGToolsSampleFunction(FString Param)
{
	int a = 5;
}

//int UCGToolsBPLibrary::test()
//{
//	//for (TObjectIterator<UCheckBasic> It; It; ++It)
//	//{
//	//	UCheckBasic* MyObject = *It;
//	//	UE_LOG(LogTemp, Display, TEXT("Found UObject named:%s"), *(MyObject->DisplayFieldName.ToString()));
//	//	MyObject->Operation();
//	//}
//	ISequencerModule& Sequencer = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
//	Sequencer.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateLambda([](TSharedRef<ISequencer> Sequence) {
//		const FName MenuName("Sequencer.MainToolBar");
//		UToolMenu* ToolMenu = UToolMenus::Get()->ExtendMenu(MenuName);
//		const FToolMenuInsert SectionInsertLocation("CurveEditor", EToolMenuInsertType::After);
//		{
//			if (ToolMenu)
//			{
//				ToolMenu->AddDynamicSection("LevelSequenceEditorDynamic", FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu)
//					{
//						FToolMenuEntry UpdateContextEntry = FToolMenuEntry::InitToolBarButton(
//							"UpdateTeamWorkAssets",
//							FUIAction(FExecuteAction::CreateLambda([]() {UE_LOG(LogTemp, Display, TEXT("UpdateTeamWorkAssets"))})),
//							TAttribute<FText>(),
//							LOCTEXT("UpdateTeamWorkAssetsTip", "Changes all selected point lights to spot lights."),
//							FSlateIcon(TEXT("CGToolStyle"), "CGToolStyle.MenuIcon")
//						);
//						FToolMenuSection& Section = InMenu->FindOrAddSection("CGTeamWork");
//						Section.AddEntry(UpdateContextEntry);
//
//					}), SectionInsertLocation);
//			}
//		}
//	}));
//	return 0;
//}
//int UCGToolsBPLibrary::GetCaptureDataFromCamera(ACineCameraActor* CameraActor)
//{
//	int result = 0;
//	UEditorActorSubsystem* ActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
//	ASceneCapture2D* Capture = Cast<ASceneCapture2D>(ActorSubsystem->SpawnActorFromClass(ASceneCapture2D::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator));
//	if (!Capture)
//		return result;
//	USceneCaptureComponent2D* Dst = Capture->GetCaptureComponent2D();
//	UCameraComponent* Src = CameraActor->GetCameraComponent();
//	if (Src && Dst)
//	{
//		Dst->SetWorldLocationAndRotation(Src->GetComponentLocation(), Src->GetComponentRotation());
//
//		FMinimalViewInfo CameraViewInfo;
//		Src->GetCameraView(/*DeltaTime =*/0.0f, CameraViewInfo);
//
//		// Use the input overscan factor to augment the destination component's FOV angle
//		// Note: The math relies on the filmback and focal length of the input camera component, which necessitates that it be a CineCameraComponet
//		if (UCineCameraComponent* SrcCineCameraComponent = Cast<UCineCameraComponent>(Src))
//		{
//			float OverscanFactor = 1.0;
//			float OriginalFocalLength = 35.0;
//			OriginalFocalLength = SrcCineCameraComponent->CurrentFocalLength;
//			// Guard against divide-by-zero
//			if (SrcCineCameraComponent->CurrentFocalLength <= 0.0f)
//			{
//				Dst->FOVAngle = 0.0f;
//			}
//			else
//			{
//				const float OverscanSensorWidth = SrcCineCameraComponent->Filmback.SensorWidth * OverscanFactor;
//				Dst->FOVAngle = FMath::RadiansToDegrees(2.0f * FMath::Atan(OverscanSensorWidth / (2.0f * OriginalFocalLength)));
//			}
//		}
//		else
//		{
//			Dst->FOVAngle = Src->FieldOfView;
//		}
//
//		const FPostProcessSettings& SrcPPSettings = CameraViewInfo.PostProcessSettings;
//		FPostProcessSettings& DstPPSettings = Dst->PostProcessSettings;
//
//		FWeightedBlendables DstWeightedBlendables = DstPPSettings.WeightedBlendables;
//
//		// Copy all of the post processing settings
//		DstPPSettings = SrcPPSettings;
//
//		// But restore the original blendables
//		DstPPSettings.WeightedBlendables = DstWeightedBlendables;
//	}
//	UTextureRenderTarget2D* RenderTarget = RenderTarget = NewObject<UTextureRenderTarget2D>();
//	RenderTarget->RenderTargetFormat = RTF_RGBA8;
//	RenderTarget->InitCustomFormat(512,512, EPixelFormat::PF_FloatRGBA, true);
//	RenderTarget->UpdateResourceImmediate();
//	Dst->TextureTarget = RenderTarget;
//	Dst->bCaptureEveryFrame = true;
//	Dst->bCaptureOnMovement = true;
//	Dst->CaptureSource = SCS_FinalToneCurveHDR;		//	The only choice to support post process
//	Dst->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_LegacySceneCapture;
//	Dst->bAlwaysPersistRenderingState = true;
//	Dst->CaptureScene();
//	int32 CaptureWidth = 512;
//	int32 CaptureHeight = 512;
//	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
//	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
//	TArray<FLinearColor> SurfaceData;
//	SurfaceData.AddUninitialized(CaptureWidth * CaptureHeight);
//	FTextureRenderTargetResource* RTSource = RenderTarget->GameThread_GetRenderTargetResource();
//	FReadSurfaceDataFlags ReadSurfaceDataFlags = FReadSurfaceDataFlags();
//	RTSource->ReadLinearColorPixelsPtr(SurfaceData.GetData(), ReadSurfaceDataFlags);
//	//const TArray64<uint8> PNGData = ImageWrapper->GetCompressed();
//	FString CaptureName = "D:/test.png";
//	FArchive* Ar = IFileManager::Get().CreateFileWriter(*CaptureName);
//	FBufferArchive Buffer;
//	bool bSuccess = FImageUtils::ExportRenderTarget2DAsPNG(RenderTarget, Buffer);
//	if (bSuccess)
//	{
//		Ar->Serialize(const_cast<uint8*>(Buffer.GetData()), Buffer.Num());
//	}
//	ImageWrapper.Reset();
//	delete Ar;
//	return 0;
//}

#undef LOCTEXT_NAMESPACE