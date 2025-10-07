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

	bool bPathExists(FString Path);


	TArray<FAssetData> InData; 

private:
	int LevenshteinDistance(const FString S1,const FString S2);
	void PathsOfSharingSuffix(const FString& Path);

 
	TSharedPtr<SAssetSearchBox> SearchBox;
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
};
