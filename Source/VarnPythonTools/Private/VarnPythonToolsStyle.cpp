// Copyright Broken Rock Studios LLC. All Rights Reserved.

#include "VarnPythonToolsStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"

#define RootToContentDir Style->RootToContentDir

namespace
{
	const FName LogoBrushName(TEXT("VarnPythonTools.Logo"));
}

TSharedPtr<FSlateStyleSet> FVarnPythonToolsStyle::StyleInstance = nullptr;

void FVarnPythonToolsStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FVarnPythonToolsStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		ensure(StyleInstance.IsUnique());
		StyleInstance.Reset();
	}
}

FName FVarnPythonToolsStyle::GetStyleSetName()
{
	static const FName StyleSetName(TEXT("VarnPythonToolsStyle"));
	return StyleSetName;
}

const FSlateBrush* FVarnPythonToolsStyle::GetLogoBrush()
{
	return StyleInstance.IsValid() ? StyleInstance->GetBrush(LogoBrushName) : nullptr;
}

FSlateIcon FVarnPythonToolsStyle::GetLogoIcon()
{
	return FSlateIcon(GetStyleSetName(), LogoBrushName);
}

TSharedRef<FSlateStyleSet> FVarnPythonToolsStyle::Create()
{
	const TSharedRef<FSlateStyleSet> Style = MakeShared<FSlateStyleSet>(GetStyleSetName());
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("VarnPythonTools"));
	if (ensure(Plugin.IsValid()))
	{
		Style->SetContentRoot(Plugin->GetBaseDir() / TEXT("Resources"));
	}

	// Tab and menu icons render at 16x16; Slate rasterizes the SVG at the size it is drawn.
	Style->Set(LogoBrushName, new IMAGE_BRUSH_SVG(TEXT("PythonLogo"), FVector2f(16.0f, 16.0f)));
	return Style;
}

#undef RootToContentDir
