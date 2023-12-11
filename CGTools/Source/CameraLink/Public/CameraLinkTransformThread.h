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
#include "ThreadImpl.h"
#include "IPAddress.h"

class FCameraLinkModule;

class CAMERALINK_API FCameraLinkTransformThread : public FCameraLinkService
{
public:
	ULevelSequence* Sequence = nullptr;
public:
	FCameraLinkTransformThread( CameraLinkActorServiceData* Data, FSocket* TmpSocket) : FCameraLinkService(Data, TmpSocket){ bSend = true;};
	virtual bool Operation();
	FString GetSeuqneceTransformProperty();
	FString GetProertyStr(UMovieScene* MovieScene, TArray<int32> AllTimes, FMovieSceneChannelProxy& ChannelProxy);
	TArray<int32> GetAllKeyTimes(UMovieScene* MovieScene, FMovieSceneChannelProxy& ChannelProxy);
	FString GetCameraPropertyJson(TArray<double> PropertyList);
	virtual void PreRun();
	virtual void BreakThread()
	{
		if (bRunning)
		{
			FCameraLinkService::BreakThread();
			UE_LOG(LogTemp, Display, TEXT("Transform Connection Disconnected!"));
			bSend = true;
		}
	}
};