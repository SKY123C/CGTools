#include "CameraLinkRTClient.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "AssetToolsModule.h"
#include "Factories/TextureRenderTargetFactoryNew.h"
#include "AssetToolsModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CameraLinkBPLib.h"

class FCameraLinkModule;

bool FCameraLinkSocketRTClient::Operation()
{
	FString Operation = TEXT("rendertarget");
	FTCHARToUTF8 Convert(*Operation);
	TArray<uint8> Output(reinterpret_cast<const uint8*>(Convert.Get()), Convert.Length());
	int Sent = 0;
	if (Socket->Send(Output.GetData(), Output.Num(), Sent))
	{
		while (bRunning)
		{
			if (!Socket || Socket->GetConnectionState() != SCS_Connected)
				break;
			if (ClientData->RTQueue.IsEmpty())
				continue;
			int Sent1 = 0;
			TArray<uint8> RawData;
			ClientData->RTQueue.Dequeue(RawData);
			if (RawData.Num() < 1)
			{
				break;
			}
			bool ConnetState = Socket->Send(RawData.GetData(), RawData.Num(), Sent1);
			if (!ConnetState)
			{
				break;
			}

		}
	}
	return false;
}

void FCameraLinkSocketRTClient::BreakThread()
{
	FCameraLinkClient::BreakThread();
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			UCineCameraComponent* Component = ClientData->CineCameraActor->GetCineCameraComponent();
			if (ClientData)
			{
				ClientData->CineCameraActor->Destroy();
				ClientData->SceneCaptureActor->Destroy();
			}
		});
	UE_LOG(LogTemp, Display, TEXT("RT Connection Disconnected!"));
}
