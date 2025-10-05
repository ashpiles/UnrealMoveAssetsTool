// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EditorUtilityMenuSettings.generated.h"

USTRUCT(BlueprintType)
struct FWidgetEntry
{ 
	GENERATED_BODY()
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Tool Data")
	FText EntryLabel;
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Tool Data")
	FText EntryToolTip; 

	// will also take a special blueprint which encapsulates a function
	UPROPERTY(EditAnywhere, meta =(AllowedClasses="/Script/Blutility.EditorUtilityWidgetBlueprint"))
	FSoftObjectPath MenuEntryFunctionalityPath; 
};


UCLASS(Config = Editor, DefaultConfig, ClassGroup = Editor, meta = (DisplayName = "Editor Utility Widget menu", ToolTip = "Custom menu configuration for project editor utility widgets"))
class DUNGEONDASHEDITOR_API UEditorUtilityMenuSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Tool Data")
	FText Category;
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Tool Data")
	FText CategoryToolTip;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Tool Data")
	TArray<FWidgetEntry> EditorUtilityWidgets;
	
};
