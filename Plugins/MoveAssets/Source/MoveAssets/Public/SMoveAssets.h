// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetToolsModule.h"

class FContentBrowserModule;
class FAssetRegistryModule;
DECLARE_DELEGATE(FOnMoveOperation)

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

	SMoveAssets();

 
	friend class FMoveAssetsMenuExtension;
	
	
private:
	FReply OnSortAssetsButtonClicked() const;
	FReply OnMoveToSelectedFolderClicked() const;
	FReply OnCachedSelectedAssets();
	FReply OnCachedDestinationPath(); 
	FString SelectDestinationPath() const;
	FString GetNewFolderName() const;

	TSharedRef<SWidget> GenerateMoveAssetIconState(const FAssetData&) const;
	bool OnAssetSelected(FAssetData InAssetData) const;
	
protected:
	bool MakeFolder(FString NewPath) const;
	bool UpdateRefrencers(FString& Path) const;
	void MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, const FString& Path) const;
	void GetAssetDependencies(const FAssetData&, TArray<FName>& OutDependencies) const; 

private:
	FContentBrowserModule& ContentBrowserModule;
	FAssetRegistryModule& AssetRegistryModule;
	FAssetToolsModule& AssetToolsModule;
	
	TSharedPtr<SEditableTextBox> NewFolderNameTextBox;
	TSharedPtr<STextBlock> DestinationPathText;
	TSharedPtr<STextBlock> SelectedAssetsNumText;
	TSharedPtr<SMenuPanel> AdvancedMenu;
	TSharedPtr<SButton> CacheDestinationPathButton;
	
	TSet<FAssetData> CachedSelectedAssets;
	FDelegateHandle AssetViewGeneratorHandle;
	FOnMoveOperation SuccesfullMoveOperation;
	FOnMoveOperation FailedMoveOperation;
	FString CachedDestinationPath;
	bool bIsAutoSavingAssets = true;
	bool bIsAutoRemovingRedirectors = true;
	bool bCreateNewFolder = false;

};
