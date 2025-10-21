// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SAssetSearchBox;
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

	// connected to an event from the extension launcher
	// the event will then spawn this menu and we can then spawn this
	bool AddAdvancedMenu();
	bool RemoveAdvancedMenu();

	void FuzzyFind(const FString* Input, TArray<FString>& Output);
	int RecursiveLevenShteinDistance(FString Str1, FString Str2);
	void MergeSortPaths(const FString& Input, TArray<FString>& Paths, int Index, int Range);
	void MergePaths(const FString& Input, TArray<FString>& Paths, int Index, int Middle, int Range);
	

	TSharedPtr<STextBlock> SelectedAssetsNumTextBox;
	TSharedPtr<STextBlock> DestinationPathTextBox;
	TSharedPtr<SEditableTextBox> NewFolderName;
	TSharedPtr<SAssetSearchBox> SearchBar; 
	TSharedPtr<SWindow> AdvancedMenuWindow;
	TArray<FAssetData> CachedSelectedAssets;
	
	bool bIsAutoSavingAssets = false;
	bool bIsAutoRemovingRedirectors = false; 
};
