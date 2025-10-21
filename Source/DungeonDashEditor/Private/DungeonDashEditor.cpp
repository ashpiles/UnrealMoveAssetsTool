#include "DungeonDashEditor.h"

#include "ContentBrowserModule.h"
#include "LevelEditor.h"
#include "MenuExtensions/ContentBrowserMenuExtension.h"

#define LOCTEXT_NAMESPACE "FDungeonDashEditorModule"
DEFINE_LOG_CATEGORY(LogDungeonDashEditor)

void FDungeonDashEditorModule::StartupModule()
{
	ContentBrowserMenuExtension = MakeShareable(new FContentBrowserMenuExtension());
	
}

void FDungeonDashEditorModule::ShutdownModule()
{
    
}  


#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDungeonDashEditorModule, DungeonDashEditor)