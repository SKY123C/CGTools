// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "LevelSequence.h"
#include "CineCameraComponent.h"
#include "CameraLinkRTClient.h"
#include "CameraLinkTransfromClient.h"
#include "Engine/SceneCapture2D.h"
#include "Editor/CameraLinker.h"
#include "Editor/SCameraLink.h"


class CAMERALINK_API FCameraLinkModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	int32 RTWidth = 512;
	int32 RTHeight = 512;
	TSharedRef<SDockTab> SpawnCameraLinkTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<FCameraLinker> CameraLinkerPtr;
	void OpenEditor();
	void OnActorDestroyedCallback(AActor* CameraActor);
	void RegisterSlateStyle();
	TSharedPtr<class FSlateStyleSet> Style;
	void ClearTree();
	TSharedPtr<FCameraLinker> GetCameraLinker()
	{
		return CameraLinkerPtr;
	}
private:
	FName TabName = "TACameraLink";
	TSharedPtr<SCameraLink> CameraLinkEditor;

};