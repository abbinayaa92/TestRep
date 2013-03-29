#include "Hop.h"
#include <iostream>

using namespace std;

#define clamp(val, a, b) max(min(val, b), a)

Hop::Hop() :
_t(0),
_alpha(0),
_whichFootBehind(0),
_phase(kHopPreAligning),
_alignDelta(0),
_alignDuration(0.5)
{
	_footActive[0] = false;
	_footActive[1] = false;
}

Hop::~Hop() {
}

void Hop::reset() {
	FootAnimator::reset();
	cout << "[Hop] reset" << endl;
	_phase = kHopPreAligning;
	_t = 0;
	_footActive[0] = false;
	_footActive[1] = false;
}
void Hop::update(double dt) {
	FootAnimator::update(dt);
	//cout << "hopping = " << _phase << endl;
	switch (_phase) {
		case kHopPreAligning:
		{			
			float delta = float(_foot[0]->pos.z - _foot[1]->pos.z);	
			_whichFootBehind = delta > 0 ? FOOT_LEFT : FOOT_RIGHT;
			_alignDelta = fabs(delta);
			int type = _whichFootBehind;
			_footAnim[type].swingDuration = _alignDuration;
			_footAnim[type].swingLength = _alignDelta;
			_foot[type]->toPos.z = _foot[type]->lastPos.z - _footAnim[type].swingLength;
			_footAnim[type].animating = true;
			_footAnim[type].t = 0;

			_phase = kHopAligning;
			break;
		}
		case kHopAligning:
		{
			bool done = this->updateFoot(dt, _whichFootBehind);
			if (done) {
				_phase = kHopHopping;
			}
			break;
		}
		case kHopHopping:
		{
			_t += dt;

			float diff = _currSwingDuration + _contactDuration + 0.1f;
			//float a = (1 - _alpha) / (1 + _alpha) * _contactDuration;


			if (_t > diff && _currSwingLength > 0 && !_footAnim[0].animating && !_footAnim[1].animating) {
				//cout << _swingDuration << " " << _contactDuration << " " << diff << endl;
				_t = 0;
				_footActive[0] = true;
				_footActive[1] = true;
					
				_currSwingDuration = clamp(_currSwingDuration, 0.1f, 0.9f);

				if (_currSwingDuration > 0) {
					_footAnim[0].swingDuration = _currSwingDuration;
					_footAnim[0].swingLength = _currSwingLength;

					_footAnim[1].swingDuration = _currSwingDuration;
					_footAnim[1].swingLength = _currSwingLength;

					_foot[0]->toPos.z = _foot[0]->pos.z - _footAnim[0].swingLength;
					_foot[1]->toPos.z = _foot[1]->pos.z - _footAnim[1].swingLength;

					_footAnim[0].t = 0;
					_footAnim[1].t = 0;

					_footAnim[0].t0 = 0;
					_footAnim[1].t0 = 0;

					_footAnim[0].t1 = 0;
					_footAnim[1].t1 = 0;

					_footAnim[0].animating = true;
					_footAnim[1].animating = true;
				}
			}

			if (_footActive[0]) {
				bool done = this->updateFoot(dt, FOOT_LEFT);
				if (done) {
					_footActive[0] = false;
				}
			}
			if (_footActive[1]) {
				bool done = this->updateFoot(dt, FOOT_RIGHT);
				if (done) {
					_footActive[1] = false;
				}
			}
			
			break;
		}
	}
}

bool Hop::updateFoot(double dt, int type) {
	//FootDrawable *foot = _foot[type];

	//if (!_footAnim[type].animating) {				
	//	if (_currSwingDuration > 0) {
	//		_footAnim[type].swingDuration = _currSwingDuration;
	//		_footAnim[type].swingLength = _currSwingLength;

	//		foot->toPos.z = foot->lastPos.z + _footAnim[type].swingLength;
	//	}
	//}

	bool done = FootAnimator::updateFoot(dt, type);
	
	if (done) {
		_footAnim[type].animating = false;
		_footAnim[type].doneAnimating = true;
	}
	//return true;
	return done;
}
