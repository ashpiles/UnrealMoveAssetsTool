
#include "MoveAssetsMenuExtension.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "SMoveAssets.h"

#define LOCTEXT_NAMESPACE "DungeonDashEditor"


FMoveAssetsMenuExtension::FMoveAssetsMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenderDelegates = ContentBrowserModule. GetAllAssetViewContextMenuExtenders();
	MenuExtenderDelegates.Add( FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FMoveAssetsMenuExtension::MenuExtensionDelegate));
 
	SAssignNew(MoveAssetsWidget, SMoveAssets);
	
	FAssetViewExtraStateGenerator StateGenerator(
		FOnGenerateAssetViewExtraStateIndicators::CreateSP(MoveAssetsWidget.ToSharedRef(), &SMoveAssets::GenerateMoveAssetIconState),
		FOnGenerateAssetViewExtraStateIndicators()
		);
	AssetViewGeneratorHandle = ContentBrowserModule.AddAssetViewExtraStateGenerator(StateGenerator);
	
}

TSharedPtr<SMoveAssets> FMoveAssetsMenuExtension::GetWidget()
{
	return MoveAssetsWidget;
}


void FMoveAssetsMenuExtension::AddWidgetEntries(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString("Move Asset Helper"),
		FText::FromString("Helper that moves and or sort assets to a new folder at desired path while cleaning up redirectors"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(this, &FMoveAssetsMenuExtension::MakeWidget))
		); 
}

void FMoveAssetsMenuExtension::MakeWidget()
{  
	TSharedPtr<SWindow> MainEditorWindow = FGlobalTabmanager::Get()->GetRootWindow(); 
	TSharedPtr<SWindow> MoveAssetsWidgetWindow = SNew(SWindow) 
	.Title(FText::FromString("Move Asset Helper")) 
	.ClientSize(FVector2D(400, 130)) 
	.SizingRule(ESizingRule::FixedSize)
	.SupportsMaximize(false)
	.SupportsMinimize(false)
	.FocusWhenFirstShown(false)
	.IsInitiallyMaximized(false)
	[
		MoveAssetsWidget.ToSharedRef()
	];

	// Clean up our MoveAssetWidget Data on close
	// Ugly but works
	MoveAssetsWidgetWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this] (TSharedPtr<SWindow> Window)
	{
		MoveAssetsWidget->CachedSelectedAssets.Empty();
		MoveAssetsWidget->CachedDestinationPath.Empty();
	}));
	
	FSlateApplication::Get().AddWindowAsNativeChild(MoveAssetsWidgetWindow.ToSharedRef(), MainEditorWindow.ToSharedRef());
}

// better name to explain its responsibilities
TSharedRef<FExtender> FMoveAssetsMenuExtension::MenuExtensionDelegate(const TArray<FAssetData>& Assets)
{ 
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

	MenuExtender->AddMenuExtension(
		"CommonAssetActions",
		EExtensionHook::After,
		TSharedPtr<FUICommandList>(),
		FMenuExtensionDelegate::CreateSP(this, &FMoveAssetsMenuExtension::AddWidgetEntries)
		);
	return MenuExtender.ToSharedRef();
}





#undef LOCTEXT_NAMESPACE
