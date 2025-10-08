// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


class SAssetSearchBox;

struct FPathNode
{
	FString Path;
	FString SearchString;

	int32 CachedEditDistance;

	// its possible the compare call is expensive
	int32 EditDistance() const
	{
		if (!Path.IsEmpty() && !SearchString.IsEmpty())
			return SearchString.Compare(Path);
		return 0;
	}
	int32 PathDistance() const
	{
		if (!Path.IsEmpty() && !SearchString.IsEmpty())
			return FMath::Abs(Path.Len() - SearchString.Len());
		return 0;
	}
};


/**
 * 
 */
class DUNGEONDASHEDITOR_API FMoveAssetsWidget : public TSharedFromThis<FMoveAssetsWidget>
{
public:

	void MakeWidget();
	
	bool MakeFolder(FString FolderPath);
	bool UpdateRefrencers(FString& Path);
	void MoveAssetsTo(TArray<FAssetData> SelectedAssets, FString Path);



	TArray<FAssetData> InData; 

private:

	static void AddStringToBucket(const FString& Input, TArray<TArray<FPathNode> >& Buckets);
	static void SelectionSortFuzzySearchBucket(TArray<FPathNode>& Bucket);
	static TArray<FString> FuzzySearch(const FString& Input, int BatchSize, int StartingIndex);


	TArray<FPathNode> PathTrie;
	TSharedPtr<SAssetSearchBox> SearchBox;
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
};
