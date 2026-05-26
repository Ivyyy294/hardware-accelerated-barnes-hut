#include "pch.h"
#include "DebugInfo.h"
#include <IvyyyTime.h>
#include "IvyyyDebug.h"
#include <IvyyyEngine.h>
#include "Session.h"

void DebugInfo::Start ()
{
	fontmesh = gameObject->GetComponent<FontRenderer> ().get ();
}

void DebugInfo::Update ()
{
	UpdateFps();

	if (fontmesh != nullptr)
		fontmesh->SetText ( L"FPS: " + std::to_wstring (fps) + L"\nDeltaTIme: " + std::to_wstring (deltatime));
	//else
	//	Debug::Log("FPS: " + std::to_string(fps) + ", DeltaTIme: " + std::to_string(deltatime));

	if (Input::KeyDown(Key::KEY_G))
		Gizmo::Draw = !Gizmo::Draw;

	if (Input::KeyDown(Key::KEY_ESC))
		IvyyyEngine::Me()->Exit();

	if (Input::KeyDown(Key::KEY_P))
		Session::Me().m_logDebugInfo = true;
}

void DebugInfo::UpdateFps()
{
	deltatime = Time::UnscaledDeltaTime ();

	if (timer <= 1.f)
	{
		timer += deltatime;
		tmpFps++;
	}
	else
	{
		timer -= 1.0f;
		fps = tmpFps;
		tmpFps = 0;
	}
}
