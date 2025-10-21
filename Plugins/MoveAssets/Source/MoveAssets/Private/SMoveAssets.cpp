// Fill out your copyright notice in the Description page of Project Settings.


#include "SMoveAssets.h"


#include "AssetSelection.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "SAssetSearchBox.h"
#include "AssetRegistry/AssetRegistryModule.h"


 

void SMoveAssets::Construct(const FArguments& InArgs)
{
	// Initializing important variables
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	// To bypass const in move asset operations so that Selected Assets can be updated

    ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(30,10)
		[
			SAssignNew(NewFolderName, SEditableTextBox) 
		]
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SBox)
				.Padding(20,5)
				.MinDesiredHeight(70)
				.MinDesiredWidth(200)
				[
					SNew(SButton)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Text(FText::FromString("Move"))
					.OnClicked(this, &SMoveAssets::OnMoveToSelectedFolderClicked)
				]
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SBox)
				.Padding(20,5)
				.MinDesiredHeight(70)
				.MinDesiredWidth(200)
				[
					SNew(SButton)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Text(FText::FromString("Move & Sort"))
					.OnClicked(this, &SMoveAssets::OnSortAssetsButtonClicked)
				]
			] 
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(10)
			[
				SNew(SCheckBox)
				.OnCheckStateChanged_Lambda([this] (ECheckBoxState State) -> void
				{
					if (State == ECheckBoxState::Checked)
					{
						
						AddAdvancedMenu(); // trigger spawn event
					}
					else
					{
						
						RemoveAdvancedMenu(); // trigger despawn event
					}
				})
			]
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
			]
		]
	];
		


} 

bool SMoveAssets::MakeFolder(FString NewPath, bool bSkipErrorMessage = false) const
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	if (!AssetRegistryModule.Get().AddPath(NewPath) && !bSkipErrorMessage)
	{ 
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to make path at " + NewPath)) ;
		return false;
	}
	
	return true;
}

bool SMoveAssets::UpdateRefrencers(FString& Path) const
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
	FEditorFileUtils::SaveDirtyPackages(true, true, true);
	AssetTools.FixupReferencers(Redirectors);
	return true;
}

TArray<FAssetData> SMoveAssets::MoveAssetsTo(const TArray<FAssetData>& SelectedAssets, FString Path) const
{
	Path.RemoveFromStart(TEXT("/All"));
	TArray<UObject*> Assets;
	TArray<FAssetData> NewSelectedAssets;
 
	
	for (const FAssetData& AssetData : SelectedAssets)
	{ 
		if (UObject* Asset = AssetData.GetAsset())
		{
			Assets.Add(Asset);
			NewSelectedAssets.Add(FAssetData(Asset, FAssetData::ECreationFlags::None)); 
		}
		else
			NewSelectedAssets.Add(AssetData); 
	}
	AssetViewUtils::MoveAssets(Assets, Path);
	
	return NewSelectedAssets;
}

void SMoveAssets::GetAssetDependencies(const FAssetData& AssetData, TArray<FName>& OutDependencies) const
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	AssetRegistryModule.Get().GetDependencies(AssetData.PackageName, OutDependencies);

	for (FName& Dependency : OutDependencies)
		UE_LOG(LogTemp, Display, TEXT("%s"), *Dependency.ToString())

}

FReply SMoveAssets::OnSortAssetsButtonClicked() const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> MovedAssets;
	TArray<FAssetData> SelectedAssets;
	FString ToPath = SelectDestinationPath();
	FString FromPath = ToPath;
	
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	SelectedAssets.Append(CachedSelectedAssets);
	
	TMap<FName, TArray<FAssetData>> AssetTypes;

	if (MakeFolder(FromPath + "/" + NewFolderName.Get()->GetText().ToString()))
	{
		ToPath += "/" + NewFolderName.Get()->GetText().ToString();
	}

	if (SelectedAssets.Num() <= 0)
	{ 
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to sort assets, cache selected assets before commencing operation")) ;
		return FReply::Handled();
	}

	for (const FAssetData& Asset : SelectedAssets )
	{
		FName AssetName = Asset.AssetClassPath.GetAssetName();
		AssetTypes.FindOrAdd(AssetName).Add(Asset);
	}
	for (auto Element : AssetTypes )
	{ 
		FString UniqueToPath = ToPath + "/" + Element.Key.ToString() + "s";
		MakeFolder(UniqueToPath, true);
		MovedAssets.Append(MoveAssetsTo(Element.Value, UniqueToPath)); 
		UpdateRefrencers(FromPath); 
	}
	
	
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Sorted " + FString::FromInt(CachedSelectedAssets.Num()) + " assets to " + FString::FromInt(AssetTypes.Num()) + " unique folders at " + SelectDestinationPath())) ;

	return FReply::Handled(); 
}

FReply SMoveAssets::OnMoveToSelectedFolderClicked() const
{
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);
	SelectedAssets.Append(CachedSelectedAssets);
	FString FromPath = SelectDestinationPath();
	FString ToPath = SelectDestinationPath() + "/" + NewFolderName.Get()->GetText().ToString();
	if (MakeFolder(ToPath))
	{
		MoveAssetsTo(SelectedAssets, ToPath);
		UpdateRefrencers(FromPath);
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Moved " + FString::FromInt(CachedSelectedAssets.Num()) + " assets to " + SelectDestinationPath()));
		
	}
	
	return FReply::Handled();
}

FReply SMoveAssets::OnCacheSelectedAssets()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	AssetSelectionUtils::GetSelectedAssets(CachedSelectedAssets);
	FString TextBoxString = "Number of Selected Assets: " + FString::FromInt(CachedSelectedAssets.Num());
	SelectedAssetsNumTextBox.Get()->SetText(FText::FromString(TextBoxString));


	return FReply::Handled();
}
 
FReply SMoveAssets::OnCachedDestinationPath()
{
	TArray<FString> SelectedPaths;
	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowser.Get().GetSelectedFolders(SelectedPaths);
	if (SelectedPaths.IsEmpty())
		ContentBrowser.Get().GetSelectedPathViewFolders(SelectedPaths);

	if (!SelectedPaths.IsEmpty())
	{ 
		CachedDestinationPath = SelectedPaths.Last();
		// add search box update
	}
	// also add simple search functionality

	return FReply::Handled();
}

bool SMoveAssets::AddAdvancedMenu()
{
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	SAssignNew(AdvancedMenuWindow, SWindow)
	.ClientSize(FVector2D(520, 150))
	.SizingRule(ESizingRule::FixedSize)
	.Title(FText::FromString("Asset Mover Advanced Menu"))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SHorizontalBox) 
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(5)
			[
				SNew(SCheckBox)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Auto Save Assets on Move"))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			[
				SNew(SCheckBox)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Auto Delete Redirectors on Move"))
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		.MaxHeight(200)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SBox) 
				.HeightOverride(100)
				.WidthOverride(200)
				[ 
					SNew(SButton)
					.Text(FText::FromString("Select Destination Path"))
					.OnClicked_Lambda([this]
					{
						OnCachedDestinationPath();
						return FReply::Handled();
					})
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SBox)
				.HeightOverride(20)
				.WidthOverride(300)
				[ 
					SNew(SAssetSearchBox)
					.OnTextChanged_Raw()
					.OnTextCommitted_Raw()
					.OnAssetSearchBoxSuggestionFilter_Raw()
					.OnAssetSearchBoxSuggestionChosen_Raw()
				]
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		.MaxHeight(200)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5)
			[
				SNew(SBox)
				.HeightOverride(100) 
				.WidthOverride(200)
				[ 
					SNew(SButton)
					.Text(FText::FromString("Add Assets to Next Move"))
					.OnClicked_Lambda([this]
					{
						OnCacheSelectedAssets();
						return FReply::Handled();
					})
				]
			]
		]
	];
	
	FSlateApplication::Get().AddWindowAsNativeChild(AdvancedMenuWindow.ToSharedRef(), ParentWindow.ToSharedRef()); 
	return true;
}

bool SMoveAssets::RemoveAdvancedMenu()
{
	return true;
}

FString SMoveAssets::SelectDestinationPath() const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString CurrentPath = ContentBrowserModule.Get().GetCurrentPath().GetVirtualPathString();
	CurrentPath.RemoveFromStart("/All");
	return CachedDestinationPath.IsEmpty() ? CachedDestinationPath : CurrentPath;
}

