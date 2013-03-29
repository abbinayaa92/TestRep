#include "FootAnimator.h"
#include <iostream>

using namespace std;

FootAnimator::FootAnimator() : 
_swingDuration(0), 
_swingLength(0), 
_currSwingDuration(0.5),
_currSwingLength(0),
_stopped(false)
{
	_foot[0] = NULL;
	_foot[1] = NULL;
}

FootAnimator::~FootAnimator() {
}

void FootAnimator::reset() {
	_footAnim[0].doneAnimating = true;
	_footAnim[1].doneAnimating = true;
	_stopped = false;
}

void FootAnimator::update(double dt) {
	//if (_foot[0])  this->updateFoot(dt, 0);
	//if (_foot[1]) this->updateFoot(dt, 1);
	//cout << _footAnim[0].doneAnimating << " " << _footAnim[1].doneAnimating << " " << _swingDuration << endl;
	if (_footAnim[0].doneAnimating && _footAnim[1].doneAnimating && _swingDuration > 0) {
		_currSwingDuration = _swingDuration;
		_currSwingLength = _swingLength;
		_footAnim[0].doneAnimating = false;
		_footAnim[1].doneAnimating = false;
	}
}

bool FootAnimator::stop() {
	_currSwingLength = 0;
	_stopped = true;
	
	if (!_footAnim[0].animating && !_footAnim[1].animating) {
		cout << "stop!" << endl;
		return true;
	}

	return false;
}

bool FootAnimator::updateFoot(double dt, int type) {
	FootDrawable *foot = _foot[type];
	//if (!_footAnim[type].animating) {
	//	if (_currSwingDuration > 0) {
	//		_footAnim[type].swingDuration = _currSwingDuration;
	//		_footAnim[type].swingLength = _currSwingLength;

	//		foot->toPos.z = foot->lastPos.z + _footAnim[type].swingLength;

	//		_footAnim[type].t = 0; // reset
	//		_footAnim[type].animating = true;
	//	}		
	//	//cout << "init foot " << type << endl;
	//}
	//else {
	if (_footAnim[type].animating) {
		float t = _footAnim[type].t;
		if (t <= 1) {
			const float maxHeight = 10;
					
			foot->pos.y = maxHeight * sinf((float)PI * t);
			float dy = float(foot->toPos.z - foot->lastPos.z);
			foot->pos.z = foot->lastPos.z + t * dy;

			if (_footAnim[type].t >= 1) {
				_footAnim[type].t = 2; // mark as finished
				//_footAnim[type].animating = false;
				//_footAnim[type].doneAnimating = true;
				foot->lastPos = foot->pos;
			}
			else {
				_footAnim[type].t += (float)(dt / _footAnim[type].swingDuration);
				
				if (_footAnim[type].t > 1) {
					_footAnim[type].t = 1;
				}

				return false;
			}
		}		
	}

	return true;
}

