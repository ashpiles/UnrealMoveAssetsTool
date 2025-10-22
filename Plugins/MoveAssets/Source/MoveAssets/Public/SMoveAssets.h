// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MoveAssets.h"
#include "CoreMinimal.h"

/**
 * 
 */
class MOVEASSETS_API SMoveAssets : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMoveAssets)
	{ }
	SLATE_END_ARGS()
 
	void Construct(const FArguments& InArgs);

	friend class FMoveAssetsModule;
	
	
	
protected:
	bool MakeFolder(FString NewPath, bool bSkipErrorMessasge) const;
	bool UpdateRefrencers(FString& Path) const;
	void MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, FString Path) const;
	void GetAssetDependencies(const FAssetData&, TArray<FName>& OutDependencies) const;


private:
	FReply OnSortAssetsButtonClicked() const;
	FReply OnMoveToSelectedFolderClicked() const;
	FReply OnCachedSelectedAssets();
	FReply OnCachedDestinationPath(); 
	FString SelectDestinationPath() const;

	TSharedRef<SWidget> GenerateMoveAssetIconState(const FAssetData&) const;
	bool OnAssetSelected(FAssetData InAssetData) const;
 
	TSharedPtr<SEditableTextBox> NewFolderName;
	TSharedPtr<STextBlock> DestinationPathText;
	TSharedPtr<STextBlock> CachedAssetsNumText;
	TSharedPtr<SMenuPanel> AdvancedMenu;
	FButtonStyle AdvancedMenuButtonStyle;
	TSet<FAssetData> CachedSelectedAssets;
	TArray<FDelegateHandle> AssetViewGeneratorHandles;
	FString CachedDestinationPath;
	bool bIsAutoSavingAssets = true;
	bool bIsAutoRemovingRedirectors = true;

};
