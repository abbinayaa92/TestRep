#include "animation/Animation.h"
#include "App2.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/IpEndpointName.h"
#include "ip/UdpSocket.h"
#include <iostream>
#include <fstream>
#include <include/glut.h>
#include "Globals.h"
#include <GLUtils/GLUtils.h>
#include <string>
#include <sstream>
#include <GLUtils/GLCamera.h>

#define clamp(val, a, b) max(min(val, b), a)
#define USE_HISTORY 0
#define USE_ANIMATION 1

// scaling factor from touch space to animation space
static const float UNITSCALE_Y = 8.0f;
static const float UNITSCALE_X = 6.0f;

using namespace std;

App2 *App2::_instance = NULL;

App2::~App2() {
}

bool App2::init(bool online) {
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	// set up callback methods
	glutDisplayFunc(App2::displayCallback);
	glutIdleFunc(App2::idleCallback);
	glutReshapeFunc(App2::reshapeCallback);
	glutMouseFunc(App2::mouseCallback);
	glutMotionFunc(App2::motionCallback);
	//glutPassiveMotionFunc(onMouseMovedPassive);
	glutKeyboardFunc(App2::keyboardCallback);

	_glWindow = new GLWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	_online = online;

	if (!_online) {
	}

	//_outfile.open("../data/inputs/output.txt");
	//if (!_outfile.is_open()) {
	//	cout << "Can't open the output file." << endl;
	//	return false;
	//}

#if LOG_DATA
	_outfile.open("../data/inputs/output.txt");
	if (!_outfile.is_open()) {
		cout << "Can't open the output file." << endl;
		return false;
	}
#endif

	_glWindow->addDrawable(this);
	_timer.restart();
	_lastTime = _currTime = _timer.timeEllapsed();
	_elapsedTime = -1;

	_footPrints.push_back(&_leftFootPrints);
	_footPrints.push_back(&_rightFootPrints);
	_footQ.push_back(&_leftFootQ);
	_footQ.push_back(&_rightFootQ);

	_foot.resize(2);
	_foot[0] = _foot[1] = NULL;
	_prevFoot.resize(2);
	_prevFoot[0] = _prevFoot[1] = NULL;

	_footWalkAnim.resize(2);
	_prevFootWalkAnim.resize(2);
	_footWalkAnimQueue.resize(2);
	_footDrawable.resize(2);
	_footHistory.resize(2);
	_footHistoryItr.resize(2);
	_footHistoryItrStart.resize(2);

	_footDrawable[FOOT_LEFT].pos = _footDrawable[FOOT_LEFT].lastPos = _footDrawable[FOOT_LEFT].toPos =  Point3d(-0.5f, 0, UNITSCALE_Y);
	_footDrawable[FOOT_RIGHT].pos = _footDrawable[FOOT_RIGHT].lastPos = _footDrawable[FOOT_RIGHT].toPos = Point3d(0.5f, 0, UNITSCALE_Y);
	_firstStep = true;
	_startWalking = false;
	_usingHistory = false;

#if USE_ANIMATION
	_animation = new Animation(&_footDrawable[0], &_footDrawable[1]);
	_glWindow->addDrawable(_animation);
#endif
	return true;
}

bool App2::readDataFromInputStream(istream& in) {
	return false;
}

void App2::quit() {
	// save some data?
#if LOG_DATA
	vector<FootContact>::iterator i;
	for (i = _contacts.begin(); i != _contacts.end(); ++i) {
		const FootContact &c = *i;

		_outfile << c.x << ", " << c.y << ", " << c.radius << ", " << c.angle << endl;
	}

	_outfile.close();
	//FootHistory::iterator i = _footHistory[0].begin();
	//FootHistory::iterator j = _footHistory[1].begin();
	//for (; i != _footHistory[0].end(); ++i, ++j) {
	//	const FootRecord &left = *i;
	//	const FootRecord &right = *j;

	//	_outfile << left.pos.x << " " << left.pos.y << " " << left.pos.z << " " << right.pos.x << " " << right.pos.y << " " << right.pos.z << endl;
	//}

	//_outfile.close();
#endif;
}

void App2::ProcessMessage(const ReceivedMessage& m, const IpEndpointName& remoteEndPoint) {
	try {
		if (strcmp(m.AddressPattern(), "/reqid") == 0) {
			char temp[16];
			remoteEndPoint.AddressAsString(temp);
			cout << "receiving id request from: " << temp << " on port: " << remoteEndPoint.port << endl;
			UdpTransmitSocket transmitSocket(IpEndpointName(remoteEndPoint.address, PORT_UDP_IN+1));

			char buffer[128];
			OutboundPacketStream p(buffer, 128);
			p << BeginBundleImmediate
				<< BeginMessage("/ackid")
				<< 1
				<< EndMessage
				<< EndBundle;
			transmitSocket.Send(p.Data(), p.Size());
		}
		else if (strcmp(m.AddressPattern(), "/foot") == 0) {
			ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
			int stepId = (arg++)->AsInt32();
			int phase = (arg++)->AsInt32();
			int type = (arg++)->AsInt32();
			float x = (arg++)->AsFloat();
			float y = (arg++)->AsFloat();
			float radius = (arg++)->AsFloat();
			float timestamp = (arg++)->AsFloat();

			_userIsTouching = true;

			FootContact c(x, y, radius, timestamp);

			if (type == 0) { // left foot
				if (phase == 1) { // TOUCH_BEGAN phase
					FootStepModel m;
					m.stepId = stepId;
					m.down = true;
					m.drag.push_back(c);

					if (_firstStep) {
						_footStartTime = c.timestamp; // offset to the first step
						_firstStep = false;
						_firstStepId = stepId;
					}

					m.startTime = c.timestamp - _footStartTime;

					if (!_leftFootPrints.empty()) {
						FootStepModel &last = _leftFootPrints.back();
						last.down = false;
					}

					if (_leftFootPrints.size() > MAX_STEPS_HISTORY) {
						_leftFootPrints.pop_front();
					}

					_leftFootPrints.push_back(m); // push into history
					_leftFootQ.push_back(&_leftFootPrints.back()); // push into immediate process queue

					cout << "left Q = " << _leftFootQ.size() << endl;
				}
				else if (phase == 2) { // TOUCH_MOVED
					//cout << "back" << endl;
					if (!_leftFootPrints.empty()) {
						FootStepModel &m = _leftFootPrints.back();
						m.drag.push_back(c);
					}
				}
				else if (phase == 4) { // TOUCH_ENDED
					if (!_leftFootPrints.empty()) {
						FootStepModel &m = _leftFootPrints.back();
						m.drag.push_back(c);
						m.endTime = c.timestamp - _footStartTime;
						m.down = false;
						//cout << "left ended" << endl;
					}
				}

#if LOG_DATA
				c.angle = -1000;
				_contacts.push_back(c);
#endif
			}
			else if (type == 1) { // right foot
				if (phase == 1) { // TOUCH_BEGAN phase
					FootStepModel m;
					m.stepId = stepId;
					m.down = true;
					m.drag.push_back(c);
					m.startTime = c.timestamp - _footStartTime;

					if (!_rightFootPrints.empty()) {
						FootStepModel &last = _rightFootPrints.back();
						//m.endTime = c.timestamp - _footStartTime;
						last.down = false;
					}

					if (_rightFootPrints.size() > MAX_STEPS_HISTORY) {
						_rightFootPrints.pop_front();
					}

					_rightFootPrints.push_back(m); // push into history
					_rightFootQ.push_back(&_rightFootPrints.back()); // push into immediate process queue

					cout << "right Q = " << _rightFootQ.size() << endl;
				}
				else if (phase == 2) { // TOUCH_MOVED
					if (!_rightFootPrints.empty()) {
						FootStepModel &m = _rightFootPrints.back();
						m.drag.push_back(c);
					}
				}
				else if (phase == 4) { // TOUCH_ENDED
					if (!_rightFootPrints.empty()) {
						FootStepModel &m = _rightFootPrints.back();
						m.drag.push_back(c);
						m.endTime = c.timestamp - _footStartTime;
						m.down = false;
						//cout << "right ended" << endl;
					}
				}
			}

			//cout << stepId << ", " << x << ", " << y << " -- " << timestamp << endl;
		}
	}
	catch (osc::Exception& e) {
		// any parsing errors such as unexpected argument types, or
		// missing arguments get thrown as exceptions.
		cout << "error while parsing message: "
			<< m.AddressPattern() << ": " << e.what() << endl;
	}
}

void App2::onDisplay() {
	glPushMatrix();
	{
		//float dt = 0.02f;
		_currTime = _timer.timeEllapsed();
		double dt = _currTime - _lastTime;

		update(dt);
		render();

		_lastTime = _currTime;
	}
	glPopMatrix();
}

void App2::onIdle() {
	//ensureMainWindowSelected();
	glutPostRedisplay();
}

void App2::onReshape(int w, int h) {
	glViewport(0, 0, w, h);

	_glWindow->onResize(w, h);
}

void App2::onMouse(int button, int state, int x, int y) {
	int btn = MOUSE_LBUTTON;
	int evt = MOUSE_UP;

	if (button == GLUT_RIGHT_BUTTON) btn = MOUSE_RBUTTON;
	else if (button == GLUT_MIDDLE_BUTTON) btn = MOUSE_MBUTTON;
	else if (button == GLUT_WHEEL_DOWN) btn = MOUSE_WHEEL_DOWN;
	else if (button == GLUT_WHEEL_UP) btn = MOUSE_WHEEL_UP;

	if (state == GLUT_DOWN) evt = MOUSE_DOWN;

	_glWindow->onMouseEvent(evt, btn, x, y);
}

void App2::onMouseMoved(int x, int y) {
	_glWindow->onMouseEvent(MOUSE_DRAG, -1, x, y);
}

#if LOG_DATA
float angle = 0;
int angleSign = 1;
#endif

void App2::onKeyEvent(int key) {
#if LOG_DATA
	if (key == (int)'z') {
		cout << angle << endl;
		FootContact &c = _contacts.back();
		c.angle = angle;

		angle += angleSign * 15;
		if (fabs(angle) >= 45) {
			angleSign *= -1;
		}
	}
#endif
}

void App2::updateCamera(double dt) {
	//// move camera to follow target
	//GLCamera *cam = _glWindow->getCamera();
	//Vector3d target;
	//float dz = (_footDrawable[FOOT_LEFT].pos.z - cam->getTarget().z);
	////cout << dz << endl;

	//if (fabs(dz) > 0.1) {
	//	target.setZ(dz * dt);
	//	cam->translateTarget(target);
	//}
}

void App2::update(double dt) {
	if (_startWalking) {
		_elapsedTime += dt;

		this->updateCamera(dt);
	}

	if (_usingHistory) {
		if (_userIsTouching) {
			_usingHistory = false;
			// TODO: clear queue?
			if (_foot[FOOT_LEFT]) _foot[FOOT_LEFT]->down = false;
			if (_foot[FOOT_RIGHT]) _foot[FOOT_RIGHT]->down = false;
			//_footQ[FOOT_LEFT]->pop_front();
			//_footQ[FOOT_RIGHT]->pop_front();

			_footHistory[FOOT_LEFT].clear();
			_footHistory[FOOT_RIGHT].clear();
			return;
		}
		this->updateFootUsingHistory(dt, FOOT_LEFT);
		this->updateFootUsingHistory(dt, FOOT_RIGHT);

		_historyElapsedTime += dt;

		if (_footHistoryItr[FOOT_LEFT] == _footPrints[FOOT_LEFT]->end() &&
			_footHistoryItr[FOOT_RIGHT] == _footPrints[FOOT_RIGHT]->end())
		{
			_footHistoryItr[FOOT_LEFT] = _footHistoryItrStart[FOOT_LEFT];
			_footHistoryItr[FOOT_RIGHT] = _footHistoryItrStart[FOOT_RIGHT];
			_historyElapsedTime = -0.5;
			_historyOffsetTime = _elapsedTime;
		}
	}
	else {
		this->updateFoot(dt, FOOT_LEFT);
		this->updateFoot(dt, FOOT_RIGHT);
	}

	this->updateAnimation(dt);

#if USE_ANIMATION
	//_animation->DoUpdateStep(); // TODO:
#endif
}

const float Rmin = 6.25;
const float Rmax = 10.0;
const float Dymax = (Rmax - Rmin) / 100.0;
const float Dxmax = (12.0 - Rmin) / 100.0;

void App2::updateFoot(double dt, int type) {
	if (_foot[type] != NULL) {
		if (_foot[type]->down) {
			if (!_footWalkAnim[type].animating) {
				//if (!_foot[type]->drag.empty() && ((int)_foot[type]->drag.size()) > 2) {
				//	FootContact &c0 = _foot[type]->drag.front();
				//	FootContact &c1 = _foot[type]->drag.back();

				//	float thetaNormalized = (c1.radius - Rmin) / (Rmax - Rmin);
				//	_footDrawable[type].rot.x = (1 - thetaNormalized) * 70 - 10;

				//	//if (thetaNormalized > 0.8) {
				//	//	int sizeToKeep = 5;
				//	//	int length = _foot[type]->drag.size();
				//	//	std::vector<FootContact>::iterator it = _foot[type]->drag.begin();
				//	//	if (length > sizeToKeep) {
				//	//		_foot[type]->drag.erase(it, it + (length - sizeToKeep));
				//	//	}
				//	//}

				//	float dy = c1.y - c0.y;
				//	float dx = c1.x - c0.x;
				//	float posy = c1.y * UNITSCALE_Y + _origin.y;
				//	float posx = (c1.x - 0.5f) * UNITSCALE_X;
				//
				//	//Point3d oldPos = _footDrawable[type].pos;

				//	if (fabs(dy) > fabs(dx)) {
				//		_footDrawable[type].rot.z = 0;
				//		if (fabs(dy) > Dymax) {
				//			//_footDrawable[type].pos.z = _footDrawable[type].lastPos.z + dy * 3;
				//			_footDrawable[type].pos.z = posy;
				//		}
				//	}
				//	else {
				//		_footDrawable[type].rot.x = 0;
				//		if (fabs(dx) > Dxmax) {
				//			//_footDrawable[type].pos.x = _footDrawable[type].lastPos.x + dx * 5;
				//			_footDrawable[type].pos.x = posx;
				//		}
				//		else {
				//			_footDrawable[type].rot.z = -dx * 500;
				//		}
				//	}
			}

#if USE_HISTORY
				FootRecord r;
				r.angle = _footDrawable[type].rot.x;
				r.dy = _footDrawable[type].pos.z - oldPos.z;

				_footHistory[type].push_back(r);
#endif
			//}

			//_footDrawable[type].pos.y = c.y;
			//cout << theta << endl;
		}
		else {
			_prevFoot[type] = _foot[type];
			_foot[type] = NULL;
			_footDrawable[type].radius = 0;
			_footDrawable[type].rot.x = 0;

			_footQ[type]->pop_front();

			int other = (type + 1) % 2;
			if (_prevFoot[type]->drag.back().y <= 0.1 && _foot[other] == NULL) {
				cout << "SCROLL" << endl;

				_usingHistory = true;
				_userIsTouching = false;
				_origin.y -= UNITSCALE_Y;

				// search starting point in history
				_footHistoryItr[other] = _footPrints[other]->begin();
				_footHistoryItr[type] = _footPrints[type]->begin();
				// find next step after other foot in history
				for (;_footHistoryItr[type] != _footPrints[type]->end(); ++_footHistoryItr[type]) {
					const FootStepModel &m1 = *_footHistoryItr[type];
					const FootStepModel &m2 = *_footHistoryItr[other];
					if (m1.startTime > m2.startTime) break;
				}

				_footHistoryItrStart[type] = _footHistoryItr[type];
				_footHistoryItrStart[other] = _footHistoryItr[other];

				cout << "start from = " << _footHistoryItrStart[other]->stepId << ", = " << _footHistoryItrStart[type]->stepId << endl;
				_historyStartTime = (*_footHistoryItrStart[other]).startTime;
				_historyElapsedTime = 0;
				_historyOffsetTime = _elapsedTime;

				//_footHistory[FOOT_LEFT].clear();
				//_footHistory[FOOT_RIGHT].clear();
			}
			else if (_foot[type] == NULL && _foot[other] == NULL) {
				//_footHistory[type].clear();
				//_footHistory[other].clear();
			}
#if USE_HISTORY
			int other = (type + 1) % 2;
			if (_prevFoot[type]->drag.back().y > 0.8 && _foot[other] == NULL) {
				// use history to repeat walk cycle
				cout << "USE HISTORY!" << endl;
				_usingHistory = true;
				_userIsTouching = false;
				_footHistoryItr[type] = _footHistory[type].begin();
				_footHistoryItr[other] = _footHistory[other].begin();
			}
			else if (_foot[type] == NULL && _foot[other] == NULL) {
				_footHistory[type].clear();
				_footHistory[other].clear();
			}
#endif
		}
	}
	else {
		if (!_footQ[type]->empty()) {
			FootStepModel &foot = *_footQ[type]->front();
			if (!foot.down) {
				// HACK: this is a quick fix to the foot queue stuck problem.
				//       just pop it out if its not the valid foot
				cout << "STRANGE! the foot is NULL even before it was popped out from the queue" << endl;
				_footQ[type]->pop_front();

				cout << _elapsedTime << " >= " << foot.startTime << endl;
				return;
			}

			if (!_startWalking && foot.stepId == _firstStepId) {
				_startWalking = true;
				_elapsedTime = -0.1;
			}
			//cout << "elapsed time = " << _elapsedTime << ", foot start = " << foot.startTime << endl;
			if (_elapsedTime >= foot.startTime) {
				_foot[type] = &foot;
				_footDrawable[type].lastPos = _footDrawable[type].pos;

				//if (type == 0) cout << "FOOT LEFT" << endl;
				//else           cout << "FOOT_RIGHT" << endl;
				//cout << "foot id = " << _foot[type]->stepId << endl;

				if (_prevFoot[type] != NULL) {
					int prevSize = (int)_prevFoot[type]->drag.size();
					int currSize = (int)_foot[type]->drag.size();
					//cout << "p = " << _prevFoot[type] << " prev = " << prevSize << " curr = " << currSize << endl;
					if (prevSize > 0 && currSize > 0) {
						FootAnim a;
						const FootContact &c1 = _prevFoot[type]->drag.back();
						const FootContact &c2 = _foot[type]->drag.front();

						float swingDuration = _foot[type]->startTime - _prevFoot[type]->endTime;

						a.oldPos = Point3d(c1.x, 0, c1.y);
						a.newPos = Point3d(c2.x, 0, c2.y);

						a.swingDuration = clamp(swingDuration, 0.1, 1.5);

						_footWalkAnimQueue[type].push_back(a);

						//if (type == 0) cout << "PUSH Anim LEFT"<< endl;
						//else if (type == 1) cout << "PUSH Anim RIGHT" << endl;
					}
				}
			}
		}
	}
}

void App2::updateAnimation(double dt) {
	this->updateFootAnimation(dt, FOOT_LEFT);
	this->updateFootAnimation(dt, FOOT_RIGHT);
}

void App2::updateFootAnimation(double dt, int type) {
	if (!_footWalkAnimQueue[type].empty()) {
		if (!_footWalkAnim[type].animating) {
			_footWalkAnim[type] = _footWalkAnimQueue[type].front();
			_footWalkAnim[type].animating = true;
			_footWalkAnim[type].t = 0;
			_footWalkAnimQueue[type].pop_front();
			// QUICKFIX: issue #xxx origin changed while one foot is animating, causing it to jump far away.
			// IF bug still persists, separate _originAnim for left and right
			_originAnim = _origin;
		}
	}

	FootAnim &currAnim = _footWalkAnim[type];
	if (currAnim.animating) {
		float dy = 0;
		float dx = 0;
		int other = (type + 1) % 2;

		//if (_prevFootWalkAnim[other].t > 0) {
		//	// we know other foot previous location, take that as a reference,
		//	// so they are always relative to each other.
		//	float other_dy = (currAnim.newPos.z - _prevFootWalkAnim[other].newPos.z) * UNITSCALE_Y;
		//	float other_dx = (currAnim.newPos.x - _prevFootWalkAnim[other].newPos.x) * UNITSCALE_X;

		//	float expected_posy = _footDrawable[other].lastPos.z + other_dy;
		//	float expected_posx = _footDrawable[other].lastPos.x + other_dx;

		//	dy = -(expected_posy - _footDrawable[type].lastPos.z);
		//	dx = expected_posx - _footDrawable[type].lastPos.x;
		//}
		//else {
		//	// use self as reference
		//	dy = -(currAnim.newPos.z - currAnim.oldPos.z) * UNITSCALE_Y;
		//	dx = (currAnim.newPos.x - currAnim.oldPos.x) * UNITSCALE_X;
		//}

		//if (dy >= 0) {
		//	_prev_dy = dy;
		//	_prev_dx = dx;
		//}
		//else {
		//	if (fabs(dy) > 0.5) { // moving back half the screen, normalized value
		//		dy = _prev_dy;
		//		dx = _prev_dx;

		//		_prevFootWalkAnim[other].newPos.z -= (dy / UNITSCALE_Y);
		//		_prevFootWalkAnim[other].newPos.x -= (dx / UNITSCALE_X);
		//		currAnim.swingDuration = _prevFootWalkAnim[type].swingDuration;
		//	}
		//	else {
		//
		//	}
		//}

		// use absolute space
		float posx = (currAnim.newPos.x - 0.5) * UNITSCALE_X;
		float posy = currAnim.newPos.z * UNITSCALE_Y + _originAnim.y;

		dy = posy - _footDrawable[type].lastPos.z;
		dx = posx - _footDrawable[type].lastPos.x;

		const float maxHeight = 0.5f; // fix height?
		_footDrawable[type].pos.y = maxHeight * sinf(PI * currAnim.t);
		//_footDrawable[type].pos.z = _footDrawable[type].lastPos.z - (currAnim.t * dy);
		_footDrawable[type].pos.z = _footDrawable[type].lastPos.z + (currAnim.t * dy);
		_footDrawable[type].pos.x = _footDrawable[type].lastPos.x + (currAnim.t * dx);

		//FootRecord r;
		//r.pos = _footDrawable[type].pos;
		//_footHistory[type].push_back(r);

		if (currAnim.t >= 1) {
			currAnim.animating = false;
			_prevFootWalkAnim[type] = currAnim;
		}
		else {
			currAnim.t += dt / currAnim.swingDuration;

			if (currAnim.t > 1) {
				currAnim.t = 1;
			}
		}
	}
}

void App2::updateFootUsingHistory(double dt, int type) {
	if (_footHistoryItr[type] == _footPrints[type]->end()) {
		return;
		//_footHistoryItr[type] = _footHistoryItrStart[type];
	}

	if (_footHistoryItr[type] != _footPrints[type]->end()) {
		const FootStepModel& m = *_footHistoryItr[type];
		if (_footQ[type]->empty()) {
			//cout << _historyElapsedTime << " > " << (m.startTime - _historyStartTime) << endl;

			if (_historyElapsedTime > (m.startTime - _historyStartTime)) {
				//if (type == 0) cout << "LEFT HISTORY" << _historyElapsedTime << endl;
				//else if (type == 1) cout << "RIGHT HISTORY" << _historyElapsedTime << endl;
				FootStepModel s;
				s.stepId = 100000 + m.stepId;
				s.drag.push_back(m.drag.front());
				s.startTime = m.startTime + _historyOffsetTime;
				s.endTime = m.endTime + _historyOffsetTime;
				s.down = true;

				if (_footHistory[type].size() > MAX_STEPS_HISTORY) {
					_footHistory[type].pop_front();
				}

				_footHistory[type].push_back(s);
				//cout << (&_footHistory[type].back()) << endl;

				_footQ[type]->push_back(&_footHistory[type].back());
				//_footHistoryItr[type]++;
			}
		}
		else {
			FootStepModel &s = *_footQ[type]->front();
			int size1 = (int)s.drag.size();
			int size2 = (int)m.drag.size();

			if (size1 >= size2) {
				s.drag.push_back(m.drag.back());
				s.down = false;
				_footHistoryItr[type]++;
			}
			else {
				if (size1 > 0) {
					const FootContact &c = m.drag.at(size1);

					if (_historyElapsedTime > (c.timestamp - _historyStartTime - _footStartTime)) {
						s.drag.push_back(c);
					}
				}
			}
		}
	}

	this->updateFoot(dt, type);
	//if (_footHistoryItr[type] == _footHistory[type].end()) {
	//	_footHistoryItr[type] = _footHistory[type].begin();
	//}

	//if (_footHistoryItr[type] != _footHistory[type].end()) {
	//	const FootRecord &r = *_footHistoryItr[type];
	//	_footDrawable[type].pos = r.pos;
	//	_footDrawable[type].rot.x = r.angle;
	//	_footHistoryItr[type]++;
	//}
}

void App2::render() {
	//_glWindow->draw();
}

void App2::draw() {
	Color leftFootColor(1.0f, 0.2f, 0.2f, 1.0f);
	Color rightFootColor(0.2f, 1.0f, 0.2f, 1.0f);

	this->drawFoot(_footDrawable[FOOT_LEFT].pos , _footDrawable[FOOT_LEFT].rot, leftFootColor, _footDrawable[FOOT_LEFT].radius * 0.02);
	this->drawFoot(_footDrawable[FOOT_RIGHT].pos, _footDrawable[FOOT_RIGHT].rot, rightFootColor, _footDrawable[FOOT_RIGHT].radius * 0.02);
}

void App2::drawFoot(Point3d center, Vector3d rot, Color color, float radius) {
	float footWidth = 0.25f;
	float footLength = 0.5f;
	float footThickness = 0.1f;

	Point3d boxMin(-0.5f * footWidth + center.x, center.y                , -0.5f * footLength + center.z);
	Point3d boxMax( 0.5f * footWidth + center.x, center.y + footThickness,  0.5f * footLength + center.z);

	glColor3f(color.r, color.g, color.b);

	glPushMatrix();
	{
		Point3d t = center;
		if (rot.x > 0) {
			t.z -= footLength * 0.5f;
		}
		else if (rot.x < 0) {
			t.z += footLength * 0.5f;
		}
		glTranslatef(t.x, t.y, t.z);
		glRotatef(-rot.x, 1, 0, 0);
		glRotatef(rot.y, 0, 1, 0);
		glRotatef(rot.z, 0, 0, 1);
		glTranslatef(-t.x, -t.y, -t.z);
		GLUtils::drawWireFrameBox(boxMin, boxMax);
	}
	glPopMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
	GLUtils::drawDisk(radius, Vector3d(center.x, 0.001, center.z), Point3d(0, 1, 0));
}