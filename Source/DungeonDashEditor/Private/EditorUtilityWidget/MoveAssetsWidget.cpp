// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtilityWidget/MoveAssetsWidget.h"


#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "SAssetSearchBox.h"
#include "AssetRegistry/AssetRegistryModule.h"


void FMoveAssetsWidget::MakeWidget()
{ 
	
	 FSlateApplication::Get().AddWindow( 
		SNew(SWindow)
		.Title(FText::FromString("Move Assets"))
		.ClientSize(FVector2D(550, 80))
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[ 
				SNew(SVerticalBox) 
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Center)
				.Padding(15, 0, 0, 0)
				[
					SNew(SCheckBox)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState CheckState)
					{
						WidgetSwitcher->SetActiveWidgetIndex(CheckState == ECheckBoxState::Checked ? 1 : 0);
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Center)
				.Padding(10, 0, 5, 3)
				[ 
					SNew(STextBlock)
					.Text(FText::FromString("Move to existing folder"))
					.ColorAndOpacity(FSlateColor(FColor(169, 169, 169, 100)))
				] 
						
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10, 0, 0, 0)
				.VAlign(VAlign_Center)
				[ 
						SAssignNew(WidgetSwitcher, SWidgetSwitcher)

							// Add assets to new folder at current path
						+ SWidgetSwitcher::Slot()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(0, 0, 15, 0)
							.MinWidth(125)
							[ 
								SNew(SButton)
								.OnClicked_Lambda([this]
								{
									return FReply::Handled();
								})
								.Text(FText::FromString("Create Folder"))
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
							]
							+ SHorizontalBox::Slot()
							.FillWidth(0.7f)
							.VAlign(VAlign_Center)
							.Padding(10, 0, 10, 0)
							[
								SNew(SEditableTextBox)
							]
						]
					
						// Fuzzy search for path to add assets to
						+SWidgetSwitcher::Slot()
						[ 
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(0, 0, 15, 0)
							.MinWidth(125)
							[ 
								SNew(SButton)
								.Text(FText::FromString("Move"))
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
							]
							+ SHorizontalBox::Slot()
							.FillWidth(0.7f)
							.VAlign(VAlign_Center)
							.Padding(10, 0, 10, 0)
							[
								SAssignNew(SearchBox, SAssetSearchBox)
								.OnTextChanged_Lambda([this] (const FText& Input)
								{
									for (auto String : FuzzySearch(Input.ToString(), 1, 0))
										UE_LOG(LogTemp, Display, TEXT("%s"), *String)

								})
							]
						] 
				] 
			]
		]
			
	);
}

bool FMoveAssetsWidget::MakeFolder(FString FolderPath)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	if (!AssetRegistryModule.Get().AddPath(FolderPath))
	{ 
		UE_LOG(LogTemp, Error, TEXT("Create Folder Failed - Failed to Add Path. '%s'"), *FolderPath);
		return false;
	}
	
	return true;
}

bool FMoveAssetsWidget::UpdateRefrencers(FString& Path)
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
	AssetTools.FixupReferencers(Redirectors);
	return true;
}
void FMoveAssetsWidget::MoveAssetsTo(TArray<FAssetData> SelectedAssets, FString Path)
{  
	TArray<UObject*> Assets;
	for (auto AssetData : SelectedAssets)
	{
		Assets.Add(AssetData.GetAsset());
	}
	AssetViewUtils::MoveAssets(Assets, Path);
	UpdateRefrencers(Path); 
}
  
 
 
void FMoveAssetsWidget::AddStringToBucket(const FString& Input, TArray<TArray<FPathNode>>& Buckets)
{

	FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry"); 
 
	ARM.Get().EnumerateAllCachedPaths([&Input, &Buckets](FName Path) -> bool
	{
		FString CurrentPath = Path.ToString(); 
		FPathNode CurrentNode;
		CurrentNode.Path = CurrentPath;
		CurrentNode.SearchString = Input;

		if (Buckets.Num() < CurrentNode.EditDistance())
		{
			Buckets.Reserve(CurrentNode.EditDistance() + 1);
		}
		Buckets[CurrentNode.EditDistance()].Add(CurrentNode);
		return true;
	}); 
}

// we lazily sort buckets based on where the user is on the search bar view
void FMoveAssetsWidget::SelectionSortFuzzySearchBucket(TArray<FPathNode>& Bucket)
{
	for (int i = 0; i < Bucket.Num() - 1; i++)
	{
		int MinIndex = i;
		for (int j = i+1; j < Bucket.Num(); j++)
		{
			if (Bucket[j].EditDistance() < Bucket[MinIndex].EditDistance())
				MinIndex = j;
		}

		if (MinIndex != i)
			Swap(Bucket[i], Bucket[MinIndex]); 
	}
}

// we have to somehow account for the input changing mid-process
TArray<FString> FMoveAssetsWidget::FuzzySearch(const FString& Input, int BatchSize, int StartIndex = 0)
{
	TArray<TArray<FPathNode>> PathBuckets;
	AddStringToBucket(Input, PathBuckets);
	TArray<FString> Results;
// do some checks to see if batch size and start index are valid
	for (int i = StartIndex; i < BatchSize; i++)
	{ 
		SelectionSortFuzzySearchBucket(PathBuckets[i]);
		for (int j = 0; j < PathBuckets[i].Num(); j++)
			Results.Add(PathBuckets[i][j].Path);
	}
	return Results;
}
