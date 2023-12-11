#include "CameraLinkThreadService.h"
#include "CameraLinkTransformThread.h"
#include "CameraLinkRTThread.h"


uint32 FCameraLinkThreadService::Run()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr(0, Port);
	Socket = SocketSubsystem->CreateSocket(NAME_Stream, *Description, true);
	bool Ok;
	Addr->SetIp(*IP, Ok);
	Socket->Bind(*Addr);
	Socket->Listen(2);
	while (bRunning)
	{
		FSocket* ClientSocket = Socket->Accept("CameraLink Test");
		if (!ClientSocket)
			continue;
		if (!CurrentThreadData)
			continue;
		TArray<uint8> ReceivedData;
		ReceivedData.Init(0, 50);
		int Read = 0;
		ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		FString SocketType = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
		FString Notion = SocketType + TEXT(" Starting Connect!");
		StartSubThread(SocketType, ClientSocket);
		UE_LOG(LogTemp, Display, TEXT("%s"), *Notion);
	}
	return 0;
}

bool FCameraLinkThreadService::StartSubThread(FString SocketType, FSocket* ClientSocket)
{
	FString CameraLinkCommand = SocketType.ToLower();
	UE_LOG(LogTemp, Display, TEXT("%s"), *CameraLinkCommand);
	if (CameraLinkCommand == "transform")
	{
		//
		FCameraLinkTransformThread* TransformThread = new FCameraLinkTransformThread(CurrentThreadData, ClientSocket);
		AddCallbackForCameraIml(CurrentThreadData->CineCameraActor, CurrentThreadData->TextureTarget, CurrentThreadData);
		TransformCon = FRunnableThread::Create(TransformThread, TEXT("TransformRunnable"), 0, TPri_BelowNormal);
	}
	else if (CameraLinkCommand == "rendertarget")
	{
		if (OldCurrentThreadData)
			delete OldCurrentThreadData;
		FCameraLinkRTThread* RTThread = new FCameraLinkRTThread(CurrentThreadData, ClientSocket);
		RTThread->OnCloseDelegate.AddRaw(this, &FCameraLinkThreadService::OnClose);
		RTCon = FRunnableThread::Create(RTThread, TEXT("RTRunnable"), 0, TPri_BelowNormal);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("CameraLink Command Error: %s"), *CameraLinkCommand);
	}
	return true;
}

void FCameraLinkThreadService::CloseSubThreads()
{
	if (TransformCon)
	{
		TransformCon->Kill();
		TransformCon = nullptr;
	}
	if (RTCon)
	{
		RTCon->Kill();
		RTCon = nullptr;
	}
}

void FCameraLinkThreadService::AddCallbackForCameraIml(ACineCameraActor* CameraActor, UTextureRenderTarget2D* Target2D, CameraLinkActorServiceData* ServiceData)
{
	if (!CameraActor)
		return;
	USceneComponent* RootComponent = CameraActor->GetRootComponent();
	if (!RootComponent)
		return;
	FDelegateHandle DelegateHandle = RootComponent->TransformUpdated.AddLambda(
		[this, CameraActor, ServiceData](USceneComponent* Component, EUpdateTransformFlags /*UpdateTransformFlags*/, ETeleportType /*Teleport*/)
		{
			{
				if (ServiceData)
				{
					TArray<double> TransformList;
					FVector SrcLocation = Component->GetComponentLocation();
					FRotator SrcRotation = Component->GetComponentRotation();
					TransformList.Add(SrcLocation.X);
					TransformList.Add(SrcLocation.Y);
					TransformList.Add(SrcLocation.Z);
					TransformList.Add(SrcRotation.Roll);
					TransformList.Add(SrcRotation.Pitch);
					TransformList.Add(SrcRotation.Yaw);
					ServiceData->TransformQueue.Enqueue(TransformList);
				
				}
			}
		}
	);
	ServiceData->DelegateHandle = DelegateHandle;
}