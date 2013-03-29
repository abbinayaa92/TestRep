#include "StepsAnalyzer.h"
#include <iostream>

#define MAX_STEPS 10
StepsAnalyzer::StepsAnalyzer() {
	_steps.resize(2);

	_gaitTypeCounter = 0;
}

StepsAnalyzer::~StepsAnalyzer() {
}

FootStepModel* StepsAnalyzer::addFootStep(int type, const FootStepModel& m) {
	if (type != 0 && type != 1) return NULL;

	//if (!_steps[type].empty()) {		
	//	const FootStepModel& prev = _steps[type].back();
	//	
	//	double stepDuration = m.startTime - prev.endTime;
	//	//cout << "step duration = " << stepDuration << " " << m.startTime << ", " << prev.endTime << endl;

	//	//FootSteps::iterator i;
	//	//for (i = _steps[type].begin(); i != _steps[type].end(); ++i) {
	//	//}


	//}
	GaitType gaitType = kGaitWalking;

	int other = (type + 1) % 2;
	if (_steps[other].size() >= 1) {
		FootSteps::iterator it;
		it = _steps[other].end();
		--it;

		const FootStepModel& last = *it;

		float alpha = 0;
		if (last.endTime > m.startTime) { // overlap
			// get overlap duration
			float t = last.endTime - m.startTime;
			// get contacts duration
			float t1 = last.endTime - last.startTime;
			float t2 = m.endTime - m.startTime;			

			alpha = t / (t1 + t2 - t);
			if (alpha > 0.8f) {
				//gaitType = kGaitHopping;
				_currModel.alpha = alpha;
				_currModel.contactDuration = t2;
			}
			else {
				gaitType = kGaitWalking;
				_currModel.alpha = alpha;
				_currModel.contactDuration = t2;
			}						
		}
		else {
			float t = m.startTime - last.endTime;
			float t1 = last.endTime - last.startTime;
			float t2 = m.endTime - m.startTime;

			alpha = -t / (t1 + t2 + t);
			gaitType = kGaitRunning;
			_currModel.alpha = alpha;
			_currModel.contactDuration = t2;
		}

		
		if (gaitType != _currModel.gaitType && gaitType == _prevGaitType) {
			_gaitTypeCounter++;
		}
		else {
			// reset
			_gaitTypeCounter = 0;
		}

		
		//cout << _gaitTypeCounter << endl;
		if (_gaitTypeCounter >= 2 )//|| (_currModel.gaitType != kGaitHopping && gaitType == kGaitHopping))
		{
			_prevModel = _currModel; // copy to prev

			_currModel.gaitType = gaitType;
			_currModel.alpha = alpha;

			//switch (gaitType) {
			//	case kGaitWalking:
			//		cout << "walking" << endl;
			//		break;
			//	case kGaitHopping:
			//		cout << "hopping" << endl;
			//		break;
			//	case kGaitRunning:
			//		cout << "running" << endl;
			//		break;
			//}

			this->fireGaitChangedEvent();
		}
		
		_prevGaitType = gaitType;
	}
	

	if (_steps[type].size() > MAX_STEPS) {
		_steps[type].pop_front();
	}

	_steps[type].push_back(m);	

	//cout << "[StepsAnalyzer] got step = " << m.stepId << endl;
	return &_steps[type].back();
}


void StepsAnalyzer::update(double dt) {

}