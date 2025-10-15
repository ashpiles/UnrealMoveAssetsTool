// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
DECLARE_DELEGATE_OneParam(FOnCompletedMoveOperation, const TArray<FAssetData>&)

/**
 * 
 */
class MOVEASSETS_API SMoveAssets : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMoveAssets)
	{ }
	SLATE_END_ARGS()
	
	void SetCachedSelectedAssets(const TArray<FAssetData>& SelectedAssets)
	{
		CachedSelectedAssets = SelectedAssets;
	}

	void Construct(const FArguments& InArgs);

protected:
	static bool MakeFolder(FString NewPath, bool bSkipErrorMessasge);
	static bool UpdateRefrencers(FString& Path);
	static TArray<FAssetData> MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, FString Path); 

private:
	FReply OnCreateFolderButtonClicked() const;
	FReply OnSortAssetsButtonClicked() const;
	FReply OnMoveToSelectedFolderClicked() const;
	FReply OnCacheSelectedAssets();
	FReply OnChangedDestinationPath() const; 
   

	TSharedPtr<STextBlock> SelectedAssetsNumTextBox;
	TSharedPtr<STextBlock> DestinationPathTextBox;
	TArray<FAssetData> CachedSelectedAssets;
	FOnCompletedMoveOperation CompletedMoveOperation;
};
