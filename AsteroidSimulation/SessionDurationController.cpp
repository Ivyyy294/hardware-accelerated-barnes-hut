#include "pch.h"
#include "SessionDurationController.h"
#include <IvyyyEngine.h>

void SessionDurationController::Start()
{
	m_startTime = clock();
}

void SessionDurationController::Update()
{
	clock_t currentTime = clock();
	float diff = currentTime - m_startTime;

	if (m_maxDurationSec > 0.f && diff >= m_maxDurationSec)
		IvyyyEngine::Me()->Exit();

	if (m_maxFixedFrames > 0 && m_fixedFramesCounter >= m_maxFixedFrames)
		IvyyyEngine::Me()->Exit();
}

void SessionDurationController::FixedUpdate()
{
	m_fixedFramesCounter++;
}
