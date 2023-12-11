#pragma once
#include "HAL/Runnable.h"
#include "CameraLink.h"
#include "CameraLinkData.h"

class ACineCameraActor;


class CAMERALINK_API FCameraLinkThread : public FRunnable
{
public:
	bool bRunning = false;
	bool bSend = false;
	FString IP = TEXT("127.0.0.1");
	uint32 Port = 32176;
	FSocket* Socket = nullptr;
	FOnConnectCloseDelegate OnCloseDelegate;
	void NotificationInfo(FString Label);
	virtual void Exit()
	{
		BreakThread();
	}
	virtual void BreakThread() {
		//if (Socket && bSend)
		//{
		//	FString Operation = TEXT("Close");
		//	FTCHARToUTF8 Convert(*Operation);
		//	TArray<uint8> Output(reinterpret_cast<const uint8*>(Convert.Get()), Convert.Length());
		//	int Sent = 0;
		//	Socket->Send(Output.GetData(), Output.Num(), Sent);
		//}
		if (Socket)
		{
			Socket->Close();
			delete Socket;
			Socket = nullptr;
			OnCloseDelegate.Broadcast();
		}
		bRunning = false;
		OnCloseDelegate.Broadcast();
	};
	virtual void Stop() {
		BreakThread();
	}
	virtual bool Operation() { return true; };
	virtual ~FCameraLinkThread()
	{
		OnCloseDelegate.Clear();
	}
};


class CAMERALINK_API FCameraLinkClient : public FCameraLinkThread
{
public:
	virtual uint32 Run();
	CameraLinkActorClientData* ClientData;
	FCameraLinkClient(CameraLinkActorClientData* Data) : ClientData(Data) {};
	CameraLinkActorClientData* GetData() {
		return ClientData;
	}
	virtual bool Operation();
};

class CAMERALINK_API FCameraLinkService : public FCameraLinkThread
{
public:
	CameraLinkActorServiceData* ServiceData;
	FCameraLinkService(CameraLinkActorServiceData* Data, FSocket* TmpSocket);
	CameraLinkActorServiceData* GetData() {
		return ServiceData;
	}
	virtual void PreRun() {};
	virtual uint32 Run();
};