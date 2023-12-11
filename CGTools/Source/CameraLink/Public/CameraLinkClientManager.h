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
#include "EngineUtils.h"

class FCameraLinkClientManager
{

public:
	CameraLinkActorClientData* ClientData = nullptr;
	FCameraLinkSocketRTClient* RTClient = nullptr;
	FCameraLinkTransfromClient* TransformClient = nullptr;
	TArray<FRunnableThread*> ClientThreads;

	bool HasConnecting()
	{
		UWorld* World = GEditor ? GEditor->GetEditorWorldContext(false).World() : nullptr;
		const EActorIteratorFlags Flags = EActorIteratorFlags::SkipPendingKill;
		bool bHas = false;
		if (ClientData)
		{
			for (TActorIterator<AActor> It(World, AActor::StaticClass(), Flags); It; ++It)
			{
				if (ClientData && *It == ClientData->CineCameraActor)
				{
					bHas = true;
					break;
				}
			}
		}
		return bHas;
	}

	void SetData(CameraLinkActorClientData* Data)
	{

		if (ClientData)
			delete ClientData;
		ClientData = Data;
	}

	void CreateClientThreads(CameraLinkActorClientData* Data)
	{
		if (RTClient && TransformClient && !TransformClient->bRunning && !RTClient->bRunning)
		{
			CreateImpl(Data);
		}
		else if (!RTClient && !TransformClient)
		{
			CreateImpl(Data);
		}
	}

	void CreateImpl(CameraLinkActorClientData* Data)
	{
		SetData(Data);
		RTClient = new FCameraLinkSocketRTClient(Data);
		TransformClient = new FCameraLinkTransfromClient(Data);
		TransformClient->bRunning = true;
		RTClient->bRunning = true;
		ClientThreads.Add(FRunnableThread::Create(TransformClient, TEXT("CameraLinkRunnable"), 0, TPri_BelowNormal));
		ClientThreads.Add(FRunnableThread::Create(RTClient, TEXT("CameraLinkRunnable"), 0, TPri_BelowNormal));
	}
};