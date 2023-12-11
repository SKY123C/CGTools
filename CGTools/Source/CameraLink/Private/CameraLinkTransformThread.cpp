#include "CameraLinkTransformThread.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"


bool FCameraLinkTransformThread::Operation()
{
	if (!Socket || Socket->GetConnectionState() != SCS_Connected || !bRunning || !ServiceData)
		return false;

	if (ServiceData->TransformQueue.IsEmpty())
		return true;
	TArray<double> TransformData;
	ServiceData->TransformQueue.Dequeue(TransformData);
	if (TransformData.Num() < 1)
	{
		bSend = false;
		return false;
	}
	FString Result = GetCameraPropertyJson(TransformData);
	if (Result.IsEmpty())
		return true;
	TCHAR* seriallizedChar = Result.GetCharArray().GetData();
	int32 size = FCString::Strlen(seriallizedChar);
	int32 Sent = 0;
	bool bConnectState = Socket->Send((uint8*)TCHAR_TO_UTF8(seriallizedChar), size * sizeof(TCHAR), Sent);
	if (!bConnectState)
	{
		bSend = false;
		return false;
	}
	return true;
}

FString FCameraLinkTransformThread::GetSeuqneceTransformProperty()
{
	FString Result = "";
	if (!Sequence)
		return Result;
	UMovieScene3DTransformTrack* Track = nullptr;
	UMovieScene* MovieScene = Sequence->GetMovieScene();
	if (!MovieScene)
		return Result;
	int32 Count = MovieScene->GetSpawnableCount();
	//CameraSpawnables.Reserve(Count);

	for (int32 i = 0; i < Count; ++i)
	{
		FMovieSceneSpawnable& MovieSceneSpawnable = MovieScene->GetSpawnable(i);

		if (MovieSceneSpawnable.GetObjectTemplate()->GetClass() == ACineCameraActor::StaticClass())
		{
			Track = MovieScene->FindTrack<UMovieScene3DTransformTrack>(MovieSceneSpawnable.GetGuid());
			break;
		}
	}

	if (!Track)
		return Result;
	TArray<UMovieSceneSection*> SectionArray = Track->GetAllSections();
	TArrayView<FMovieSceneFloatChannel*> ProxyFloatChannels;
	if (SectionArray.Num() < 1)
		return Result;
	UMovieSceneSection* Section = SectionArray[0];
	FMovieSceneChannelProxy& ChannelProxy = Section->GetChannelProxy();
	TArray<int32> AllTimes = GetAllKeyTimes(MovieScene, ChannelProxy);
	Result = GetProertyStr(MovieScene, AllTimes, ChannelProxy);
	return Result;
}

FString FCameraLinkTransformThread::GetProertyStr(UMovieScene* MovieScene, TArray<int32> AllTimes, FMovieSceneChannelProxy& ChannelProxy)
{
	FString Result = "";
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	for (int32 i = 0; i < AllTimes.Num(); i++)
	{
		TSharedPtr<FJsonObject> FrameData = MakeShareable(new FJsonObject);
		for (int32 j = 0; j < ChannelProxy.GetChannels<FMovieSceneFloatChannel>().Num(); j++)
		{
			TMovieSceneChannelHandle<FMovieSceneFloatChannel> ChannelHandle = ChannelProxy.MakeHandle<FMovieSceneFloatChannel>(j);
			const FName ChannelName = ChannelHandle.GetMetaData()->Name;
			FMovieSceneFloatChannel* FloatChannel = ChannelHandle.Get();
			TArray<FFrameNumber> OutTimes;
			TArray<FKeyHandle> OutKeys;
			FloatChannel->GetKeys(TRange<FFrameNumber>(), &OutTimes, &OutKeys);
			if (OutKeys.Num() < 1)
				continue;
			TMovieSceneChannelData<FMovieSceneFloatValue> ChannelData = FloatChannel->GetData();
			int32 ValueIndex = ChannelData.GetIndex(OutKeys[i]);
			FMovieSceneFloatValue FloatValue = ChannelData.GetValues()[ValueIndex];
			FrameData->SetNumberField(ChannelName.ToString(), FloatValue.Value);
		}
		JsonObject->SetObjectField(FString::FromInt(AllTimes[i]), FrameData);
	}
	FString OutJsonData;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Result);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	return Result;
}

TArray<int32> FCameraLinkTransformThread::GetAllKeyTimes(UMovieScene* MovieScene, FMovieSceneChannelProxy& ChannelProxy)
{
	TArray<int32> AllTimes;
	TArrayView<FKeyHandle> AllKeyHandles;

	for (int32 i = 0; i < ChannelProxy.GetChannels<FMovieSceneFloatChannel>().Num(); i++)
	{

		TMovieSceneChannelHandle<FMovieSceneFloatChannel> ChannelHandle = ChannelProxy.MakeHandle<FMovieSceneFloatChannel>(i);
		const FName ChannelName = ChannelHandle.GetMetaData()->Name;
		FMovieSceneFloatChannel* FloatChannel = ChannelHandle.Get();
		TArray<FFrameNumber> OutTimes;
		TArray<FKeyHandle> OutKeys;
		FloatChannel->GetKeys(TRange<FFrameNumber>(), &OutTimes, &OutKeys);
		for (FFrameNumber& KeyTime : OutTimes)
		{
			FFrameRate Rate1 = MovieScene ? MovieScene->GetTickResolution() : FFrameRate();
			FFrameRate Rate2 = MovieScene ? MovieScene->GetDisplayRate() : FFrameRate();
			FFrameTime DisplayRateTime = FFrameRate::TransformTime(KeyTime, Rate1, Rate2);
			AllTimes.Add(DisplayRateTime.GetFrame().Value);
		}
	}
	TSet<int32> Temp(AllTimes);
	AllTimes = Temp.Array();
	AllTimes.Sort();
	return AllTimes;
}

FString FCameraLinkTransformThread::GetCameraPropertyJson(TArray<double> PropertyList)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> LocationData;
	LocationData.Add(MakeShareable(new FJsonValueNumber(PropertyList[0])));
	LocationData.Add(MakeShareable(new FJsonValueNumber(PropertyList[1])));
	LocationData.Add(MakeShareable(new FJsonValueNumber(PropertyList[2])));

	TArray<TSharedPtr<FJsonValue>> RotationData;
	RotationData.Add(MakeShareable(new FJsonValueNumber(PropertyList[3])));
	RotationData.Add(MakeShareable(new FJsonValueNumber(PropertyList[4])));
	RotationData.Add(MakeShareable(new FJsonValueNumber(PropertyList[5])));

	JsonObject->SetArrayField("Location", LocationData);
	JsonObject->SetArrayField("Rotation", RotationData);
	FString OutJsonData;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonData);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	FString SendResult = OutJsonData.Replace(TEXT("\n"), TEXT("")).Replace(TEXT("\t"), TEXT("")).Replace(TEXT("\r"), TEXT("")).Replace(TEXT("\\"), TEXT(""));
	return SendResult;
}

void FCameraLinkTransformThread::PreRun()
{
	UCineCameraComponent* Component = ServiceData->CineCameraActor->GetCineCameraComponent();
	float FocalLength = Component->CurrentFocalLength;
	float FOVAngle = 0.0f;
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> FocalLengthData;
	FocalLengthData.Add(MakeShareable(new FJsonValueNumber(FocalLength)));
	TArray<TSharedPtr<FJsonValue>> SensorData;
#if ENGINE_MAJOR_VERSION > 4 
	SensorData.Add(MakeShareable(new FJsonValueNumber(Component->Filmback.SensorWidth)));
	SensorData.Add(MakeShareable(new FJsonValueNumber(Component->Filmback.SensorHeight)));

#elif ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 21
	SensorData.Add(MakeShareable(new FJsonValueNumber(Component->Filmback.SensorWidth)));
	SensorData.Add(MakeShareable(new FJsonValueNumber(Component->Filmback.SensorHeight)));

#else
	SensorData.Add(MakeShareable(new FJsonValueNumber(Component->FilmbackSettings.SensorWidth)));
	SensorData.Add(MakeShareable(new FJsonValueNumber(Component->FilmbackSettings.SensorHeight)));
#endif
	if (Component->CurrentFocalLength <= 0.0f)
	{
		FOVAngle = 0.0f;
	}
	else
	{
		float OverscanFactor = 1.0;
		float OriginalFocalLength = 35.0;
		OriginalFocalLength = Component->CurrentFocalLength;
#if ENGINE_MAJOR_VERSION < 5 && ENGINE_MINOR_VERSION < 27
		const float OverscanSensorWidth = Component->FilmbackSettings.SensorWidth * OverscanFactor;
#else
		const float OverscanSensorWidth = Component->Filmback.SensorWidth * OverscanFactor;
#endif
		FOVAngle = FMath::RadiansToDegrees(2.0f * FMath::Atan(OverscanSensorWidth / (2.0f * OriginalFocalLength)));
	}
	TArray<TSharedPtr<FJsonValue>> FOVData;
	FOVData.Add(MakeShareable(new FJsonValueNumber(FOVAngle)));
	JsonObject->SetArrayField("FocalLength", FocalLengthData);
	JsonObject->SetArrayField("SensorSize", SensorData);
	JsonObject->SetArrayField("FOV", FOVData);

	FString OutJsonData;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonData);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	FString SendResult = OutJsonData.Replace(TEXT("\n"), TEXT("")).Replace(TEXT("\t"), TEXT("")).Replace(TEXT("\r"), TEXT("")).Replace(TEXT("\\"), TEXT(""));
	TCHAR* seriallizedChar = SendResult.GetCharArray().GetData();
	int32 size = FCString::Strlen(seriallizedChar);
	int32 Sent = 0;
	bool bConnectState = Socket->Send((uint8*)TCHAR_TO_UTF8(seriallizedChar), size * sizeof(TCHAR), Sent);
}