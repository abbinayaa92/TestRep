#include "TipToe.h"

TipToe::TipToe() : _t(0) {

}

TipToe::~TipToe() {
}

void TipToe::reset() {
	FootAnimator::reset();
	_footActive[0] = false;
	_footActive[1] = false;
}

void TipToe::update(double dt) {

}

bool TipToe::updateFoot(double dt, int type) {
	FootDrawable *foot = _foot[type];

	FootAnim& a = _footAnim[type];

	if (a.animating) {
	}

	return true;
}