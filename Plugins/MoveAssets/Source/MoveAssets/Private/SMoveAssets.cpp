// Fill out your copyright notice in the Description page of Project Settings.


#include "SMoveAssets.h"
#include "AssetSelection.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "SCachedAssetIconState.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Layout/SUniformGridPanel.h"


void SMoveAssets::Construct(const FArguments& InArgs)
{ 
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(30,10)
		[
			SAssignNew(NewFolderNameTextBox, SEditableTextBox)
			.HintText(FText::FromString(TEXT("Enter New Folder Name")))
		]
		+ SVerticalBox::Slot()
		.MaxHeight(70)
		.MinHeight(70)
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
		.AutoHeight()
		.VAlign(VAlign_Bottom)
		[
		SNew(SExpandableArea)
			.AreaTitle(FText::FromString("Advanced Options"))
			.BodyContent()
			[
				SNew(SGridPanel)
				.FillColumn(1, .3f)
				.FillColumn(0, .3f)
				+ SGridPanel::Slot(0,0)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("FilledBorder"))
					.BorderBackgroundColor(FLinearColor::Gray)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.Padding(5,3)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Save Assets on Move")) 
						]
					] 
				]
				+ SGridPanel::Slot(1,0)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("FilledBorder"))
					.BorderBackgroundColor(FLinearColor::Gray)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.Padding(5,3)
						[
							SNew(SCheckBox)
							.IsChecked(ECheckBoxState::Checked)
							.OnCheckStateChanged_Lambda([this] (ECheckBoxState State) -> void
							{
								bIsAutoSavingAssets = State == ECheckBoxState::Checked;
							}) 
						]
					]

				] 
				+ SGridPanel::Slot(0,1)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("FilledBorder"))
					.BorderBackgroundColor(FLinearColor::Gray)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.Padding(5,3)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Delete Redirectors on Move")) 
						]
					]

				]
				+ SGridPanel::Slot(1,1)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("FilledBorder"))
					.BorderBackgroundColor(FLinearColor::Gray)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.Padding(5,3)
						[ 
							SNew(SCheckBox)
							.IsChecked(ECheckBoxState::Checked)
							.OnCheckStateChanged_Lambda([this] (ECheckBoxState State) -> void 
							{
								bIsAutoRemovingRedirectors = State == ECheckBoxState::Checked;
							}) 
						]
					]

				]
				+ SGridPanel::Slot(0,2)
				[
					SNew(SBox)
					[
						SNew(SButton)
					   .VAlign(VAlign_Center)
					   .HAlign(HAlign_Center)
					   .Text(FText::FromString("Select Assets"))
						.ToolTipText(FText::FromString("Add assets to the next move operation")) 
					   .OnClicked_Lambda([this]
					   {
						   OnCachedSelectedAssets();
						   return FReply::Handled();
					   }) 
					] 

				]
				+ SGridPanel::Slot(1,2)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("FilledBorder"))
					.BorderBackgroundColor(FLinearColor::Gray)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.Padding(5,3)
						.HAlign(HAlign_Left)
						[
							SAssignNew(SelectedAssetsNumText, STextBlock)
							.Text(FText::FromString("Assets Selected: "))
						] 
					] 
				]
				+ SGridPanel::Slot(0,3)
				.ColumnSpan(2)
				[ 
					SNew(SBox)
					[
						SAssignNew(CacheDestinationPathButton, SButton)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.Text(FText::FromString("Select Path"))
						.OnClicked_Lambda([this]
						{
							OnCachedDestinationPath();
							return FReply::Handled();
						}) 
					] 
				]
			]
		]
	];

}

SMoveAssets::SMoveAssets() :
	ContentBrowserModule(FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser")),
	AssetRegistryModule(FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry")),
	AssetToolsModule(FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools"))
{  
	SuccesfullMoveOperation = FOnMoveOperation::CreateLambda([this] ()
	{
		CachedSelectedAssets.Empty();
		CachedDestinationPath.Empty();
		SelectedAssetsNumText->SetText(FText::FromString("Assets Selected: " + FString::FromInt(CachedSelectedAssets.Num())));
	});

	
	FailedMoveOperation = FOnMoveOperation::CreateLambda([this] ()
	{
		CachedSelectedAssets.Empty();
		CachedDestinationPath.Empty();
		SelectedAssetsNumText->SetText(FText::FromString("Assets Selected: " + FString::FromInt(CachedSelectedAssets.Num()))); 
	});
	 
}

bool SMoveAssets::MakeFolder(FString NewPath) const
{ 
	if (!AssetRegistryModule.Get().AddPath(NewPath))
	{ 
		return false;
	}
	
	return true;
}

bool SMoveAssets::UpdateRefrencers(FString& Path) const
{ 
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
	
	FEditorFileUtils::SaveDirtyPackages(!bIsAutoSavingAssets, true, true);
	ERedirectFixupMode FixupMode = bIsAutoRemovingRedirectors ? ERedirectFixupMode::DeleteFixedUpRedirectors : ERedirectFixupMode::PromptForDeletingRedirectors;
	AssetTools.FixupReferencers(Redirectors, bIsAutoSavingAssets, FixupMode);
	return true;
}

void SMoveAssets::MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, const FString& Path) const
{
	if (SelectedAssets.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Could not move assets\n\nAssets not selected or cached")) ;
		return;
	}

	if (Path.IsEmpty() || !AssetRegistryModule.Get().PathExists(Path))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Could not move assets\n\nGiven path does not exist")) ;
		return; 
	}
	
	TArray<UObject*> Assets;
 
	
	for (const FAssetData& AssetData : SelectedAssets)
	{ 
		if (UObject* Asset = AssetData.GetAsset())
			Assets.Add(Asset); 
	}
	
	AssetViewUtils::MoveAssets(Assets, Path);
	SuccesfullMoveOperation.ExecuteIfBound();
 }

void SMoveAssets::GetAssetDependencies(const FAssetData& AssetData, TArray<FName>& OutDependencies) const
{ 
	AssetRegistryModule.Get().GetDependencies(AssetData.PackageName, OutDependencies);

	for (FName& Dependency : OutDependencies)
		UE_LOG(LogTemp, Display, TEXT("%s"), *Dependency.ToString())

}

FReply SMoveAssets::OnSortAssetsButtonClicked() const
{
	TArray<FAssetData> SelectedAssets;
	FString NewFolderNameString = GetNewFolderName();
	FString ToPath = SelectDestinationPath();
	FString FromPath = ToPath;
	
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	SelectedAssets.Append(CachedSelectedAssets.Array());
	
	TMap<FName, TArray<FAssetData>> AssetTypes;

	if (NewFolderNameString.Len() > 0)
	{
		if (MakeFolder(FromPath + "/" + NewFolderNameString))
			ToPath += "/" + NewFolderNameString; 
	}


	if (SelectedAssets.Num() <= 0)
	{ 
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to sort assets, select assets before commencing operation")) ;
		FailedMoveOperation.ExecuteIfBound();
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
		MakeFolder(UniqueToPath);
		MoveAssetsTo(Element.Value, UniqueToPath); 
		UpdateRefrencers(FromPath); 
	}
	

	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Sorted " + FString::FromInt(SelectedAssets.Num()) + " assets to " + FString::FromInt(AssetTypes.Num()) + " unique folders at " + SelectDestinationPath())) ;
	
	SuccesfullMoveOperation.ExecuteIfBound();

	return FReply::Handled(); 
}

// need to handle case where you try an incorrect move and give an appropriate err msg
FReply SMoveAssets::OnMoveToSelectedFolderClicked() const
{
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	SelectedAssets.Append(CachedSelectedAssets.Array());
	FString NewFolderName = GetNewFolderName();
	FString DestinationPath = SelectDestinationPath();
	FString ToPath =  NewFolderName.IsEmpty() ? DestinationPath : DestinationPath + "/" + NewFolderName;

	if (!NewFolderName.IsEmpty())
		MakeFolder(ToPath);
	MoveAssetsTo(SelectedAssets, ToPath);
	UpdateRefrencers(DestinationPath);
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Moved " + FString::FromInt(SelectedAssets.Num()) + " assets to " + ToPath));
	
	return FReply::Handled();
}

FReply SMoveAssets::OnCachedSelectedAssets()
{
	TArray<FAssetData> SelectedAssets;
	TSet<FAssetData> SelectedAssetsSet;
	
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets); 
	SelectedAssetsSet.Append(SelectedAssets); 
	
	TSet<FAssetData> Intersection = CachedSelectedAssets.Intersect(SelectedAssetsSet); 
	CachedSelectedAssets.Append(SelectedAssets);
	for (const FAssetData& Asset : Intersection)
		CachedSelectedAssets.Remove(Asset);

	SelectedAssetsNumText->SetText(FText::FromString("Assets Selected: " + FString::FromInt(CachedSelectedAssets.Num())));

	return FReply::Handled();
}
 
FReply SMoveAssets::OnCachedDestinationPath()
{
	TArray<FString> SelectedPaths;
	ContentBrowserModule.Get().GetSelectedFolders(SelectedPaths);
	
	if (SelectedPaths.IsEmpty())
		ContentBrowserModule.Get().GetSelectedPathViewFolders(SelectedPaths);

	FString NewDestinationPath = SelectedPaths.Last(); 
	NewDestinationPath.RemoveFromStart("/All");
	CachedDestinationPath = NewDestinationPath; 

	CacheDestinationPathButton.Get()->SetToolTipText(FText::FromString("Selected Path: " + CachedDestinationPath));
	
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Cached new destination: " + SelectDestinationPath())); 

	return FReply::Handled();
}
  

FString SMoveAssets::SelectDestinationPath() const
{
	FString CurrentPath = ContentBrowserModule.Get().GetCurrentPath().GetVirtualPathString();
	CurrentPath.RemoveFromStart("/All");
	return CachedDestinationPath.IsEmpty() ? CurrentPath : CachedDestinationPath;
}

// Gets folder name form input box and sanitizes input
FString SMoveAssets::GetNewFolderName() const
{
	FString NewFileName = NewFolderNameTextBox.Get()->GetText().ToString();
	if (NewFileName.IsEmpty())
		return NewFileName;
	
	const FRegexPattern HangingSlashPattern(TEXT(R"(^(/+)?(.*?)(/+)?$)")); 
	FRegexMatcher HangingSlashMatcher(HangingSlashPattern, NewFileName);
 
	if (HangingSlashMatcher.FindNext())
	{
		FString Leading = HangingSlashMatcher.GetCaptureGroup(1);
		FString Trailing = HangingSlashMatcher.GetCaptureGroup(3);

		if (!Leading.IsEmpty())
			NewFileName.RemoveFromStart(Leading);
		if (!Trailing.IsEmpty())
			NewFileName.RemoveFromEnd(Trailing);
	}
	
	while (NewFileName.Contains(TEXT("//")))
		NewFileName = NewFileName.Replace(TEXT("//"), TEXT("/"));
	

	return NewFileName;
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

