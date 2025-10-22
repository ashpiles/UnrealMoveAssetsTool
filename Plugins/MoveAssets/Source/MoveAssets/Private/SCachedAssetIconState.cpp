// 


#include "SCachedAssetIconState.h"


void SCachedAssetIconState::Construct(FArguments InArgs)
{
	AssetCachedListner = InArgs._OnAssetCached;
	AssetData = InArgs._AssetData;
	SetVisibility(MakeAttributeSP(this, &SCachedAssetIconState::GetVisibility));
	ChildSlot
	[
		SNew(SImage)
		.Image(FAppStyle::GetBrush("GenericWhiteBox"))
	];
}

EVisibility SCachedAssetIconState::GetVisibility() const
{
	if (AssetCachedListner.IsBound())
		return AssetCachedListner.Execute(AssetData) ? EVisibility::Visible : EVisibility::Hidden;
	return EVisibility::Hidden;
}
 
