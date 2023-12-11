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
#include "CameraLinkThreadClient.h"
#include "CameraLinkRTClient.h"
#include "IPAddress.h"

DECLARE_MULTICAST_DELEGATE(FThreadClosedDelegate)
class FCameraLinkThreadClient : public FRunnable
{
public:
	FString IP = TEXT("127.0.0.1");
	uint32 Port = 32177;
	bool bRunning = false;
	FSocket* Socket = nullptr;
	const FString Description = TEXT("CameraLink");
	virtual uint32 Run();
	bool StartSubThread(FString SocketType, FSocket* ClientSocket);
	FRunnableThread* TransformCon = nullptr;
	FRunnableThread* RTCon = nullptr;
	CameraLinkActorClientData* CurrentThreadData;
	FCameraLinkSocketRTClient* RTClient = nullptr;
	FCameraLinkTransfromClient* TransformClient = nullptr;
	FThreadClosedDelegate ClosedDelegate;
	void OnClose()
	{
		ClosedDelegate.Broadcast();
	}
	void CloseSubThreads();
};