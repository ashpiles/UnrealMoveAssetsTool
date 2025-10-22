// Copyright Epic Games, Inc. All Rights Reserved.

#include "MoveAssets.h"
#include "ContentBrowserModule.h"
#include "SMoveAssets.h"

#define LOCTEXT_NAMESPACE "FMoveAssetsModule"

void FMoveAssetsModule::StartupModule()
{
	MoveAssetsMenu = MakeShared<FMoveAssetsMenuExtension>(); 
}

void FMoveAssetsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMoveAssetsModule, MoveAssets)