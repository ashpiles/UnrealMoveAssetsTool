// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtilityWidget/MoveAssetsWidget.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "SAssetSearchBox.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/PathTree.h"



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
								.OnClicked_Lambda([this]()
								{
								})
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
								.OnTextCommitted_Lambda([this](FString Path) {})
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
	int Distance = LevenshteinDistance(Path, "/Game/Test/");
	UE_LOG(LogTemp, Warning, TEXT("%d"), Distance);
	TArray<UObject*> Assets;
	for (auto AssetData : SelectedAssets)
	{
		Assets.Add(AssetData.GetAsset());
	}
	AssetViewUtils::MoveAssets(Assets, Path);
	UpdateRefrencers(Path); 
}

bool FMoveAssetsWidget::bPathExists(FString Path)
{
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	return AssetRegistryModule.Get().PathExists(Path);
}



// problem: Need to find the closest matching string
// subproblems:
/*
 *  Find the longest matching suffix
 *  Given the set of paths we simply
 */

int FMoveAssetsWidget::LevenshteinDistance(const FString String1, const FString String2)
{
	// Declare and set matrix
	TArray<TArray<int>> DistanceMatrix;
	TArray<int> MatrixRow;
	MatrixRow.Init(0, String2.Len() + 1);
	DistanceMatrix.Init(MatrixRow, String1.Len() + 1);

	for (int i = 0; i < String1.Len(); i++)
		DistanceMatrix[i][0] = i;

	for (int j = 0; j < String2.Len(); j++)
		DistanceMatrix[0][j] = j;



	for (int j = 1; j < String2.Len(); j++)
	{
		for (int i = 1; i < String1.Len(); i++)
		{
			int SubstitutionCost = (String1[i] == String2[j]) ? 0 : 1;

			int Deletion = DistanceMatrix[i-1][j] + 1;
			int Insertion = DistanceMatrix[i][j - 1] + 1;
			int Substitution = DistanceMatrix[i-1][j-1] + SubstitutionCost;

			DistanceMatrix[i][j] = FMath::Min3(Deletion, Insertion, Substitution);
		}
	}
	return DistanceMatrix[String1.Len()][String2.Len()];

}

// find the lcs and then perform levenshtein distance
void FMoveAssetsWidget::PathsOfSharingSuffix(const FString& Path)
{
	FPathTree PathTree = FPathTree();
	TSet<FName> OutPaths;


	// index paths
	// Trie with whole words as nodes!!!!!!!
	// You get a trie node by aproxximating input word via levensthein distance
	// Nodes point to set
	// the nodes are an element of SetA with the whole branch being the SetA

	
}
