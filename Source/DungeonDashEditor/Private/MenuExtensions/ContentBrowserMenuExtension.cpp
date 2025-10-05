// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuExtensions/ContentBrowserMenuExtension.h" 

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilityWidget/MoveAssetsWidget.h"
#include "WidgetMenuSettings/EditorUtilityMenuSettings.h"

#define LOCTEXT_NAMESPACE "DungeonDashEditor"


FContentBrowserMenuExtension::FContentBrowserMenuExtension(): SelectedAssets()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenderDelegates = ContentBrowserModule. GetAllAssetViewContextMenuExtenders();
	MenuExtenderDelegates.Add( FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FContentBrowserMenuExtension::MenuExtensionDelegate));
}


void FContentBrowserMenuExtension::AddWidgetEntriesFromEditor(FMenuBuilder& MenuBuilder)
{
//	const UEditorUtilityMenuSettings* WidgetSettings = GetDefault<UEditorUtilityMenuSettings>();
//		MenuBuilder.AddSubMenu(
//			WidgetSettings->Category,
//			WidgetSettings->CategoryToolTip,
//			FNewMenuDelegate::CreateLambda([WidgetSettings, this](FMenuBuilder& MenuBuilder)
//			{
//				for (const FWidgetEntry& WidgetEntry : WidgetSettings->EditorUtilityWidgets)
//				{
//					
//					UObject* LoadedFunctionality = WidgetEntry.MenuEntryFunctionalityPath.TryLoad();
//					if (!LoadedFunctionality)
//					{
//						UE_LOG(LogTemp, Warning, TEXT("FSubMenuData Construction - Failed to load Menu Entry Functionality"));
//					}
//
//					// need a way to cast this to the correct obj
//					UEditorUtilityWidgetBlueprint* EditorUtilityWidget = Cast<UEditorUtilityWidgetBlueprint>(LoadedFunctionality);
//					if (!EditorUtilityWidget)
//					{ 
//						UE_LOG(LogTemp, Warning, TEXT("FSubMenuData Construction - Failed to convert Menu Entry Functionality"));
//					}
//
//					// Execute or spawn obj
//					const FUIAction CreateWidget (FExecuteAction::CreateLambda([EditorUtilityWidget, this]()
//					{
//						if (UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
//						{
//							EditorUtilitySubsystem->SpawnAndRegisterTab(EditorUtilityWidget);
//							if (UMoveAssetsWidget* MoveAssetsWidget = Cast<UMoveAssetsWidget>(EditorUtilityWidget->GetCreatedWidget()))
//								MoveAssetsWidget->InData = SelectedAssets;
//							
//						}
// 
//					}));
//					MenuBuilder.AddMenuEntry(WidgetEntry.EntryLabel, WidgetEntry.EntryToolTip, FSlateIcon(), CreateWidget);
//				}
//			}
//		)
//	);
}

void FContentBrowserMenuExtension::AddWidgetEntries(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString("Move Assets"),
		FText::FromString("Moves assets to a new folder at desired path"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([]
		{
			FSlateApplication::Get().AddWindow( 
				SNew(SWindow)
				.Title(FText::FromString("Move Assets"))
				.ClientSize(FVector2D(600, 100))
				.SupportsMinimize(false)
				.SupportsMaximize(false)
				[
					SNew(SVerticalBox) 
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.Padding(10)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Move to existing folder"))
								.ColorAndOpacity(FSlateColor(FColor(169, 169, 169, 100)))
							]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(10, 0, 0, 0)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.Text(FText::FromString("Move"))
							.HAlign(HAlign_Center)
						]
						
					]
					+ SVerticalBox::Slot()
					.FillHeight(1.f)
					.Padding(10)
					[
						
						SNew(SMoveAssetsWidget)
					]
					
				]
			);
		}))
		); 
}

// better name to explain its responsibilities
TSharedRef<FExtender> FContentBrowserMenuExtension::MenuExtensionDelegate(const TArray<FAssetData>& Assets)
{ 
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	SelectedAssets = Assets;

	MenuExtender->AddMenuExtension(
		"CommonAssetActions",
		EExtensionHook::After,
		TSharedPtr<FUICommandList>(),
		FMenuExtensionDelegate::CreateSP(this, &FContentBrowserMenuExtension::AddWidgetEntries)
		);
	return MenuExtender.ToSharedRef();
}





#undef LOCTEXT_NAMESPACE
