#include "pch.h"

#include <Windows.h>

#include <IvyyyWindow.h>
#include <IvyyyScreen.h>
#include <IvyyyEngine.h>
#include <IvyyySceneHandler.h>
#include <IvyyyMsgBox.h>

#include "SceneDirectIntegration.h"
#include "SceneBarnesHut.h"
#include "SceneBarnesHutCS.h"
#include "Session.h"
#include "SessionLogger.h"

using namespace Ivyyy;

int WINAPI WinMain (_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
		_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//Load session variables
	SessionVariables variables;


	if (!SessionVariablesLoadFromFile (variables, "variables.csv"))
	{
		variables.numAsteroids = 1024;
		variables.ugc = 0.00005f;
		variables.fixedDeltaTime = Time::UnscaledFixedDeltaTime();
		variables.distRange = { 10.f, 1.f };
		variables.eRange = { 0.f, 0.25f };
		variables.iRange = { -10.f, 10.f };
		variables.maxFixedFrames = 0;
	}

	Session::Me().m_variables = variables;

	if (SessionLogger::Me().StartSession(variables.outDir))
	{
		Window::SetFullscreen (true);
		Screen::SetResolution (Ivyyy::Resolution{ 1920, 1080 });
		Renderer::SetMode (Renderer::Mode::DX11);

		if (variables.method == SessionVariables::ForceGeneratorMethod::DIRECT)
			SceneHandler::AddScene <SceneDirectIntegration>();
		else if (variables.method == SessionVariables::ForceGeneratorMethod::BARNES_HUT_CPU)
			SceneHandler::AddScene <SceneBarnesHut>();
		else
			SceneHandler::AddScene <SceneBarnesHutCS>();

		int val = IvyyyEngine::Start(hInstance, nShowCmd);

		SessionLogger::Me().EndSession();

		return val;
	}
	else
	{
		Ivyyy::MsgBox::ShowError(L"Failed to start session!");
		return 0;
	}

}