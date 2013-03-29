#pragma once
#include "FootAnimator.h"

class Walk : public FootAnimator {
public:
	Walk();
	~Walk();
	void setContactDuration(float duration) { _contactDuration = duration; };
	void setOverlapAlpha(float alpha) { _alpha = alpha; };
	virtual void update(double dt);
	virtual void reset();
protected:
	virtual bool updateFoot(double dt, int type);

private:
	double _t; // keyframe position
	float _alpha;
	float _contactDuration;
	int _whichFoot;
	bool _leftFootActive;
	bool _rightFootActive;
};
