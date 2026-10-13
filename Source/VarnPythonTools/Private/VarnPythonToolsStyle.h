// Copyright Broken Rock Studios LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSlateStyleSet;
struct FSlateBrush;

/** Slate style set holding the plugin's own icons. */
class FVarnPythonToolsStyle
{
public:
	static void Initialize();
	static void Shutdown();

	static FName GetStyleSetName();

	/** The Python logo, rasterized from Resources/PythonLogo.svg. */
	static const FSlateBrush* GetLogoBrush();

	/** The same logo as a ready-made icon for tab spawners and menu entries. */
	static FSlateIcon GetLogoIcon();

private:
	static TSharedRef<FSlateStyleSet> Create();

	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
