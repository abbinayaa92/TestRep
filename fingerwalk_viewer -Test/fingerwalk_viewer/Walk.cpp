#include "Walk.h"
#include <iostream>

using namespace std;

float walk_y[] = {
0.911133f, 
0.929688f, 
0.928711f, 
0.927734f, 
0.926758f, 
0.925781f, 
0.924805f, 
0.923828f, 
0.921875f, 
0.920898f, 
0.919922f, 
0.919922f, 
0.917969f, 
0.915039f, 
0.90918f, 
0.896484f, 
0.891602f,
};

float walk_t[] = {
4.71752f, 
5.03539f,
5.07538f,
5.07555f,
5.0954f, 
5.13538f, 
5.1557f, 
5.19539f, 
5.23538f, 
5.23556f,
5.27537f,
5.29537f,
5.31538f,
5.31556f,
5.33538f,
5.35539f,
5.3754f
};

Walk::Walk() :
_t(0),
_contactDuration(0.4f),
_alpha(0.1f),
_whichFoot(0), // left foot first
_leftFootActive(false),
_rightFootActive(true) 
{
}

Walk::~Walk() {
}

void Walk::reset() {
	FootAnimator::reset();
	cout << "[Walk] reset" << endl;
	_t = 0;
	_leftFootActive = false;
	_rightFootActive = false;
}

void Walk::update(double dt) {
	FootAnimator::update(dt);

	//cout << "contact = " << _contactDuration << " swing = " << _currSwingLength << endl;
	//if (_contactDuration <= 0 || _currSwingLength <= 0) return;

	_t += dt;

	float a = (1 - _alpha) / (1 + _alpha);
	
	float diff = a * _contactDuration + 0.1f;
	
	// adjust swing duration
	float s = (1 - 3 * _alpha) / (1 + _alpha);

	_swingDuration = s * _contactDuration;

	if (_t > diff && _currSwingLength > 0) {
		_t = 0; // reset

		if (_whichFoot == FOOT_LEFT) {
			_leftFootActive = true;
		}
		else {			
			_rightFootActive = true;			
		}

		_whichFoot = (_whichFoot + 1) % 2;
	}	

	if (_leftFootActive) {
		bool done = this->updateFoot(dt, FOOT_LEFT);

		if (done) {			
			_leftFootActive = false;
		}
	}
	if (_rightFootActive) {
		bool done = this->updateFoot(dt, FOOT_RIGHT);
		if (done) {			
			_rightFootActive = false;
		}
	}

	//if (_footAnim[1].doneAnimating && _swingDuration > 0) {
	//	_currSwingDuration = _swingDuration;
	//	_currSwingLength = _swingLength;
	//	_footAnim[1].doneAnimating = false;
	//}
}

bool Walk::updateFoot(double dt, int type) {
	FootDrawable *foot = _foot[type];
	
	if (!_footAnim[type].animating) {
		if (_currSwingDuration > 0) {
			_footAnim[type].swingDuration = _currSwingDuration;
			_footAnim[type].swingLength = _currSwingLength;

			int other = (type + 1) % 2;			
			foot->toPos.z = _foot[other]->toPos.z - 0.5f * _footAnim[type].swingLength;						

			_footAnim[type].t = 0; // reset
			_footAnim[type].t0 = 0;
			_footAnim[type].t1 = 0;
			_footAnim[type].animating = true;
		}		
		//cout << "init foot " << type << endl;
	}	

	FootAnim& a = _footAnim[type];
	
	if (a.animating) {		
		
		if (a.t0 <= 1) {
			float takeOffDuration = 0.03f;

			foot->rot.x = a.t0 * 50;

			if (a.t0 >= 1) {
				a.t0 = 2; // mark as finished
				a.takeoffAngle = 50;
				a.landingAngle = 35;
			}
			else {
				a.t0 += float(dt / takeOffDuration);

				if (a.t0 > 1) {
					a.t0 = 1;
				}
			}
		}
		else if (a.t <= 1) {
			foot->rot.x = (1 - a.t) * a.takeoffAngle + a.t * -a.landingAngle;
			//cout << foot->rot.x << endl;
			FootAnimator::updateFoot(dt, type);							
		}
		else if (a.t1 <= 1) {
			float landingDuration = 0.05f;

			foot->rot.x = (1 - a.t1) * -a.landingAngle;

			if (a.t1 >= 1) {
				a.t1 = 2; // mark as finished
				a.animating = false;
				a.doneAnimating = true;
				foot->rot.x = 0; // force to zero at the end of animation

				return true;
			}
			else {
				a.t1 += float(dt / landingDuration);

				if (a.t1 > 1) {
					a.t1 = 1;
				}
			}			
		}
		
		return false;
	}
	

	
	return true;
}
