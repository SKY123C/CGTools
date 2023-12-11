#pragma once
#include "CameraLinkData.h"
#include "CameraLinkThreadService.h"
#include "CameraLinkClientManager.h"

class UTextureRenderTarget2D;

DECLARE_MULTICAST_DELEGATE(FCameraDestroyedDelegate);

class CAMERALINK_API FCameraLinker : public TSharedFromThis<FCameraLinker>, public FGCObject
{
public:
	FCameraLinker();
	//TSharedRef<FUICommandList> CameraLinkerSharedBindings;
	void StartService(ACineCameraActor* CineCameraActor, TArray<AActor*> ShownActor, int32 Width, int32 Height);
	int32 RTWidth;
	int32 RTHeight;
	UTextureRenderTarget2D* CreateSceneCapture(ACineCameraActor* Actor, TArray<AActor*>& Actors, FString RTName, CameraLinkActorData* CameraLinkData);
	UMaterial* CreateCameraLinkMaterial(CameraLinkActorServiceData* CameraLinkData);
	bool CheckValid();
	FString RTName = "CameraLinkServiceRT";
	FString EnvRTName = "CameraLinkServiceEnvRT";
	FString CameraLinkPackagePath = "/Game/Meshes/CGPublicity/Content/CameraLink/";
	UTextureRenderTarget2D* CreateTempTextureTarget(FString Name, FString PackagePath, CameraLinkActorData* Data);
	void OnActorDestroyedCallback(AActor* Actor);
	void StartClient();
	void AddColorToQueue(TArray<FColor> Colors, CameraLinkActorClientData* ClientData);
	virtual void AddReferencedObjects(FReferenceCollector& ReferenceCollector);
	void AddShownActors(TArray<AActor*> Actors);
	void ServiceInit();
	void CloseService();
	void OnServiceThreadsClose();
	void OnClientThreadsClose();
	void InitSetting();
	FCameraDestroyedDelegate OnDestroyDelegate;
	UMaterial* GetMaterialBrush()
	{
		return ServiceThreadImpl->CurrentThreadData->MaterialBrush;
	}
	~FCameraLinker()
	{

	}
public:
	//Service
	FCameraLinkThreadService* ServiceThreadImpl;
	FRunnableThread* ServiceThread;
	bool bHasConnectState = false;
public:
	//Client
	FCameraLinkClientManager ClientManager;
};
