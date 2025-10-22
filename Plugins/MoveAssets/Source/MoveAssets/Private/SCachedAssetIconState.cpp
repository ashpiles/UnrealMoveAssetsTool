// 


#include "SCachedAssetIconState.h"


void SCachedAssetIconState::Construct(FArguments InArgs)
{
	AssetCachedListner = InArgs._OnAssetCached;
	AssetData = InArgs._AssetData;
	SetVisibility(MakeAttributeSP(this, &SCachedAssetIconState::GetVisibility));
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Symbols.Check"))
		.BorderBackgroundColor(FLinearColor::Black)
		.DesiredSizeScale(FVector2D(1.2f,1.2f))
		[ 
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Symbols.Check"))
				.ColorAndOpacity(FLinearColor(.2f,1.f,.5f,1.f))
		]
	];
}

EVisibility SCachedAssetIconState::GetVisibility() const
{
	if (AssetCachedListner.IsBound())
		return AssetCachedListner.Execute(AssetData) ? EVisibility::Visible : EVisibility::Hidden;
	return EVisibility::Hidden;
}
 
