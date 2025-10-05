// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtilityWidget/PathSearch.h"

#include "SAssetSearchBox.h"

void FPathSearch::Initialize()
{
}

TSharedRef<SWidget> FPathSearch::MakeSearchBar()
{
	return SNew(SVerticalBox)
   + SVerticalBox::Slot()
   .AutoHeight()
   [
	   SNew(SAssetSearchBox)
	   .HintText(NSLOCTEXT("MySearch", "SearchAssets", "Search...")) // placeholder text
	   .OnTextChanged(this, &FPathSearch::OnSearchTextChanged)
	   .OnTextCommitted(this, &FPathSearch::OnSearchTextCommitted)
   ]
   + SVerticalBox::Slot()
   .AutoHeight()
   [
	   SNew(STextBlock)
	   .Text(NSLOCTEXT("MySearch", "SearchResults", "Results will go here"))
   ];
}

void FPathSearch::OnSearchTextChanged(const FText& InText)
{
}

void FPathSearch::OnSearchTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
}
