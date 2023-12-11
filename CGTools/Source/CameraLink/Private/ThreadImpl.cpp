#include "ThreadImpl.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

void FCameraLinkThread::NotificationInfo(FString Lable)
{

	FNotificationInfo Info(FText::FromString(Lable));
	Info.bFireAndForget = true;
	Info.FadeOutDuration = 5.0f;
	Info.ExpireDuration = 5.0f;
	AsyncTask(ENamedThreads::GameThread, [this, Info]()
	{
			FSlateNotificationManager::Get().AddNotification(Info);
	});
}

uint32 FCameraLinkClient::Run()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	const FString Description = TEXT("CameraLinkClient");
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	bool bIsValid = false;
	Addr->SetIp(*IP, bIsValid);
	Addr->SetPort(Port);
	Socket = SocketSubsystem->CreateSocket(NAME_Stream, *Description, true);
#if ENGINE_MAJOR_VERSION >4
	if (Socket->Connect(Addr.Get()))
#else
	if (Socket->Connect(Addr.Get()))
#endif
	{
		Operation();
	}
	BreakThread();
	OnCloseDelegate.Broadcast();
	return 1;
}

bool FCameraLinkClient::Operation()
{
	return true;
}


FCameraLinkService::FCameraLinkService(CameraLinkActorServiceData* Data, FSocket* TmpSocket) : ServiceData(Data)
{
	bRunning = true;
	Socket = TmpSocket;
}

uint32 FCameraLinkService::Run()
{
	PreRun();
	NotificationInfo("CameraLink Start Connect!");
	while (bRunning)
	{
		bool Result = Operation();
		if (!Result)
			break;
	}
	BreakThread();
	OnCloseDelegate.Broadcast();
	return 0;
}
