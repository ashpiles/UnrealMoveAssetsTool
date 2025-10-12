// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtilityWidget/MoveAssetsWidget.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

#include "AssetSelection.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/PathTree.h"



void FMoveAssetsWidget::MakeWidget()
{

	TSharedPtr<SWindow> MainEditorWindow = FGlobalTabmanager::Get()->GetRootWindow();
    TSharedRef<SWindow> WidgetWindow = FSlateApplication::Get().AddWindow(
        SNew(SWindow)
        .Title(FText::FromString("Move Assets"))
        .ClientSize(FVector2D(450, 150))
        .SupportsMinimize(false)
        .SupportsMaximize(false)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(10, 0, 5, 3)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(.3f)
                .VAlign(VAlign_Center)
                .Padding(20)
                [
                    SNew(SBox)
                    .HeightOverride(35.f)  // Force the button height
                    [
                        SNew(SButton)
                        .Text(FText::FromString("Create Folder"))
                        .VAlign(VAlign_Center)
	                    .HAlign(HAlign_Center)
                        .OnClicked_Raw(this, &FMoveAssetsWidget::OnCreateFolderButtonClicked)
                    ]
                ]
                + SHorizontalBox::Slot()
                .FillWidth(.7f)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(TextBox, SEditableTextBox)
                    .Text(FText::FromString("MyNewFolder"))
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(.3f)
                .VAlign(VAlign_Center)
                .Padding(20)
                [
                    SNew(SBox)
                    .HeightOverride(35.f)
                    [
                        SNew(SButton)
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        .Text(FText::FromString("Move to Selected Folder"))
                        .OnClicked_Raw(this, &FMoveAssetsWidget::OnMoveToSelectedFolderClicked)
                    ]
                ]
                + SHorizontalBox::Slot()
                .FillWidth(.3f)
                .VAlign(VAlign_Center)
                .Padding(20)
                [
                    SNew(SBox)
                    .HeightOverride(35.f)
                    [
                        SNew(SButton)
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        .Text(FText::FromString("Sort Assets by Type"))
                        .OnClicked_Raw(this, &FMoveAssetsWidget::OnSortAssetsButtonClicked)
                    ]
                ]
            ]
        ]
    );

	FSlateApplication::Get().AddWindowAsNativeChild(WidgetWindow, MainEditorWindow.ToSharedRef());
}

void FMoveAssetsWidget::MakeWidget2()
{
	FSlateApplication::Get().AddWindow( 
	   SNew(SWindow)
	   .Title(FText::FromString("Move Assets"))
	   .ClientSize(FVector2D(550, 230))
	   .SupportsMinimize(false)
	   .SupportsMaximize(false)
	   [
		   SNew(SVerticalBox)
	   ]
	   );
}

bool SMoveAssetsWidget::MakeFolder(FString FolderPath)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	if (!AssetRegistryModule.Get().AddPath(FolderPath))
	{ 
		UE_LOG(LogTemp, Error, TEXT("Create Folder Failed - Failed to Add Path. '%s'"), *FolderPath);
		return false;
	}
	
	return true;
}

bool SMoveAssetsWidget::UpdateRefrencers(FString& Path)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	
	IAssetTools& AssetTools = AssetToolsModule.Get();
	FARFilter Filter;
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(FName(Path));

	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	TArray<UObjectRedirector*> Redirectors;
	for (const FAssetData& AssetData : AssetList)
	{
		if (AssetData.IsRedirector())
		{
			Redirectors.Add(Cast<UObjectRedirector>(AssetData.GetAsset()));
		}
	}

	if (Redirectors.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateRefrences - no Refrences to update"));
		return false;
	}
	AssetTools.FixupReferencers(Redirectors);
	return true;
}

void FMoveAssetsWidget::MoveAssetsTo(TArray<FAssetData>& SelectedAssets, FString Path)
{
	Path.RemoveFromStart(TEXT("/All"));
	TArray<UObject*> Assets;
	for (auto AssetData : SelectedAssets)
	{
		if (UObject* Asset = AssetData.GetAsset())
			Assets.Add(Asset);
	}
	AssetViewUtils::MoveAssets(Assets, Path);
	UpdateRefrencers(Path);

}

 
FReply FMoveAssetsWidget::OnCreateFolderButtonClicked() const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString DesiredPath = ContentBrowserModule.Get().GetCurrentPath().GetVirtualPathString() + "/" + TextBox->GetText().ToString();
	DesiredPath.RemoveFromStart(TEXT("/All"));
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	if (MakeFolder(DesiredPath))
	{ 
		MoveAssetsTo(SelectedAssets, DesiredPath);
		UpdateRefrencers(DesiredPath);
	}
	
	return FReply::Handled();
}

FReply FMoveAssetsWidget::OnSortAssetsButtonClicked() const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString CurrentPath = ContentBrowserModule.Get().GetCurrentPath().GetVirtualPathString();
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets); 
	TSet<FName> AssetTypes;

	for (FAssetData& Asset : SelectedAssets )
	{
		FName AssetName = Asset.AssetClassPath.GetAssetName();
		FString DestinationPath = CurrentPath + "/" + AssetName.ToString() + "s";
		DestinationPath.RemoveFromStart(TEXT("/All"));
		TArray<FAssetData> FilteredAsset;
		FilteredAsset.Add(Asset);
		if (!AssetTypes.Contains(AssetName))
		{
			AssetTypes.Add(AssetName);
			MakeFolder(DestinationPath);
		}
		
		MoveAssetsTo(FilteredAsset, DestinationPath);
		
		UE_LOG(LogTemp, Display, TEXT("Asset: %s"), *AssetName.ToString()); 
	}


	UpdateRefrencers(CurrentPath);
	
	return FReply::Handled();
}

FReply FMoveAssetsWidget::OnMoveToSelectedFolderClicked() const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	
	TArray<FString> SelectedPaths;
	TArray<FAssetData> CachedAssets;

	AssetSelectionUtils::GetSelectedAssets(CachedAssets); 
	FString CurrentPath = ContentBrowserModule.Get().GetCurrentPath().GetVirtualPathString();
	ContentBrowserModule.Get().GetSelectedFolders(SelectedPaths);

	if (SelectedPaths.Num() > 0)
	{ 
		MoveAssetsTo(CachedAssets, SelectedPaths.Last());
		UpdateRefrencers(CurrentPath);
	}
		
	return FReply::Handled(); 
} 
