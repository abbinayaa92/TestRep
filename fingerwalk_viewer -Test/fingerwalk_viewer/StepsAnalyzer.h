#pragma once

#include <list>
#include <vector>
#include "Foot.h"

using namespace std;

enum GaitType {
	kGaitWalking = 0,
	kGaitWalkBack =1,
	kGaitRunning = 2,
	kGaitTiptoeing = 4,
	kGaitJumping = 3,
	kGaitJumpLong=10,
	kGaitTiptoeBack = 5,
	kGaitLimping =8,
	kGaitLimpBack=9,
	kGaitJog =11,
	kGaitCatWalk =13,
	kGaitMoonWalk =14,
	kGaitHop =15,
	kGaitTap=16,
	kGaitSpin=17,
	kGaitSkate=18,
	kGaitDuck=19,
	kGaitKick=20,
	kGaitMarch=21,
	kGaitSideWalk=22,
	kGaitSneak=23,
	kGaitSplit=24,
	kGaitSit=25,
	kGaitSideSkip=26,
	kGaitBridge=27,
	kGaitWalkInplace=28,
};

struct LocomotionModel {
	float stepLength;
	double stepDuration;
	float contactDuration;
	GaitType gaitType;
	float alpha; // (run) -1 <= a < 1 (walk)

	// default constructor
	LocomotionModel() : stepLength(0.5), stepDuration(1), contactDuration(0), gaitType(kGaitWalking), alpha(0) {};
};

class GaitChangedListener {
public:
	virtual void gaitChanged(const LocomotionModel& lm) = 0;
};

class StepsAnalyzer
{
public:
	StepsAnalyzer();
	~StepsAnalyzer();

	void update(double dt);
	const LocomotionModel& getLocomotionModel() { return _currModel; };
	FootStepModel* addFootStep(int type, const FootStepModel& m);	
	void addGaitChangedListener(GaitChangedListener *gcl) {
		if (gcl) {
			_listeners.push_back(gcl);
		}		
	}
protected:
	void fireGaitChangedEvent() {
		GaitListeners::iterator i;
		for (i = _listeners.begin(); i != _listeners.end(); ++i) {
			GaitChangedListener *gcl = *i;
			gcl->gaitChanged(_currModel);
		}
	}

protected:
	LocomotionModel _currModel;
	LocomotionModel _prevModel;
	int _gaitTypeCounter;
	GaitType _prevGaitType;

	typedef list<FootStepModel> FootSteps;
	FootSteps _leftSteps;
	FootSteps _rightSteps;
	vector<FootSteps> _steps;
	
	typedef vector<GaitChangedListener*> GaitListeners;
	GaitListeners _listeners;
};
