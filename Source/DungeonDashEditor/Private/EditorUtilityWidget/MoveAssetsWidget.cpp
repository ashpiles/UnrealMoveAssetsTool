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
	IndexPathStrings();
	
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
								.OnClicked_Lambda([]
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
								.OnTextChanged_Lambda([this] (const FText& Input)
								{
									UE_LOG(LogTemp, Warning, TEXT("---NEW SEARCH---"))
									TArray<FString> SearchResults;
									FuzzyFind(Input.ToString(), 3, SearchResults);
									for (auto String : SearchResults)
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

  
bool FMoveAssetsWidget::SegmentPathHeirachy(const FString& Input, TArray<FString>& OutArray)
{
	for (int i = 0; i < Input.Len(); i++)
	{
		if (Input[i] == '/' || OutArray.IsEmpty())
			OutArray.Add("/");
		else
			OutArray.Last().AppendChar(Input[i]);
	}
	return true;
}

void FMoveAssetsWidget::IndexPathStrings()
{
	FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	ARM.Get().GetAllCachedPaths(PathStrings);
	for (int i = 0; i < PathStrings.Num(); i++)
	{
		TArray<FString> OutArray;
		if (SegmentPathHeirachy(PathStrings[i], OutArray))
		{
			for (int j = 0; j < OutArray.Num(); j++)
			{
				NGramIndex.FindOrAdd(OutArray[j]).Add(i);
			}
		}
	}
}


void FMoveAssetsWidget::FuzzyFind(const FString& Input, int FuzzyRange, TArray<FString>& OutArray)
{
	TArray<FString> SegmentedInput;
	TSet<int> SpeculitiveRootPaths;
	SegmentPathHeirachy(Input, SegmentedInput);
	for (const auto& Element : NGramIndex)
	{
		int CompareResult = FMath::Abs(LevenshteinDistance(Element.Key,(SegmentedInput[0])));
		if (CompareResult <= FuzzyRange)
			SpeculitiveRootPaths.Append(NGramIndex[Element.Key]);
		// 0(n)
	}

	for (int i = 0; i < SpeculitiveRootPaths.Num(); i++)
	{
		// 0(n)
		if (FMath::Abs(LevenshteinDistance(Input, PathStrings[i])) <= FuzzyRange)
			OutArray.Add(PathStrings[i]);
	}
}
