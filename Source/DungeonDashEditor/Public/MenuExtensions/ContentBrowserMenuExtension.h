// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/MoveAssetsWidget.h"
#include "EditorUtilityWidget/PathSearch.h"

DECLARE_DELEGATE_OneParam(FFillSubmenuDelegate, FMenuBuilder&)


/**
 * 
 */
class DUNGEONDASHEDITOR_API FContentBrowserMenuExtension : public TSharedFromThis<FContentBrowserMenuExtension>
{
public:
	FContentBrowserMenuExtension();
	void AddWidgetEntriesFromEditor(FMenuBuilder& MenuBuilder);
	void AddWidgetEntries(FMenuBuilder& MenuBuilder);
 
	TSharedRef<FExtender> MenuExtensionDelegate(const TArray<FAssetData>& SelectedAssets);
	TArray<FAssetData> SelectedAssets;



};
