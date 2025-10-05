// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *  Connects the Search-bar with the desired content and algorithm to use
 */
class DUNGEONDASHEDITOR_API FPathSearch : public TSharedFromThis<FPathSearch>
{
public:
	void Initialize();

	
	TSharedRef<SWidget> MakeSearchBar();

private:
	void OnSearchTextChanged(const FText& InText);
	void OnSearchTextCommitted(const FText& InText, ETextCommit::Type CommitType);
};
