// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ContentBrowserModule.h"
#include "MoveAssetsMenuExtension.h"
#include "Modules/ModuleManager.h"

class FMoveAssetsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FMoveAssetsMenuExtension> MoveAssetsMenu;
	TSharedPtr<SMoveAssets> MoveAssetsWidget;

};
