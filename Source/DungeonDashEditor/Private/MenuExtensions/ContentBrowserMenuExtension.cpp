// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuExtensions/ContentBrowserMenuExtension.h" 

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilityWidget/MoveAssetsWidget.h"
#include "WidgetMenuSettings/EditorUtilityMenuSettings.h"

#define LOCTEXT_NAMESPACE "DungeonDashEditor"


FContentBrowserMenuExtension::FContentBrowserMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenderDelegates = ContentBrowserModule. GetAllAssetViewContextMenuExtenders();
	MenuExtenderDelegates.Add( FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FContentBrowserMenuExtension::MenuExtensionDelegate));
	MoveAssetsWidget = MakeShared<FMoveAssetsWidget>();
}


void FContentBrowserMenuExtension::AddWidgetEntries(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString("Move Assets"),
		FText::FromString("Moves assets to a new folder at desired path"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(MoveAssetsWidget.ToSharedRef(), &FMoveAssetsWidget::MakeWidget))
		); 
}

// better name to explain its responsibilities
TSharedRef<FExtender> FContentBrowserMenuExtension::MenuExtensionDelegate(const TArray<FAssetData>& Assets)
{ 
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MoveAssetsWidget->SetCachedSelectedAssets(Assets);

	MenuExtender->AddMenuExtension(
		"CommonAssetActions",
		EExtensionHook::After,
		TSharedPtr<FUICommandList>(),
		FMenuExtensionDelegate::CreateSP(this, &FContentBrowserMenuExtension::AddWidgetEntries)
		);
	return MenuExtender.ToSharedRef();
}





#undef LOCTEXT_NAMESPACE
