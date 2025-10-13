// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathSearch.h"


class SAssetSearchBox;
/**
 * 
 */
class DUNGEONDASHEDITOR_API FMoveAssetsWidget : public TSharedFromThis<FMoveAssetsWidget>
{
public: 
	void MakeWidget();
	void SetCachedSelectedAssets(const TArray<FAssetData>& SelectedAssets)
	{
		CachedSelectedAssets = SelectedAssets;
	}

protected:
	static bool MakeFolder(FString NewPath);
	static bool UpdateRefrencers(FString& Path);
	static void MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, FString Path);

private:
	FReply OnCreateFolderButtonClicked() const;
	FReply OnSortAssetsButtonClicked() const;
	FReply OnMoveToSelectedFolderClicked() const;
	FReply OnCacheSelectedAssets();
	FReply OnChangedDestinationPath() const;

   

	TSharedPtr<STextBlock> SelectedAssetsNumTextBox;
	TSharedPtr<STextBlock> DestinationPathTextBox;
	TArray<FAssetData> CachedSelectedAssets;
};
