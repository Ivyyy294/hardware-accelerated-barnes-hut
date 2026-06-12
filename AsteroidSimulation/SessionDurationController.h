#pragma once

#include <IvyyyComponent.h>

class SessionDurationController :
    public Component
{
//Variables
public:
	int m_maxFixedFrames = 0;

private:
	int m_fixedFramesCounter = 0;

//Methods
public:
	void Update() override;
	void FixedUpdate() override;
};

