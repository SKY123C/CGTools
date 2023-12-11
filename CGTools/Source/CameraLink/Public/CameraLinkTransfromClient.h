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
#include "IPAddress.h"
#include "ThreadImpl.h"

class CAMERALINK_API FCameraLinkTransfromClient : public FCameraLinkClient
{
public:
	FCameraLinkTransfromClient(CameraLinkActorClientData* Data) : FCameraLinkClient(Data) {};
	void SetCameraProperty(FString Result);
	virtual bool Operation();
	virtual void BreakThread();
	void InitCameraSetting();
};