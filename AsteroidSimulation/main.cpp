#include <Windows.h>
#include <IvyyyWindow.h>
#include <IvyyyScreen.h>
#include <IvyyySceneHandler.h>
#include "IvyyyEngine.h"

using namespace Ivyyy;

int WINAPI WinMain (_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
		_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	Window::SetFullscreen(true);
	Screen::SetResolution(Ivyyy::Resolution{ 1920, 1080 });
	Renderer::SetMode(Renderer::Mode::DX11);
	//SceneHandler::AddScene <SolarSystemScene>();
	return Ivyyy::IvyyyEngine::Start(hInstance, nShowCmd);
}