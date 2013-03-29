#pragma once
#include "FootAnimator.h"

class TipToe : public FootAnimator {
public:
	TipToe();
	~TipToe();
	virtual void update(double dt);
	virtual void reset();
protected:
	virtual bool updateFoot(double dt, int type);
private:
	double _t;
	
	int _whichFoot;
	bool _footActive[2];
};
