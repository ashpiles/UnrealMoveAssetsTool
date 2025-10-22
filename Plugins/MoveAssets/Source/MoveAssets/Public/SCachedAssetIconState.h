// 

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FOnAssetCachedListner, FAssetData)


/**
 * 
 */
class MOVEASSETS_API SCachedAssetIconState : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCachedAssetIconState){}
		SLATE_EVENT(FOnAssetCachedListner, OnAssetCached)
		SLATE_ARGUMENT(FAssetData, AssetData)
	SLATE_END_ARGS()
public: 
	void Construct(FArguments InArgs);
	
private:

	EVisibility GetVisibility() const;
  
	FOnAssetCachedListner AssetCachedListner;
	FAssetData AssetData;
	
};
