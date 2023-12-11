#include "CameraLinkTransfromClient.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "AssetToolsModule.h"
#include "Factories/TextureRenderTargetFactoryNew.h"
#include "AssetToolsModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CameraLinkBPLib.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/ArrayReader.h"

TArray<double> GetValueFromJson(TSharedPtr<FJsonObject> RootJsonObj, FString Field)
{
	TArray<double> Values;
	const TArray< TSharedPtr<FJsonValue> >* LocationPtr;
	if (RootJsonObj->TryGetArrayField(*Field, LocationPtr))
	{
		for (TSharedPtr<FJsonValue> item : *LocationPtr)
		{
			double value = 0;
			item->TryGetNumber(value);
			Values.Add(value);
		}
	}
	return Values;
}

void FCameraLinkTransfromClient::SetCameraProperty(FString Result)
{
	FString NewResult = Result.Replace(TEXT("\n"), TEXT("")).Replace(TEXT("\t"), TEXT("")).Replace(TEXT("\r"), TEXT(""));
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(NewResult);
	TSharedPtr<FJsonObject> RootJsonObj = MakeShareable(new FJsonObject);
	if (FJsonSerializer::Deserialize(JsonReader, RootJsonObj))
	{
		TArray<double> Location = GetValueFromJson(RootJsonObj, "Location");
		TArray<double> Rotaion = GetValueFromJson(RootJsonObj, "Rotation");
		TArray<double> FocalLength = GetValueFromJson(RootJsonObj, "FocalLength");
		// Check for changes in attributes;

		AsyncTask(ENamedThreads::GameThread, [this, Rotaion, Location]()
			{
				if (Rotaion.Num() > 2)
					ClientData->CineCameraActor->K2_SetActorRotation(FRotator(Rotaion[1], Rotaion[2], Rotaion[0]), true);
				if (Location.Num() > 2)
				{
					FHitResult HitResult;
					ClientData->CineCameraActor->K2_SetActorLocation(FVector(Location[0], Location[1], Location[2]), false, HitResult, false);
				}
			});

	}
}

bool FCameraLinkTransfromClient::Operation()
{
	FString Operation = TEXT("transform");
	FTCHARToUTF8 Convert(*Operation);
	TArray<uint8> Output(reinterpret_cast<const uint8*>(Convert.Get()), Convert.Length());
	int Sent = 0;
	if (Socket->Send(Output.GetData(), Output.Num(), Sent))
	{
		InitCameraSetting();
		while (bRunning)
		{
			if (Socket->GetConnectionState() != SCS_Connected)
				break;
			int Read = 0;
			FArrayReader ReceivedData = FArrayReader();
			ReceivedData.SetNumUninitialized(1024);
			bool bSuccess = Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
			if (!bSuccess)
				break;
			FString CameraLinkCommand = FString(5, UTF8_TO_TCHAR(ReceivedData.GetData()));
			if (CameraLinkCommand.Find("Close") != -1)
			{
				ClientData->RTQueue.Enqueue(TArray<uint8>());
				break;
			}
			//FString Result = FString(ReceivedData.Num(), UTF8_TO_TCHAR(ReceivedData.GetData()));
			uint8 data[1024];
			FMemory::Memzero(data, 1024);
			FMemory::Memcpy(data, ReceivedData.GetData(), ReceivedData.Num());
			//FString Result = FString(ReceivedData.Num(), UTF8_TO_TCHAR(ReceivedData.GetData()));
			FString Result = ((const char*)data);
			SetCameraProperty(Result);
		}
	}
	return false;
}

void FCameraLinkTransfromClient::BreakThread()
{
	FCameraLinkClient::BreakThread();
	ClientData->RTQueue.Enqueue(TArray<uint8>());
	UE_LOG(LogTemp, Display, TEXT("Transform Disconnect!"));
}

void FCameraLinkTransfromClient::InitCameraSetting()
{
	FArrayReader ReceivedData = FArrayReader();
	int Read = 0;
	ReceivedData.SetNumUninitialized(1024);
	bool bSuccess = Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	if (!bSuccess)
		return;
	//FString Result = FString(ReceivedData.Num(), UTF8_TO_TCHAR(ReceivedData.GetData()));
	uint8 data[1024];
	FMemory::Memzero(data, 1024);
	FMemory::Memcpy(data, ReceivedData.GetData(), ReceivedData.Num());
	//FString Result = FString(ReceivedData.Num(), UTF8_TO_TCHAR(ReceivedData.GetData()));
	FString Result = ((const char*)data);
	FString NewResult = Result.Replace(TEXT("\n"), TEXT("")).Replace(TEXT("\t"), TEXT("")).Replace(TEXT("\r"), TEXT(""));
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(NewResult);
	TSharedPtr<FJsonObject> RootJsonObj = MakeShareable(new FJsonObject);
	if (FJsonSerializer::Deserialize(JsonReader, RootJsonObj))
	{
		TArray<double> FOV = GetValueFromJson(RootJsonObj, "FOV");
		TArray<double> SensorSize = GetValueFromJson(RootJsonObj, "SensorSize");
		TArray<double> FocalLength = GetValueFromJson(RootJsonObj, "FocalLength");
		AsyncTask(ENamedThreads::GameThread, [this, FOV, SensorSize, FocalLength]()
			{
				UCineCameraComponent* Component = ClientData->CineCameraActor->GetCineCameraComponent();
				Component->CurrentFocalLength = FocalLength[0];
#if ENGINE_MAJOR_VERSION > 4
				Component->Filmback.SensorWidth = SensorSize[0];
				Component->Filmback.SensorHeight = SensorSize[1];

#elif ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 21
				Component->Filmback.SensorWidth = SensorSize[0];
				Component->Filmback.SensorHeight = SensorSize[1];
#else
				Component->FilmbackSettings.SensorWidth = SensorSize[0];
				Component->FilmbackSettings.SensorHeight = SensorSize[1];
#endif
				ClientData->SceneCaptureActor->GetCaptureComponent2D()->FOVAngle = FOV[0];
			});

	}
}