#include "pch.h"
#include "FrameTimeRecorder.h"
#include "SessionLoggerManager.h"

FrameTimeRecorder* FrameTimeRecorder::me = nullptr;

void FrameTimeRecorder::Start()
{
	SessionLoggerManager::Me()->m_frameTime.WriteLine("frameTime;forceCalcTime");
}

void FrameTimeRecorder::OnEnable()
{
	if (me == nullptr)
		me = this;
	else
		gameObject->SetActive(false);
}

void FrameTimeRecorder::LogFrameTime()
{
	SessionLoggerManager::Me()->m_frameTime.WriteLine(std::to_string(Time::UnscaledDeltaTime() * 1000.f) + ";" + std::to_string(m_forceCalcTime * 1000.f));
	m_forceCalcTime = 0.f;
}
