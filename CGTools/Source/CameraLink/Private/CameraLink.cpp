#include "CameraLink.h"
#include "CoreUObject.h"
#include "Editor.h"
#include "Engine/SceneCapture2D.h"
#include "CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Factories/TextureRenderTargetFactoryNew.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "CameraLinkBPLib.h"
#if ENGINE_MAJOR_VERSION >=5
#include "Subsystems/EditorAssetSubsystem.h"
#include "Subsystems/EditorActorSubsystem.h"
#endif
#include "EditorLevelLibrary.h"
#include "Widgets/Docking/SDockTab.h"
#include "Templates/SharedPointer.h"
#include "Interfaces/IPluginManager.h"
#include "Brushes/SlateImageBrush.h"
#include "Styling/SlateStyle.h"
#include "UMGStyle.h"
#include "Styling/SlateStyleRegistry.h"
#define LOCTEXT_NAMESPACE "FCameraLinkModule"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
const FVector2D Icon128x128(128.0f, 128.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon32x32(32.0f, 32.0f);

void FCameraLinkModule::StartupModule()
{
	RegisterSlateStyle();
	IModuleInterface::StartupModule();
	CameraLinkerPtr = MakeShareable(new FCameraLinker);
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, FOnSpawnTab::CreateRaw(this, &FCameraLinkModule::SpawnCameraLinkTab))
		.SetDisplayName(FText::FromString("CameraLink"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	if (GEngine)
		GEngine->OnLevelActorDeleted().AddRaw(this, &FCameraLinkModule::OnActorDestroyedCallback);
	CameraLinkerPtr->OnDestroyDelegate.AddRaw(this, &FCameraLinkModule::ClearTree);
}


void FCameraLinkModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

TSharedRef<SDockTab> FCameraLinkModule::SpawnCameraLinkTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> Tab = SNew(SDockTab).TabRole(ETabRole::NomadTab);
	if (CameraLinkEditor.IsValid())
	{
		Tab->SetContent(CameraLinkEditor.ToSharedRef());
	}
	else
	{
		Tab->SetContent(SAssignNew(CameraLinkEditor, SCameraLink, CameraLinkerPtr->AsShared()));
	}
	return Tab;
}

void FCameraLinkModule::OpenEditor()
{
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION >= 27
	FGlobalTabmanager::Get()->TryInvokeTab(TabName);
#else
	FGlobalTabmanager::Get()->InvokeTab(TabName);
#endif
}

void FCameraLinkModule::OnActorDestroyedCallback(AActor* CameraActor)
{

	if (CameraLinkerPtr->ServiceThreadImpl->CurrentThreadData)
	{
	
		ACineCameraActor* DestroyedActor = Cast<ACineCameraActor>(CameraActor);
		if (DestroyedActor == CameraLinkerPtr->ServiceThreadImpl->CurrentThreadData->CineCameraActor)
		{
			CameraLinkerPtr->ServiceInit();
			CameraLinkerPtr->OnDestroyDelegate.Broadcast();
			CameraLinkEditor->ClearTree();
			CameraLinkerPtr->ServiceThreadImpl->SetData(nullptr);
		}
	}
}

void FCameraLinkModule::ClearTree()
{
	CameraLinkEditor->ClearTree();
}

void FCameraLinkModule::RegisterSlateStyle()
{
	Style = MakeShareable(new FSlateStyleSet("CameraLinkSlateStyle"));
#if ENGINE_MAJOR_VERSION > 4
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("CGTools")->GetBaseDir() / TEXT("Resources"));
#else
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("TAPython")->GetBaseDir() / TEXT("Resources"));
#endif
	Style->Set("CameraLinkSlateStyle.CloseServiceIcon", new IMAGE_BRUSH(TEXT("CloseService"), Icon32x32));
	Style->Set("CameraLinkSlateStyle.StartServiceIcon", new IMAGE_BRUSH(TEXT("StartService"), Icon32x32));
	Style->Set("CameraLinkSlateStyle.SettingIcon_32", new IMAGE_BRUSH(TEXT("Setting_32"), Icon32x32));
	FSlateStyleRegistry::RegisterSlateStyle(*Style.Get());
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCameraLinkModule, CameraLink)