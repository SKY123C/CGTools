#include "Editor/CameraLinker.h"
#include "CoreUObject.h"
#include "Editor.h"
#include "Engine/SceneCapture2D.h"
#include "CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Factories/TextureRenderTargetFactoryNew.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "CameraLinkBPLib.h"
#include "CameraLinkThreadService.h"
#if ENGINE_MAJOR_VERSION >=5
#include "Subsystems/EditorAssetSubsystem.h"
#include "Subsystems/EditorActorSubsystem.h"
#endif
#include "EditorLevelLibrary.h"
#include "Misc/MessageDialog.h"

FCameraLinker::FCameraLinker()
{
	ServiceThreadImpl = new FCameraLinkThreadService;
	//RTClient = new FCameraLinkSocketRTClient(&ClientData);
	//TransformClient = new FCameraLinkTransfromClient(&ClientData);
}

void FCameraLinker::StartService(ACineCameraActor* CineCameraActor, TArray<AActor*> ShownActors, int32 Width, int32 Height)
{
	//check
	ServiceInit();
	CameraLinkActorServiceData* ServiceData = new CameraLinkActorServiceData;
	ServiceData->CineCameraActor = CineCameraActor;
	CreateSceneCapture(CineCameraActor, ShownActors, RTName, ServiceData);
	ServiceData->EnvTextureTarget = CreateTempTextureTarget(EnvRTName, CameraLinkPackagePath, ServiceData);
	ServiceData->MaterialBrush = CreateCameraLinkMaterial(ServiceData);
	ServiceThreadImpl->SetData(ServiceData);
	if (!ServiceThreadImpl->bRunning)
	{
		ServiceThreadImpl->bRunning = true;
		ServiceThreadImpl->ClosedDelegate.AddRaw(this, &FCameraLinker::OnServiceThreadsClose);
		FRunnableThread::Create(ServiceThreadImpl, TEXT("CameraLinkRunnable"), 0, TPri_BelowNormal);
	}
	else
	{
		//
	}
}

UTextureRenderTarget2D* FCameraLinker::CreateSceneCapture(ACineCameraActor* Actor, TArray<AActor*>& Actors, FString Name, CameraLinkActorData* CameraLinkData)
{
	UTextureRenderTarget2D* RenderTarget = nullptr;
	ASceneCapture2D* Capture = Cast<ASceneCapture2D>(GEditor->GetEditorWorldContext(false).World()->SpawnActor(ASceneCapture2D::StaticClass()));
	if (!Capture)
		return RenderTarget;

	USceneCaptureComponent2D* Dst = Capture->GetCaptureComponent2D();

	//Set Setting
	UCineCameraComponent* SrcCineCameraComponent = Actor->GetCineCameraComponent();
	if (!SrcCineCameraComponent)
		return RenderTarget;

	FVector z = Actor->GetRootComponent()->GetComponentLocation();

	FRotator x = Actor->GetRootComponent()->GetComponentRotation();
	//Dst->SetWorldLocationAndRotation(z, x);
	
	Actor->GetRootComponent()->SetWorldLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	Capture->AttachToActor(Actor, FAttachmentTransformRules::KeepRelativeTransform);
	float OverscanFactor = 1.0;
	float OriginalFocalLength = 35.0;
	OriginalFocalLength = SrcCineCameraComponent->CurrentFocalLength;
	// Guard against divide-by-zero
	if (SrcCineCameraComponent->CurrentFocalLength <= 0.0f)
	{
		Dst->FOVAngle = 0.0f;
	}
	else
	{
#if ENGINE_MAJOR_VERSION < 5 && ENGINE_MINOR_VERSION < 27
		const float OverscanSensorWidth = SrcCineCameraComponent->FilmbackSettings.SensorWidth * OverscanFactor;
#else
		const float OverscanSensorWidth = SrcCineCameraComponent->Filmback.SensorWidth * OverscanFactor;
#endif
		Dst->FOVAngle = FMath::RadiansToDegrees(2.0f * FMath::Atan(OverscanSensorWidth / (2.0f * OriginalFocalLength)));
	}
	FMinimalViewInfo CameraViewInfo;
	SrcCineCameraComponent->GetCameraView(/*DeltaTime =*/0.0f, CameraViewInfo);
	const FPostProcessSettings& SrcPPSettings = CameraViewInfo.PostProcessSettings;
	FPostProcessSettings& DstPPSettings = Dst->PostProcessSettings;
	FWeightedBlendables DstWeightedBlendables = DstPPSettings.WeightedBlendables;
	DstPPSettings = SrcPPSettings;
	DstPPSettings.WeightedBlendables = DstWeightedBlendables;

	//Create TextureRender
	RenderTarget = CreateTempTextureTarget(Name, CameraLinkPackagePath, CameraLinkData);
	Dst->TextureTarget = RenderTarget;
	Dst->bCaptureEveryFrame = true;
	Dst->bCaptureOnMovement = true;
	Dst->CaptureSource = SCS_SceneColorHDR;		//	The only choice to support post process
	if (Actors.Num() > 0)
	{
		Dst->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		Dst->ShowOnlyActors = Actors;
	}
	else
	{
		Dst->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	}
	Dst->bAlwaysPersistRenderingState = true;
	Dst->bAlwaysPersistRenderingState = true;
	Dst->CaptureScene();
	CameraLinkData->CineCameraActor = Actor;
	CameraLinkData->SceneCaptureActor = Capture;
	CameraLinkData->TextureTarget = RenderTarget;
	return RenderTarget;
}

UMaterial* FCameraLinker::CreateCameraLinkMaterial(CameraLinkActorServiceData* CameraLinkData)
{
	if (!CameraLinkData->EnvTextureTarget || !CameraLinkData->TextureTarget)
		return nullptr;
	UTextureRenderTarget2D* SrcRT = CameraLinkData->TextureTarget;
	UTextureRenderTarget2D* DstRT = CameraLinkData->EnvTextureTarget;
	FString Name = "CameraLinkRTMaterial";
	FString PackageName = CameraLinkPackagePath + Name;
#if ENGINE_MAJOR_VERSION >=5
	bool bCreate = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>()->DoesAssetExist(PackageName);
#else
	bool bCreate = UEditorAssetLibrary::DoesAssetExist(PackageName);
#endif

	if (bCreate)
		return LoadObject<UMaterial>(NULL, *PackageName);
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
	UMaterial* Material = Cast<UMaterial>(FAssetToolsModule::GetModule().Get().CreateAsset(Name, FPackageName::GetLongPackagePath(CameraLinkPackagePath), UMaterial::StaticClass(), Factory));
	Material->MaterialDomain = EMaterialDomain::MD_UI;
	Material->PostEditChange();
	UMaterialExpressionIf* ExpressionIf = NewObject<UMaterialExpressionIf>(Material);
	UMaterialExpressionTextureSample* SrcExpressionTextureSample = NewObject<UMaterialExpressionTextureSample>(Material);
	UMaterialExpressionTextureSample* DstExpressionTextureSample = NewObject<UMaterialExpressionTextureSample>(Material);
	SrcExpressionTextureSample->Texture = SrcRT;
	DstExpressionTextureSample->Texture = DstRT;

	//Init Position
	ExpressionIf->MaterialExpressionEditorX = -144;
	ExpressionIf->MaterialExpressionEditorY = -16;
	SrcExpressionTextureSample->MaterialExpressionEditorX = -560;
	SrcExpressionTextureSample->MaterialExpressionEditorY = -128;
	DstExpressionTextureSample->MaterialExpressionEditorX = -576;
	DstExpressionTextureSample->MaterialExpressionEditorY = 176;
	//Connect
	ExpressionIf->ConstB = 0;
	ExpressionIf->A.Connect(4, SrcExpressionTextureSample);
	ExpressionIf->ALessThanB.Connect(0, DstExpressionTextureSample);
	ExpressionIf->AGreaterThanB.Connect(0, DstExpressionTextureSample);
	ExpressionIf->AEqualsB.Connect(0, SrcExpressionTextureSample);
#if ENGINE_MAJOR_VERSION > 4
	Material->GetExpressionCollection().AddExpression(ExpressionIf);
	Material->GetExpressionCollection().AddExpression(SrcExpressionTextureSample);
	Material->GetExpressionCollection().AddExpression(DstExpressionTextureSample);
	Material->GetEditorOnlyData()->EmissiveColor.Connect(0, ExpressionIf);
#else
	Material->Expressions.Add(ExpressionIf);
	Material->Expressions.Add(SrcExpressionTextureSample);
	Material->Expressions.Add(DstExpressionTextureSample);
	Material->EmissiveColor.Connect(0, ExpressionIf);
#endif
	Material->PostEditChange();
	return Material;
}

bool FCameraLinker::CheckValid()
{
	bool Result = true;
	return false;
}

UTextureRenderTarget2D* FCameraLinker::CreateTempTextureTarget(FString Name, FString PackagePath, CameraLinkActorData* Data)
{
	FString PackageName = PackagePath + Name;
#if ENGINE_MAJOR_VERSION >=5
	bool bCreate = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>()->DoesAssetExist(PackageName);
#else
	bool bCreate = UEditorAssetLibrary::DoesAssetExist(PackageName);
#endif

	if (bCreate)
		return LoadObject<UTextureRenderTarget2D>(NULL, *PackageName);
	UTextureRenderTargetFactoryNew* Factory = NewObject<UTextureRenderTargetFactoryNew>();
	UObject* NewAsset = FAssetToolsModule::GetModule().Get().CreateAsset(Name, FPackageName::GetLongPackagePath(PackagePath), UTextureRenderTarget2D::StaticClass(), Factory);
	UTextureRenderTarget2D* RenderTarget = Cast<UTextureRenderTarget2D>(NewAsset);
	RenderTarget->RenderTargetFormat = RTF_RGBA8;
	RenderTarget->InitCustomFormat(Data->Width, Data->Height, EPixelFormat::PF_FloatRGBA, true);
	RenderTarget->UpdateResourceImmediate();
	return RenderTarget;
}

void FCameraLinker::OnActorDestroyedCallback(AActor* CameraActor)
{


}

void FCameraLinker::AddColorToQueue(TArray<FColor> Colors, CameraLinkActorClientData* ClientData)
{
	TArray<uint8> RawData;

	for (FColor& Color : Colors)
	{
		RawData.Add(Color.R);
		RawData.Add(Color.G);
		RawData.Add(Color.B);
		RawData.Add(Color.A);
	}
	ClientData->RTQueue.Enqueue(RawData);
}

void FCameraLinker::StartClient()
{
	if (ClientManager.HasConnecting())
	{
		FText const Title = FText::FromString("CameraLink");
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("The current project is connected"), &Title);
		return;
	}
#if ENGINE_MAJOR_VERSION >= 5
	AActor* SpawnedActor = GEditor->GetEditorSubsystem<UEditorActorSubsystem>()->SpawnActorFromClass(
		ACineCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, true);
#else
	AActor* SpawnedActor = UEditorLevelLibrary::SpawnActorFromClass(ACineCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
#endif
	CameraLinkActorClientData* ClientData = new CameraLinkActorClientData;
	ACineCameraActor* CaptureCameraActor = Cast<ACineCameraActor>(SpawnedActor);
	ClientData->CineCameraActor = CaptureCameraActor;
	TArray<AActor*> ShowActors;
	ClientData->TextureTarget = CreateSceneCapture(CaptureCameraActor, ShowActors, "CameraLinkSceneRT", ClientData);
	FTextureRenderTargetResource* RTSource = ClientData->TextureTarget->GameThread_GetRenderTargetResource();
	CaptureCameraActor->GetRootComponent()->TransformUpdated.AddLambda(
		[this, RTSource, ClientData](USceneComponent* Component, EUpdateTransformFlags /*UpdateTransformFlags*/, ETeleportType /*Teleport*/)
		{
			ENQUEUE_RENDER_COMMAND(ReadRTPixelsCommand)
				([this, RTSource, ClientData](FRHICommandListImmediate& RHICmdList)
					{
						TArray<FColor> RawPixels;
						FIntRect SourceRect = FIntRect(0, 0, RTSource->GetSizeXY().X, RTSource->GetSizeXY().Y);
						RawPixels.SetNum(SourceRect.Width() * SourceRect.Height());
						FReadSurfaceDataFlags ReadDataFlags;
						RHICmdList.ReadSurfaceData(RTSource->GetRenderTargetTexture(), SourceRect, RawPixels, ReadDataFlags);
						AddColorToQueue(RawPixels, ClientData);
					}

			);
		}
	);
	ClientManager.CreateClientThreads(ClientData);
}

void FCameraLinker::AddReferencedObjects(FReferenceCollector& ReferenceCollector)
{
	
}

void FCameraLinker::AddShownActors(TArray<AActor*> Actors)
{
	if (Actors.Num() > 0)
		return;
}

void FCameraLinker::ServiceInit()
{
	ServiceThreadImpl->CloseSubThreads();
	if (ServiceThreadImpl->CurrentThreadData)
	{	

	}
}

void FCameraLinker::CloseService()
{
	ServiceInit();
	ServiceThreadImpl->SetData(nullptr);
}

void FCameraLinker::OnServiceThreadsClose()
{
	//if (ServiceData->DelegateHandle.IsValid())
	//{
	//	ServiceData->CineCameraActor->GetRootComponent()->TransformUpdated.Remove(ServiceData->DelegateHandle);
	//	ServiceData->DelegateHandle.Reset();
	//}
	//ServiceData->TransformQueue.Empty();
	OnDestroyDelegate.Broadcast();
}

void FCameraLinker::OnClientThreadsClose()
{

}

void FCameraLinker::InitSetting()
{

}