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
class FCameraLinkThreadService : public FRunnable
{
public:
	FString IP = TEXT("127.0.0.1");
	ULevelSequence* Sequence = nullptr;
	UTextureRenderTarget2D* RenderTarget = nullptr;
	UTextureRenderTarget2D* SrcRenderTarget = nullptr;
	uint32 Port = 32176;
	bool bRunning = false;
	FSocket* Socket = nullptr;
	const FString Description = TEXT("CameraLink");
	virtual uint32 Run();
	bool StartSubThread(FString SocketType, FSocket* ClientSocket);
	FRunnableThread* TransformCon = nullptr;
	FRunnableThread* RTCon = nullptr;
	CameraLinkActorServiceData* CurrentThreadData = nullptr;
	CameraLinkActorServiceData* OldCurrentThreadData = nullptr;
	void AddCallbackForCameraIml(ACineCameraActor* CameraActor, UTextureRenderTarget2D* Target2D, CameraLinkActorServiceData* ServiceData);
	void SetData(CameraLinkActorServiceData* Data)
	{
		if (CurrentThreadData)
			delete CurrentThreadData;
		CurrentThreadData = Data;

	}
	FThreadClosedDelegate ClosedDelegate;
	void OnClose()
	{
		ClosedDelegate.Broadcast();
	}
	void CloseSubThreads();
};