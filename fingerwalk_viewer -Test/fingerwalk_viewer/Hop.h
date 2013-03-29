#pragma once
#include "FootAnimator.h"
enum HopPhase {
	kHopPreAligning,
	kHopAligning,
	kHopHopping,
};
class Hop : public FootAnimator {
public:
	Hop();
	~Hop();
	virtual void update(double dt);
	void setContactDuration(float duration) { _contactDuration = duration; };
	void setAlpha(float alpha) { _alpha = alpha; };
	virtual void reset();
protected:
	virtual bool updateFoot(double dt, int type);
private:
	double _t;
	float _alpha;
	float _contactDuration;
	bool _footActive[2];
	int _whichFootBehind;
	HopPhase _phase;
	float _alignDelta;
	float _alignDuration;
};
