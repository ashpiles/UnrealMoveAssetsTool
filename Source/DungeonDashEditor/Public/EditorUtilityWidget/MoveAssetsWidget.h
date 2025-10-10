// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


class SAssetSearchBox;
 

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
	static int LevenshteinDistance(FString String1, FString String2);


	TArray<FAssetData> InData; 

private:

	static bool SegmentPathHeirachy(const FString& Input, TArray<FString>& OutArray);
	void IndexPathStrings();
	void FuzzyFind(const FString& Input, int FuzzyRange, TArray<FString>& OutArray);
	

	TArray<FString> PathStrings;
	TMap<FString, TArray<int>> NGramIndex;
	TSharedPtr<SAssetSearchBox> SearchBox;
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
};
