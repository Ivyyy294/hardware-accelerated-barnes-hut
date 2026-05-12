#pragma once

#include <IvyyyComponent.h>

class SessionDurationController :
    public Component
{
//Variables
public:
	int m_maxFixedFrames = 0;
	float m_maxDurationSec = 0.f;

private:
	int m_fixedFramesCounter = 0;
	clock_t m_startTime;

//Methods
public:
	void Start() override;
	void Update() override;
	void FixedUpdate() override;
};

