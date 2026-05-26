#include "pch.h"
#include "FrameTimeRecorder.h"
#include "Session.h"

FrameTimeRecorder* FrameTimeRecorder::me = nullptr;

void FrameTimeRecorder::Start()
{
	Session::Me().m_frameTimeFile.WriteLine("frameTime;forceCalcTime");
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
	Session::Me().m_frameTimeFile.WriteLine(std::to_string(Time::UnscaledDeltaTime() * 1000.f) + ";" + std::to_string(m_forceCalcTime));
	m_forceCalcTime = 0.f;
}
