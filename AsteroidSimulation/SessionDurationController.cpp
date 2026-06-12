#include "pch.h"
#include "SessionDurationController.h"
#include <IvyyyEngine.h>

void SessionDurationController::Update()
{
	if (m_maxFixedFrames > 0 && m_fixedFramesCounter >= m_maxFixedFrames)
		IvyyyEngine::Me()->Exit();
}

void SessionDurationController::FixedUpdate()
{
	m_fixedFramesCounter++;
}
