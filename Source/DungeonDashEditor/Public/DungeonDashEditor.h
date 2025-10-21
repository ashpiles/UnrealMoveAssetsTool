#pragma once

#include "CoreMinimal.h"
#include "MenuExtensions/ContentBrowserMenuExtension.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDungeonDashEditor, All, All)

class FDungeonDashEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
     
    UPROPERTY()
    TSharedPtr<FContentBrowserMenuExtension> ContentBrowserMenuExtension;
};
