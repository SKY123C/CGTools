#include "CameraLinkRTThread.h"
#include "RenderingThread.h"
#include "CameraLinkBPLib.h"
#include "Misc/FileHelper.h"


class FCameraLinkModule;
class UTexture2D;

bool FCameraLinkRTThread::Operation()
{
	if (!Socket || !ServiceData || Socket->GetConnectionState() != SCS_Connected)
		return false;
	int32 Read = 0;
	TArray<uint8> RawData;
	RawData.SetNum(ServiceData->Width * ServiceData->Height * 4);
	bool bSuccess = Socket->Recv(RawData.GetData(), RawData.Num(), Read);
	if (!bSuccess)
	{
		return false;
	}
	FString CameraLinkCommand = FString(5, UTF8_TO_TCHAR(RawData.GetData()));
	if (CameraLinkCommand.Find("Close") != -1)
	{
		return false;
	}
	AsyncTask(ENamedThreads::GameThread, [this, RawData]()
		{
			UCameraLinkBPLib::SetRenderTargetData(ServiceData->EnvTextureTarget, RawData, ServiceData->Width, ServiceData->Height, 4);
			//UpdateTexture2D(RawData);
		});
	return true;
}

void FCameraLinkRTThread::BreakThread()
{
	if (bRunning)
	{
		FPlatformProcess::Sleep(1);
		FCameraLinkService::BreakThread();
		UE_LOG(LogTemp, Display, TEXT("RT Connection Disconnected!"));
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				if (ServiceData->SceneCaptureActor)
					ServiceData->SceneCaptureActor->Destroy();
				NotificationInfo("CameraLink Disconnected!");
			});
	}
}