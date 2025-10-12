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
	void MakeWidget2();
	

protected:
	static bool MakeFolder(FString NewPath);
	static bool UpdateRefrencers(FString& Path);
	static void MoveAssetsTo(TArray<FAssetData>& SelectedAssets, FString Path);

private:
	FReply OnCreateFolderButtonClicked() const;
	FReply OnSortAssetsButtonClicked() const;
	FReply OnMoveToSelectedFolderClicked() const;

   

	TArray<FString> PathStrings;
	TSharedPtr<SEditableTextBox> TextBox;
};
