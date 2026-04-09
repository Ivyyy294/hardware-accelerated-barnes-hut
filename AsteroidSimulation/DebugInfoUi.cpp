#include "pch.h"

#include <IvyyyFontRenderer.h>

#include "DebugInfoUi.h"
#include "DebugInfo.h"

DebugInfoUi::DebugInfoUi()
{
	transform.SetPivot(Transform::Pivot::TOPLEFT);
	transform.SetSpace(Transform::Space::SCREEN);
	auto fontRenderer = AddComponent<FontRenderer>();
	fontRenderer->SetSize(Vector2(512.f, 100.f));

	AddComponent<DebugInfo>();
}
