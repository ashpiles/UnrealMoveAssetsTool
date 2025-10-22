// Fill out your copyright notice in the Description page of Project Settings.


#include "SMoveAssets.h"


#include "AssetDefinitionRegistry.h"
#include "AssetDefinition.h"
#include "AssetSelection.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "AssetViewWidgets.h"
#include "ContentBrowserModule.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "SCachedAssetIconState.h"
#include "AssetRegistry/AssetRegistryModule.h"




void SMoveAssets::Construct(const FArguments& InArgs)
{ 
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	FAssetViewExtraStateGenerator StateGenerator(
		FOnGenerateAssetViewExtraStateIndicators::CreateSP(this, &SMoveAssets::GenerateMoveAssetIconState),
		FOnGenerateAssetViewExtraStateIndicators()
		);
	
	ContentBrowserModule.AddAssetViewExtraStateGenerator(StateGenerator);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(30,10)
		[
			SAssignNew(NewFolderName, SEditableTextBox)
			.HintText(FText::FromString(TEXT("Enter New Folder Name")))
		]
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SBox)
				.Padding(20,5)
				.MinDesiredHeight(70)
				.MinDesiredWidth(200)
				[
					SNew(SButton)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Text(FText::FromString("Move"))
					.OnClicked(this, &SMoveAssets::OnMoveToSelectedFolderClicked)
				]
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SBox)
				.Padding(20,5)
				.MinDesiredHeight(70)
				.MinDesiredWidth(200)
				[
					SNew(SButton)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Text(FText::FromString("Move & Sort"))
					.OnClicked(this, &SMoveAssets::OnSortAssetsButtonClicked)
				]
			] 
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoHeight()
		[
			SNew(SBox)
			.Padding(20,5)
			.MinDesiredHeight(20)
			.MinDesiredWidth(30)
			[
				
				SNew(SComboButton)
				.MenuPlacement(MenuPlacement_BelowAnchor)
				.CollapseMenuOnParentFocus(false)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Advanced Options"))
				]
				.MenuContent()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(SBorder)
						.BorderBackgroundColor(FSlateColor(FLinearColor(.3f, .3f, .3f, 1.f)))
						.Padding(2)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("Border"))
							.BorderBackgroundColor(FSlateColor(FLinearColor::Black))
							[ 
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(5, 0, 0, 0)
								[
									SNew(SCheckBox)
									.IsChecked(ECheckBoxState::Checked)
									.OnCheckStateChanged_Lambda([this] (ECheckBoxState State) -> void
									{
										bIsAutoSavingAssets = State == ECheckBoxState::Checked;
									})
								]
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(5, 0, 5, 0)
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString("Auto Save Assets"))
								] 
							]
						]
					]
					+ SVerticalBox::Slot()
					[
						SNew(SBorder)
						.BorderBackgroundColor(FSlateColor(FLinearColor(.3f, .3f, .3f, 1.f)))
						.Padding(2)
						[ 
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("Border"))
							.BorderBackgroundColor(FSlateColor(FLinearColor::Black))
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(5, 0, 0, 0)
								[ 
									SNew(SCheckBox)
									.IsChecked(ECheckBoxState::Checked)
									.OnCheckStateChanged_Lambda([this] (ECheckBoxState State) -> void 
									{
										bIsAutoRemovingRedirectors = State == ECheckBoxState::Checked;
									})
								]
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(5, 0, 5, 0)
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString("Auto Delete Redirectors")) 
								]
							] 
						]
						
					]
					+ SVerticalBox::Slot()
					[
						SNew(SBorder)
						.BorderBackgroundColor(FSlateColor(FLinearColor(.3f, .3f, .3f, 1.f)))
						.Padding(2)
						[ 
							SNew(SButton)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.Text(FText::FromString("Cache Selected Assets"))
							.OnClicked_Lambda([this]
							{
								OnCachedSelectedAssets();
								return FReply::Handled();
							})
						]
					]
					+ SVerticalBox::Slot()
					[
						SNew(SBorder)
						.BorderBackgroundColor(FSlateColor(FLinearColor(.3f, .3f, .3f, 1.f)))
						.Padding(2)
						[ 
							SNew(SButton)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.Text(FText::FromString("Cache Selected Path"))
							.OnClicked_Lambda([this]
							{
								OnCachedDestinationPath();
								return FReply::Handled();
							})
						]
					]
				]
			]
		]
	];
		


} 

bool SMoveAssets::MakeFolder(FString NewPath, bool bSkipErrorMessage = false) const
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	if (!AssetRegistryModule.Get().AddPath(NewPath) && !bSkipErrorMessage)
	{ 
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to make path at " + NewPath)) ;
		return false;
	}
	
	return true;
}

bool SMoveAssets::UpdateRefrencers(FString& Path) const
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	
	IAssetTools& AssetTools = AssetToolsModule.Get();
	FARFilter Filter;
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(FName(Path));

	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	TArray<UObjectRedirector*> Redirectors;
	for (const FAssetData& AssetData : AssetList)
	{
		if (AssetData.IsRedirector())
		{
			Redirectors.Add(Cast<UObjectRedirector>(AssetData.GetAsset()));
		}
	}

	if (Redirectors.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateRefrences - no Refrences to update"));
		return false;
	}
	FEditorFileUtils::SaveDirtyPackages(true, true, true);
	ERedirectFixupMode FixupMode = bIsAutoRemovingRedirectors ? ERedirectFixupMode::DeleteFixedUpRedirectors : ERedirectFixupMode::PromptForDeletingRedirectors;
	AssetTools.FixupReferencers(Redirectors, bIsAutoSavingAssets, FixupMode);
	return true;
}

void SMoveAssets::MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, FString Path) const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<UObject*> Assets;
	TArray<FAssetData> NewSelectedAssets;
 
	
	for (const FAssetData& AssetData : SelectedAssets)
	{ 
		if (UObject* Asset = AssetData.GetAsset())
		{
			Assets.Add(Asset);
			NewSelectedAssets.Add(FAssetData(Asset, FAssetData::ECreationFlags::None)); 
		}
		else
			NewSelectedAssets.Add(AssetData); 
	}
	AssetViewUtils::MoveAssets(Assets, Path);

	// Remove Check mark Icons
	for (auto GeneratorHandle : AssetViewGeneratorHandles)
		ContentBrowserModule.RemoveAssetViewExtraStateGenerator(GeneratorHandle);
 }

void SMoveAssets::GetAssetDependencies(const FAssetData& AssetData, TArray<FName>& OutDependencies) const
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	AssetRegistryModule.Get().GetDependencies(AssetData.PackageName, OutDependencies);

	for (FName& Dependency : OutDependencies)
		UE_LOG(LogTemp, Display, TEXT("%s"), *Dependency.ToString())

}

FReply SMoveAssets::OnSortAssetsButtonClicked() const
{
	TArray<FAssetData> SelectedAssets;
	FString ToPath = SelectDestinationPath();
	FString FromPath = ToPath;
	
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	SelectedAssets.Append(CachedSelectedAssets.Array());
	
	TMap<FName, TArray<FAssetData>> AssetTypes;

	if (MakeFolder(FromPath + "/" + NewFolderName.Get()->GetText().ToString()))
	{
		ToPath += "/" + NewFolderName.Get()->GetText().ToString();
	}

	if (SelectedAssets.Num() <= 0)
	{ 
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to sort assets, cache selected assets before commencing operation")) ;
		return FReply::Handled();
	}

	for (const FAssetData& Asset : SelectedAssets )
	{
		FName AssetName = Asset.AssetClassPath.GetAssetName();
		AssetTypes.FindOrAdd(AssetName).Add(Asset);
	}
	for (auto Element : AssetTypes )
	{ 
		FString UniqueToPath = ToPath + "/" + Element.Key.ToString() + "s";
		MakeFolder(UniqueToPath, true);
		MoveAssetsTo(Element.Value, UniqueToPath); 
		UpdateRefrencers(FromPath); 
	}
	
	
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Sorted " + FString::FromInt(CachedSelectedAssets.Num()) + " assets to " + FString::FromInt(AssetTypes.Num()) + " unique folders at " + SelectDestinationPath())) ;

	return FReply::Handled(); 
}

FReply SMoveAssets::OnMoveToSelectedFolderClicked() const
{
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	SelectedAssets.Append(CachedSelectedAssets.Array());
	FString FromPath = SelectDestinationPath();
	FString ToPath = SelectDestinationPath() + "/" + NewFolderName.Get()->GetText().ToString();
	if (MakeFolder(ToPath))
	{
		MoveAssetsTo(SelectedAssets, ToPath);
		UpdateRefrencers(FromPath);
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Moved " + FString::FromInt(CachedSelectedAssets.Num()) + " assets to " + SelectDestinationPath()));
		
	}
	
	return FReply::Handled();
}

FReply SMoveAssets::OnCachedSelectedAssets()
{
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	CachedSelectedAssets.Append(SelectedAssets);

	return FReply::Handled();
}
 
FReply SMoveAssets::OnCachedDestinationPath()
{
	TArray<FString> SelectedPaths;
	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowser.Get().GetSelectedFolders(SelectedPaths);
	if (SelectedPaths.IsEmpty())
		ContentBrowser.Get().GetSelectedPathViewFolders(SelectedPaths);

	if (!SelectedPaths.IsEmpty())
	{ 
		CachedDestinationPath = SelectedPaths.Last();
	}


	return FReply::Handled();
}
  

FString SMoveAssets::SelectDestinationPath() const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString CurrentPath = ContentBrowserModule.Get().GetCurrentPath().GetVirtualPathString();
	CurrentPath.RemoveFromStart("/All");
	return CachedDestinationPath.IsEmpty() ? CachedDestinationPath : CurrentPath;
}

TSharedRef<SWidget> SMoveAssets::GenerateMoveAssetIconState(const FAssetData& AssetData) const
{
	return SNew(SCachedAssetIconState)
		.AssetData(AssetData)
		.OnAssetCached(FOnAssetCachedListner::CreateRaw(this, &SMoveAssets::OnAssetSelected)); 
}

bool SMoveAssets::OnAssetSelected(FAssetData InAssetData) const
{
	return CachedSelectedAssets.Contains(InAssetData);
}

