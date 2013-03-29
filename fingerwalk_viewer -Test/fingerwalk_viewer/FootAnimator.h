#pragma once
#include "Foot.h"

#define MAX_FEET_DISTANCE 1.0

class FootAnimator
{
public:
	FootAnimator();
	~FootAnimator();
	void setLeftFoot(FootDrawable *leftFoot) { _foot[0] = leftFoot; };
	void setRightFoot(FootDrawable *rightFoot) { _foot[1] = rightFoot; };

	void setFootSwingDuration(float duration) { _swingDuration = duration; };
	void setFootSwingLength(float length) { if (!_stopped) _swingLength = length; };
	virtual void update(double dt);
	virtual void reset();
	bool stop();
protected:
	virtual bool updateFoot(double dt, int type);
protected:
	FootDrawable *_foot[2];

	float _swingDuration;
	float _swingLength;

	float _currSwingDuration;
	float _currSwingLength;
	bool _stopped;

	FootAnim _footAnim[2];
};
