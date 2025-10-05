// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathSearch.h"


class SAssetSearchBox;
/**
 * 
 */
class DUNGEONDASHEDITOR_API SMoveAssetsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMoveAssetsWidget)
		: _LabelText(FText::GetEmpty())
		{}
		SLATE_ARGUMENT(FText, LabelText)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	bool MakeFolder(FString FolderPath);
	bool UpdateRefrencers(FString& Path);
	void MoveAssetsTo(TArray<FAssetData> SelectedAssets, FString Path);

	bool bPathExists(FString Path);
 

	// this does not update when you select a new set of items
	TArray<FAssetData> InData; 

private:
	int LevenshteinDistance(const FString S1,const FString S2);
	void PathsOfSharingSuffix(const FString& Path);
 
	TSharedPtr<FPathSearch> SearchBox;
};
