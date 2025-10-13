// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtilityWidget/MoveAssetsWidget.h"


#include "AssetSelection.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"



void FMoveAssetsWidget::MakeWidget()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString DestinationPathTextBoxString = ContentBrowserModule.Get().GetCurrentPath().GetVirtualPathString();
	DestinationPathTextBoxString.RemoveFromStart("/All");
	
	TSharedPtr<SWindow> MainEditorWindow = FGlobalTabmanager::Get()->GetRootWindow();
    TSharedRef<SWindow> WidgetWindow = SNew(SWindow)
        .Title(FText::FromString("Move Assets"))
        .ClientSize(FVector2D(760, 245))
        .SupportsMinimize(false)
        .SupportsMaximize(false)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
	        .FillWidth(.5f)
            [
                SNew(SVerticalBox)
            	+ SVerticalBox::Slot()
				  .AutoHeight()
				  .VAlign(VAlign_Bottom) 
				  .Padding(20, 80, 20, 20)
				  [
					  SNew(SBox)
					  .HeightOverride(35.f)
					  .WidthOverride(50.f)
					  [
						  SNew(SButton)
						  .Text(FText::FromString("Select Path to Move Assets To"))
						  .VAlign(VAlign_Center)
						  .HAlign(HAlign_Center)
						  .OnClicked_Raw(this, &FMoveAssetsWidget::OnChangedDestinationPath)
					  ]
				  ]
                + SVerticalBox::Slot()
	            .AutoHeight()
                .VAlign(VAlign_Bottom) 
                .Padding(20, 0, 20, 20)
                [
                    SNew(SBox)
                    .HeightOverride(35.f)
				    .WidthOverride(50.f)
                    [
                        SNew(SButton)
                        .Text(FText::FromString("Select Assets to Move"))
                        .VAlign(VAlign_Center)
	                    .HAlign(HAlign_Center)
                        .OnClicked_Lambda([&]
                        {
                        	return OnCacheSelectedAssets();
                        })
                    ]
                ]
                + SVerticalBox::Slot()
	            .AutoHeight()
                .VAlign(VAlign_Bottom) 
	            .Padding(20, 0, 10, 20)
                [
                    SAssignNew(SelectedAssetsNumTextBox, STextBlock)
                    .Text(FText::FromString("Number of Selected Assets: 0"))
                ] + SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Bottom) 
	            .Padding(20, 0, 10, 20)
				[
					SAssignNew(DestinationPathTextBox, STextBlock)
					.Text(FText::FromString(DestinationPathTextBoxString))
				]
            ]

            + SHorizontalBox::Slot()
	        .FillWidth(.5f)
            [
                SNew(SVerticalBox)
            	+ SVerticalBox::Slot()
				  .VAlign(VAlign_Center)
				  .Padding(20, 10, 20, 10)
				  [
					  SNew(SBox)
					  .HeightOverride(40.f)
					  .WidthOverride(50.f)
					  [
						  SNew(SButton)
						  .HAlign(HAlign_Center)
						  .VAlign(VAlign_Center)
						  .Text(FText::FromString("Move Assets"))
						  .OnClicked_Raw(this, &FMoveAssetsWidget::OnMoveToSelectedFolderClicked)
					  ]
				  ]
                + SVerticalBox::Slot()
                .VAlign(VAlign_Center)
                .Padding(20, 10, 20, 10)
                [
                    SNew(SBox)
				    .HeightOverride(40.f)
                	.WidthOverride(50.f)
                    [
                        SNew(SButton)
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        .Text(FText::FromString("Move Assets to New Folder at Destination"))
                        .OnClicked_Raw(this, &FMoveAssetsWidget::OnCreateFolderButtonClicked)
                    ]
                ]
              
	            + SVerticalBox::Slot()
				.VAlign(VAlign_Center)
                .Padding(20, 10, 20, 10)
				[
					SNew(SBox)
				    .HeightOverride(40.f)
					.WidthOverride(50.f)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Text(FText::FromString("Sort Assets by Type"))
						.OnClicked_Raw(this, &FMoveAssetsWidget::OnSortAssetsButtonClicked)
					]
				]
            ]
        ];

	FSlateApplication::Get().AddWindowAsNativeChild(WidgetWindow, MainEditorWindow.ToSharedRef());

}

 
bool FMoveAssetsWidget::MakeFolder(FString NewPath)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	if (!AssetRegistryModule.Get().AddPath(NewPath))
	{ 
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to make path at " + NewPath)) ;
		return false;
	}
	
	return true;
}

bool FMoveAssetsWidget::UpdateRefrencers(FString& Path)
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

void FMoveAssetsWidget::MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, FString Path)
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
	FString DesiredPath = DestinationPathTextBox->GetText().ToString() + "/NewFolder";
	// maybe this can pop up a new window to name the folder?
	DesiredPath.RemoveFromStart(TEXT("/All"));
	if (MakeFolder(DesiredPath))
		MoveAssetsTo(CachedSelectedAssets, DesiredPath);
	
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Moved " + FString::FromInt(CachedSelectedAssets.Num()) + " assets to " + DesiredPath)) ;
	
	return FReply::Handled();
}

FReply FMoveAssetsWidget::OnSortAssetsButtonClicked() const
{
	FString CurrentPath = DestinationPathTextBox->GetText().ToString();
	TSet<FName> AssetTypes;

	for (const FAssetData& Asset : CachedSelectedAssets )
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
		
	}
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Sorted " + FString::FromInt(CachedSelectedAssets.Num()) + " assets to " + FString::FromInt(AssetTypes.Num()) + " unique folders at " + CurrentPath)) ;

	return FReply::Handled();

}

FReply FMoveAssetsWidget::OnMoveToSelectedFolderClicked() const
{
	FString CurrentPath = DestinationPathTextBox->GetText().ToString();
	MoveAssetsTo(CachedSelectedAssets, CurrentPath);
	
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Moved " + FString::FromInt(CachedSelectedAssets.Num()) + " assets to " + CurrentPath));
	return FReply::Handled();
}

FReply FMoveAssetsWidget::OnCacheSelectedAssets()
{
	AssetSelectionUtils::GetSelectedAssets(CachedSelectedAssets);
	FString TextBoxString = "Number of Selected Assets: " + FString::FromInt(CachedSelectedAssets.Num());
	SelectedAssetsNumTextBox.Get()->SetText(FText::FromString(TextBoxString));

	return FReply::Handled();
}

FReply FMoveAssetsWidget::OnChangedDestinationPath() const
{
	TArray<FString> SelectedPaths;
	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowser.Get().GetSelectedFolders(SelectedPaths);
	if (SelectedPaths.IsEmpty())
		ContentBrowser.Get().GetSelectedPathViewFolders(SelectedPaths);

	if (!SelectedPaths.IsEmpty())
		DestinationPathTextBox.Get()->SetText(FText::FromString(SelectedPaths.Last()));

	return FReply::Handled();
}
 

