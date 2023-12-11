#pragma once
#include "Engine/SceneCapture2D.h"
class UTextureRenderTarget2D;
class ACineCameraActor;
class ASceneCapture2D;
class FRunnableThread;
class UMaterial;


struct CameraLinkActorData
{
	//CameraLinkActorData(){};
	//CameraLinkActorData(const CameraLinkActorData& Data)
	//{
	//	TextureTarget = Data.TextureTarget;
	//	CineCameraActor = Data.CineCameraActor;
	//	Width = Data.Width;
	//	Height = Data.Height;
	//}
	UTextureRenderTarget2D* TextureTarget = nullptr;
	ACineCameraActor* CineCameraActor = nullptr;
	ASceneCapture2D* SceneCaptureActor = nullptr;
	int32 Width = 1280;
	int32 Height = 720;
	FDelegateHandle DelegateHandle;
	virtual ~CameraLinkActorData()
	{	
		if (CineCameraActor && DelegateHandle.IsValid())
			CineCameraActor->GetRootComponent()->TransformUpdated.Remove(DelegateHandle);
		if (SceneCaptureActor)
			SceneCaptureActor->Destroy();
	}
};


struct  CameraLinkActorServiceData : CameraLinkActorData
{
	//CameraLinkActorServiceData() {

	//};
	//CameraLinkActorServiceData(const CameraLinkActorServiceData& Data) : CameraLinkActorData(Data)
	//{
	//	MaterialBrush = Data.MaterialBrush;
	//	EnvTextureTarget = Data.EnvTextureTarget;
	//}
	//CameraLinkActorServiceData& operator= (CameraLinkActorServiceData& Data)
	//{
	//	this->MaterialBrush = Data.MaterialBrush;
	//	this->EnvTextureTarget = Data.EnvTextureTarget;
	//	return *this;
	//}
	TQueue<TArray<double>> TransformQueue;
	UTextureRenderTarget2D* EnvTextureTarget;
	UMaterial* MaterialBrush = nullptr;
	virtual ~CameraLinkActorServiceData()
	{

	}
};


struct  CameraLinkActorClientData : CameraLinkActorData
{
	//CameraLinkActorClientData(){

	//};
	//CameraLinkActorClientData& operator= (CameraLinkActorClientData& Data)
	//{
	//	this->MaterialBrush = Data.MaterialBrush;
	//	this->EnvTextureTarget = Data.EnvTextureTarget;
	//	return *this;
	//}
	//CameraLinkActorClientData(const CameraLinkActorClientData& Data) : CameraLinkActorData(Data)
	//{

	//};
	TQueue<TArray<uint8>> RTQueue;
	virtual ~CameraLinkActorClientData()
	{
		
	}
};

DECLARE_MULTICAST_DELEGATE(FOnConnectCloseDelegate);