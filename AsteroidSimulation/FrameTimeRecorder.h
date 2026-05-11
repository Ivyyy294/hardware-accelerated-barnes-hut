#pragma once

#include <IvyyyComponent.h>

class FrameTimeRecorder :
    public Component
{
//Variables
public:
	float m_forceCalcTime = 0.f;
private:
	static FrameTimeRecorder* me;
//Methods
public:
	inline static FrameTimeRecorder* Me(){return me;};
	void Start() override;
	void OnEnable() override;
	void LogFrameTime();
};

