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
	bool MakeFolder(FString NewPath, bool bSkipErrorMessasge) const;
	bool UpdateRefrencers(FString& Path) const;
	TArray<FAssetData> MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, FString Path) const;
	void GetAssetDependencies(const FAssetData&, TArray<FName>& OutDependencies) const;

private:
	FReply OnCreateFolderButtonClicked() const;
	FReply OnSortAssetsButtonClicked() const;
	FReply OnMoveToSelectedFolderClicked() const;
	FReply OnCacheSelectedAssets();
	FReply OnChangedDestinationPath() const;
 

	TSharedPtr<STextBlock> SelectedAssetsNumTextBox;
	TSharedPtr<STextBlock> DestinationPathTextBox;
	TSharedPtr<SCheckBox> DependancyCheckerCheckBox;
	TArray<FAssetData> CachedSelectedAssets;
	FOnCompletedMoveOperation CompletedMoveOperation;
};
