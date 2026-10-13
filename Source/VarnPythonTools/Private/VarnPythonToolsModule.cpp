// Copyright Broken Rock Studios LLC. All Rights Reserved.

#include "Modules/ModuleManager.h"

#include "Framework/Docking/TabManager.h"
#include "ISettingsModule.h"
#include "SVarnPythonBrowser.h"
#include "SVarnPythonEditor.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"
#include "VarnPythonToolsSettings.h"
#include "VarnPythonToolsStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SNullWidget.h"

#define LOCTEXT_NAMESPACE "VarnPythonBrowser"

namespace
{
	const FName PythonBrowserTabName(TEXT("VarnPythonBrowser"));
	const FName& PythonEditorTabName = SVarnPythonEditor::TabName;
}

class FVarnPythonToolsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		if (IsRunningCommandlet())
		{
			return;
		}

		FVarnPythonToolsStyle::Initialize();

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			                        PythonBrowserTabName,
			                        FOnSpawnTab::CreateRaw(this, &FVarnPythonToolsModule::SpawnTab))
		                        .SetDisplayName(LOCTEXT("Title", "Varn Python Browser"))
		                        .SetTooltipText(LOCTEXT("TabTip", "Browse and run Python scripts."))
		                        .SetIcon(FVarnPythonToolsStyle::GetLogoIcon())
		                        .SetMenuType(ETabSpawnerMenuType::Hidden);

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			                        PythonEditorTabName,
			                        FOnSpawnTab::CreateRaw(this, &FVarnPythonToolsModule::SpawnEditorTab))
		                        .SetDisplayName(LOCTEXT("EditorTitle", "Varn Python Editor"))
		                        .SetTooltipText(LOCTEXT("EditorTabTip", "Browse Python scripts and open them in tabs."))
		                        .SetIcon(FVarnPythonToolsStyle::GetLogoIcon())
		                        .SetMenuType(ETabSpawnerMenuType::Hidden);
		FTabManager::RegisterDefaultTabWindowSize(FTabId(PythonEditorTabName), FVector2D(1100, 700));

		ISettingsModule& Settings = FModuleManager::LoadModuleChecked<ISettingsModule>(TEXT("Settings"));
		Settings.RegisterSettings(
			TEXT("Editor"), TEXT("Plugins"), TEXT("VarnPythonBrowser"),
			LOCTEXT("SettingsTitle", "Varn Python Browser"),
			LOCTEXT("SettingsDescription", "Personal Python script folders for this project. Refresh the browser after editing."),
			GetMutableDefault<UVarnPythonToolsSettings>());

		UToolMenus::RegisterStartupCallback(
			FSimpleMulticastDelegate::FDelegate::CreateRaw(
				this, &FVarnPythonToolsModule::RegisterMenus));
	}

	virtual void ShutdownModule() override
	{
		if (IsRunningCommandlet())
		{
			return;
		}
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
		if (ISettingsModule* Settings = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
		{
			Settings->UnregisterSettings(TEXT("Editor"), TEXT("Plugins"), TEXT("VarnPythonBrowser"));
		}
		if (TSharedPtr<SDockTab> Tab = BrowserTab.Pin())
		{
			// Destroy our widgets before module code can be unloaded.
			Tab->SetContent(SNullWidget::NullWidget);
			Tab->RequestCloseTab();
		}
		if (TSharedPtr<SDockTab> Tab = EditorTab.Pin())
		{
			Tab->SetContent(SNullWidget::NullWidget);
			Tab->RequestCloseTab();
		}
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PythonBrowserTabName);
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PythonEditorTabName);
		FTabManager::UnregisterDefaultTabWindowSize(FTabId(PythonEditorTabName));
		FVarnPythonToolsStyle::Shutdown();
	}
private:
	void RegisterMenus()
	{
		FToolMenuOwnerScoped Owner(this);
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Window"));
		FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("VarnPythonBrowser"), LOCTEXT("Section", "Python"), FToolMenuInsert());
		Section.AddMenuEntry(
			PythonBrowserTabName,
			LOCTEXT("Title", "Varn Python Browser"),
			LOCTEXT("MenuTip", "Browse and run Python scripts."),
			FVarnPythonToolsStyle::GetLogoIcon(),
			FUIAction(
				FExecuteAction::CreateLambda(
					[]
					{
						FGlobalTabmanager::Get()->TryInvokeTab(PythonBrowserTabName);
					})));
		Section.AddMenuEntry(
			PythonEditorTabName,
			LOCTEXT("EditorTitle", "Varn Python Editor"),
			LOCTEXT("EditorMenuTip", "Browse Python scripts and open them in tabs."),
			FVarnPythonToolsStyle::GetLogoIcon(),
			FUIAction(
				FExecuteAction::CreateLambda(
					[]
					{
						FGlobalTabmanager::Get()->TryInvokeTab(PythonEditorTabName);
					})));
	}

	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(SVarnPythonBrowser)
			]
			.Label(LOCTEXT("TabLabel", "Varn PyBrowser"))
			[
				SNew(SVarnPythonBrowser)
			];
		BrowserTab = Tab;
		return Tab;
	}

	TSharedRef<SDockTab> SpawnEditorTab(const FSpawnTabArgs& Args)
	{
		const TSharedRef<SDockTab> Tab = SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			.Label(LOCTEXT("TabLabel", "Varn PyEditor"));
		// The editor builds its own tab manager for opened scripts, so it needs the tab that hosts it.
		Tab->SetContent(SNew(SVarnPythonEditor, Tab));
		EditorTab = Tab;
		return Tab;
	}

	TWeakPtr<SDockTab> BrowserTab;
	TWeakPtr<SDockTab> EditorTab;
};

IMPLEMENT_MODULE(FVarnPythonToolsModule, VarnPythonTools)

#undef LOCTEXT_NAMESPACE
