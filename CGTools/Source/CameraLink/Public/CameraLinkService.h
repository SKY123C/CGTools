#pragma once
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Sockets.h"
#include "LevelSequence.h"
#include "CineCameraActor.h"
#include "MovieSceneSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneChannelHandle.h"
#include "Curves/KeyHandle.h"
#include "Misc/FrameNumber.h"
#include "Misc/FrameRate.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CameraLinkTransformThread.h"
#include "CameraLinkRTThread.h"
#include "IPAddress.h"

DECLARE_MULTICAST_DELEGATE(FThreadClosedDelegate)

class CAMERALINK_API FCameraLinkSocketService : public FRunnable
{
public:
	FString IP = TEXT("127.0.0.1");
	ULevelSequence* Sequence = nullptr;
	UTextureRenderTarget2D* RenderTarget = nullptr;
	UTextureRenderTarget2D* SrcRenderTarget = nullptr;
	uint32 Port = 32176;
	FSocket* Socket = nullptr;
	FCameraLinkTransformThread* TransformThread = nullptr;
	FCameraLinkRTThread* RTThread = nullptr;
	bool bRunning = false;
	FThreadClosedDelegate OnSubThreadCloseDelegate;
	TArray<FRunnableThread*> Threads;

	void ExecSubThreadCloseDelegate()
	{
		if (TransformThread)
			TransformThread->bRunning = false;
		OnSubThreadCloseDelegate.Broadcast();
		
	}

	void ReloadThread(ULevelSequence* SrcSequence, UTextureRenderTarget2D* SrcTarget)
	{
		TransformThread->Sequence = SrcSequence;
	}

	void SetData(bool result, UTextureRenderTarget2D* SrcTarget, int32 Width, int32 Height)
	{
		bRunning = result;
		SrcRenderTarget = SrcTarget;
	}

	FCameraLinkSocketService(const CameraLinkActorServiceData& Data)
	{
		/*TransformThread = new FCameraLinkTransformThread(Data, nullptr);
		RTThread = new FCameraLinkRTThread(Data, nullptr);*/
	}

	void ClearSubThread()
	{
		if (Threads.Num() > 0)
		{
			for (FRunnableThread* Thread : Threads)
			{
				if (Thread)
				{
					Thread->Kill();
					delete Thread;
				}
			}
			Threads.Empty();
		}
	}
	virtual uint32 Run() {
		//TransformThread->OnCloseDelegate.AddRaw(this, &FCameraLinkSocketService::ExecSubThreadCloseDelegate);
		RTThread->OnCloseDelegate.AddRaw(this, &FCameraLinkSocketService::ExecSubThreadCloseDelegate);
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		const FString Description = TEXT("CameraLink");
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
			TArray<uint8> ReceivedData;
			ReceivedData.Init(0, 50);
			int Read = 0;
			ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
			FString SocketType = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
			FString Notion = SocketType + TEXT(" Starting Connect!");
			UE_LOG(LogTemp, Display, TEXT("%s"), *Notion);
			StartSubThread(SocketType, ClientSocket);
		}
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
		bRunning = false;
		ClearSubThread();
		return 0;
	}

	bool StartSubThread(FString SocketType, FSocket* ClientSocket)
	{
	//	ClearSubThread();
		FString CameraLinkCommand = SocketType.ToLower();
		UE_LOG(LogTemp, Display, TEXT("%s"), *CameraLinkCommand);
		if (CameraLinkCommand == "transform")
		{
			
			TransformThread->bRunning = true;
			TransformThread->Sequence = Sequence;
			TransformThread->Socket = ClientSocket;
			FRunnableThread* Thread = FRunnableThread::Create(TransformThread, TEXT("TransformRunnable"), 0, TPri_BelowNormal);
			Threads.Add(Thread);
		}
		else if (CameraLinkCommand == "rendertarget")
		{
			RTThread->bRunning = true;
			RTThread->Socket = ClientSocket;
			//RTThread->SrcRenderTarget = SrcRenderTarget;
			FRunnableThread* Thread = FRunnableThread::Create(RTThread, TEXT("RTRunnable"), 0, TPri_BelowNormal);
			Threads.Add(Thread);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("CameraLink Command Error: %s"), *CameraLinkCommand);
		}
		return true;
	}

	bool isHasConnected()
	{
		return TransformThread->bRunning && RTThread->bRunning;
	}

	~FCameraLinkSocketService()
	{
		if (Socket != nullptr)
		{
			delete Socket;
			Socket = nullptr;
		}
		delete TransformThread;delete RTThread;
		ClearSubThread();
	}
};
