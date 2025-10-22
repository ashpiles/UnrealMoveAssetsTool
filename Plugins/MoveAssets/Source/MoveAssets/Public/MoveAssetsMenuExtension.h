// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SMoveAssets;
/**
 * 
 */
class MOVEASSETS_API FMoveAssetsMenuExtension : public TSharedFromThis<FMoveAssetsMenuExtension>
{
public:
	FMoveAssetsMenuExtension();

	TSharedPtr<SMoveAssets> GetWidget();
	
private:
	void AddWidgetEntries(FMenuBuilder& MenuBuilder);
	void MakeWidget();
 
	TSharedRef<FExtender> MenuExtensionDelegate(const TArray<FAssetData>& SelectedAssets);
	TSharedPtr<SMoveAssets> MoveAssetsWidget; 
	FDelegateHandle AssetViewGeneratorHandle;
};
