#include "App.h"
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

// scaling factor from touch space to animation space
static const float UNITSCALE_Y = 3.0f * 20;
static const float UNITSCALE_X = 5.0f * 20;

using namespace std;

App *App::_instance = NULL;

App::~App() {
}

bool App::init(bool online) {
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	// set up callback methods
	glutDisplayFunc(App::displayCallback);
	glutIdleFunc(App::idleCallback);
	glutReshapeFunc(App::reshapeCallback);
	glutMouseFunc(App::mouseCallback);
	glutMotionFunc(App::motionCallback);
	//glutPassiveMotionFunc(onMouseMovedPassive);
	//glutKeyboardFunc(onKeyboard);

	_glWindow = new GLWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	_online = online;

	if (!_online) {
		// load data from file
		//ifstream file;
		//file.open("../data/inputs/walk-small-step-1.txt");
		//if (!file.is_open()) {
		//	cout << "Can't open the input file." << endl;
		//	return false;
		//}


		//bool ok = readDataFromInputStream(file);
		//if (!ok) {
		//	cout << "Error reading file." << endl;
		//	return false;
		//}

		//_stepLItr = _stepsL.begin();
		//_stepRItr = _stepsR.begin();
		//_stepLIndex = -1;
		//_stepRIndex = -1;
	}

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
	_footWalkAnim.resize(2);
	_prevFootWalkAnim.resize(2);
	_footWalkAnimQueue.resize(2);
	_footDrawable.resize(2);
	
	_footDrawable[FOOT_LEFT].pos = _footDrawable[FOOT_LEFT].lastPos = Point3d(-0.5f * 20, 0, UNITSCALE_Y);
	_footDrawable[FOOT_RIGHT].pos = _footDrawable[FOOT_RIGHT].lastPos = Point3d(0.5f * 20, 0, UNITSCALE_Y);
	_firstStep = true;
	_startWalking = false;
	return true;
}

bool App::readDataFromInputStream(istream& in) {
	//string line;
	//while (getline(in, line)) {
	//	stringstream ss(line);
	//	string value;

	//	FootStep step;
	//	for (int i = 0; getline(ss, value, ','); i++) {
	//		//cout << value << ", ";
	//		switch (i) {
	//			case 0: // step count
	//				step.stepId = atoi(value.c_str());
	//				break;
	//			case 1: // phase
	//				step.phase = atoi(value.c_str());
	//				break;
	//			case 2: // left/right foot
	//				step.type = atoi(value.c_str());
	//				break;				
	//			case 3:
	//				step.x = atof(value.c_str());
	//				break;
	//			case 4:
	//				step.y = atof(value.c_str());
	//				break;
	//			case 5:
	//				step.radius = atof(value.c_str());
	//				break;
	//			case 6:
	//				step.timestamp = atof(value.c_str());
	//				break;
	//		}
	//	}

	//	if (step.type == 0) { // left foot
	//		_stepsL.push_back(step);
	//	}
	//	else { // right foot
	//		_stepsR.push_back(step);
	//	}		
	//}
	//cout << "Done reading file." << endl;
	//return true;
	return false;
}

void App::quit() {
	// save some data?
}

void App::ProcessMessage(const ReceivedMessage& m, const IpEndpointName& remoteEndPoint) {
	//cout << "got message!" << endl;
	try {
		if (strcmp(m.AddressPattern(), "/reqid") == 0) {
			char temp[16];
			remoteEndPoint.AddressAsString(temp);
			cout << "receiving id request from: " << temp << " on port: " << remoteEndPoint.port << endl;
			UdpTransmitSocket transmitSocket(IpEndpointName(remoteEndPoint.address, PORT_UDP_IN+1));

			char buffer[1024];
			OutboundPacketStream p(buffer, 1024);
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
						FootStepModel &m = _leftFootPrints.back();
						//m.endTime = c.timestamp - _footStartTime;
						m.down = false;
					}

					if (_leftFootPrints.size() > MAX_STEPS_HISTORY) {
						_leftFootPrints.pop_front();
					}

					_leftFootPrints.push_back(m); // push into history
					_leftFootQ.push_back(&_leftFootPrints.back()); // push into immediate process queue

					//cout << "left prints = " << _leftFootQ.size() << endl;					
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
			}
			else if (type == 1) { // right foot
				if (phase == 1) { // TOUCH_BEGAN phase
					FootStepModel m;
					m.stepId = stepId;
					m.down = true;
					m.drag.push_back(c);
					m.startTime = c.timestamp - _footStartTime;

					if (!_rightFootPrints.empty()) {
						FootStepModel &m = _rightFootPrints.back();
						//m.endTime = c.timestamp - _footStartTime;
						m.down = false;
					}

					if (_rightFootPrints.size() > MAX_STEPS_HISTORY) {
						_rightFootPrints.pop_front();
					}

					_rightFootPrints.push_back(m); // push into history
					_rightFootQ.push_back(&_rightFootPrints.back()); // push into immediate process queue

					//cout << "right prints = " << _rightFootQ.size() << endl;
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
			//if (type == 0) { // left foot
			//	_currStepL.stepId = stepId;
			//	_currStepL.phase = phase;
			//	_currStepL.type = type;
			//	_currStepL.x = x;
			//	_currStepL.y = y;
			//	_currStepL.radius = radius;
			//	_currStepL.timestamp = timestamp;				
			//}
			//else { // right foot
			//	_currStepR.stepId = stepId;
			//	_currStepR.phase = phase;
			//	_currStepR.type = type;
			//	_currStepR.x = x;
			//	_currStepR.y = y;
			//	_currStepR.radius = radius;
			//	_currStepR.timestamp = timestamp;				
			//}
			//if (arg != m.ArgumentsEnd()) throw ExcessArgumentException();

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

void App::onDisplay() {
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

void App::onIdle() {
	//ensureMainWindowSelected();
	glutPostRedisplay();
}

void App::onReshape(int w, int h) {
	glViewport(0, 0, w, h);

	_glWindow->onResize(w, h);
}

void App::onMouse(int button, int state, int x, int y) {
	int btn = MOUSE_LBUTTON;
	int evt = MOUSE_UP;

	if (button == GLUT_RIGHT_BUTTON) btn = MOUSE_RBUTTON;
	else if (button == GLUT_MIDDLE_BUTTON) btn = MOUSE_MBUTTON;
	else if (button == GLUT_WHEEL_DOWN) btn = MOUSE_WHEEL_DOWN;
	else if (button == GLUT_WHEEL_UP) btn = MOUSE_WHEEL_UP;
	
	if (state == GLUT_DOWN) evt = MOUSE_DOWN;

	_glWindow->onMouseEvent(evt, btn, x, y);
}

void App::onMouseMoved(int x, int y) {
	_glWindow->onMouseEvent(MOUSE_DRAG, -1, x, y);
}

void App::updateCamera(double dt) {
	// move camera to follow target
	GLCamera *cam = _glWindow->getCamera();
	Vector3d target;
	float dz = (_footDrawable[FOOT_LEFT].pos.z - cam->getTarget().z);
	//cout << dz << endl;

	if (fabs(dz) > 0.1) {
		target.setZ(dz * dt);
		cam->translateTarget(target);
	}
}

void App::update(double dt) {
	if (_startWalking) {
		_elapsedTime += dt;
		
		this->updateCamera(dt);
	}

	for (int i = 0; i < 2; ++i) {
		if (_foot[i] != NULL) {
			if (_foot[i]->down) {
				// analyze foot roll here?
				if (!_footWalkAnim[i].animating) {
					FootContact &c = _foot[i]->drag.back();
					_footDrawable[i].radius = c.radius;
					// animate heel angle or sliding feet?
				}
			}
			else {
				_prevFoot[i] = _foot[i]; // pass to prev foot
				_foot[i] = NULL;			
				_footDrawable[i].radius = 0; // reset radius
				_footQ[i]->pop_front();

				int other = i == FOOT_LEFT ? FOOT_RIGHT : FOOT_LEFT;
				if (_prevFoot[i]->drag.back().y < 0.2 && _foot[other] == NULL) {
					cout << "SCROLL" << endl;
					// use history to repeat walk cycle
					_usingHistory = true;
					_userIsTouching = false;
					_lastFootTypeOnGround = i;

					_origin.y -= UNITSCALE_Y;

				}
			}
		}
		else {
			if (!_footQ[i]->empty()) {
				FootStepModel &foot = *_footQ[i]->front();
				if (!foot.down) {
				// HACK: this is a quick fix to the foot queue stuck problem.
				//       just pop it out if its not the valid foot
					cout << "STRANGE! the foot is NULL even before it was popped out from the queue" << endl;
					_footQ[i]->pop_front();

					cout << _elapsedTime << " >= " << foot.startTime << endl;
					return;
				}

				if (!_startWalking && foot.stepId == _firstStepId) { // starts with left foot
					_startWalking = true;
					//_elapsedTime = -0.1;
					_elapsedTime = 0;
				}

				// TODO: put elapsed time slightly behind the timestamp, otherwise it will immediately pick up from the queue,
				//       because elapsed time always further ahead then all time stamp in the queue.
				if (_elapsedTime >= foot.startTime) {
					// start analyzing new foot step					
					_foot[i] = &foot;
					
					//if (_prevFoot[i]->stepId != -1) {
					if (_prevFoot[i] != NULL) {
						
						//_foot[i].timeFromPreviousStep = _foot[i].startTime - _prevFoot[i].endTime;
						//_footWalk.push_back(StepPair(_prevFoot[i], _foot[i]));						
						float swingDuration = _foot[i]->startTime - _prevFoot[i]->endTime;
						cout << "swing duration= " << (i == 0 ? "LEFT=" : "RIGHT=" ) << swingDuration << endl;

						int other = (i + 1) % 2;
						if (swingDuration <= 2.0f || (_foot[other] != NULL && _foot[other]->down)) { // otherwise consider users put off their fingers and starts new steps
							FootAnim a;
						
							const FootContact &c1 = _prevFoot[i]->drag.back();
							const FootContact &c2 = _foot[i]->drag.front();
							
							// analyze foot roll
							std::vector<FootContact>::iterator it;
							a.peak_y = 0;						
							int length = _prevFoot[i]->drag.size();
							

							if (length > 100) {
								it = _prevFoot[i]->drag.begin();
								_prevFoot[i]->drag.erase(it, it + (length - 100)); // just take the last 200 drags
							}

							float startTime = _prevFoot[i]->drag.front().timestamp;

							//if (length <= 200) { // max analyze 200 drags each, otherwise, ignore foot displacement							
								int k = 0;
								for (it = _prevFoot[i]->drag.begin(); it != _prevFoot[i]->drag.end(); ++it, ++k) {
									const FootContact &ci = *it;
									float dy = ci.y - _prevFoot[i]->drag.front().y; // shift to origin
									float t = ci.timestamp - startTime;

									if (a.peakend_t <= 0 && dy < 0) {
										a.peakend_t = t;
										a.peakend_idx = k;
									}

									if (dy > a.peak_y) {
										a.peak_y = dy; 
										a.peak_t = t;
										a.peak_idx = k;
									}
								}

								//float contactTime = _prevFoot[i]->drag.back().timestamp - startTime;
								//a.peakT /= contactTime;    // normalize t
								//a.peakEndT /= contactTime; // normalize t

								//cout << a.peak_y << ", " << a.peak_t << ", " << a.peakend_t << endl;
							//}
							float contactDuration = _prevFoot[i]->drag.back().timestamp - startTime;
							a.oldPos = Point3d(c1.x, 0, c1.y);
							a.newPos = Point3d(c2.x, 0, c2.y);
							//cout << _foot[i]->startTime << " - " << _prevFoot[i].endTime << endl;
							
							a.contactDuration = clamp(contactDuration, 0.5, 1.5);
							a.landingDuration = clamp(a.peak_t, 0.1, 1);
							a.swingDuration = clamp(swingDuration - a.contactDuration - a.landingDuration, 0.5, 2);

							_lastSwingDuration = a.swingDuration;

							//cout << "contact duration = " << a.contactDuration << endl;
							_footWalkAnimQueue[i].push_back(a);
							
							cout << "push animation = " << (i == 0 ? "LEFT" : "RIGHT" )<< endl;
						}						
					}
				}
			}			
		}				
	}
	
	this->updateAnimation(dt);

	//if (_usingHistory) {		
	//	if (_userIsTouching) {
	//		_usingHistory = false;
	//	}
	//	else {
	//		_timeSinceLastContact += dt;

	//		if ((_timeSinceLastContact > _lastSwingDuration) {
	//		}
	//	}
	//		// push next step with history data
	//		//if (_lastFootTypeOnGround == FOOT_LEFT) {
	//		//	int s = _rightFootPrints.size();
	//		//	if (s > 2) {
	//		//		s = 2; // max 2 steps before
	//		//	}
	//		//	
	//		//	_footQ[FOOT_RIGHT].push_back(_rightFootPrints[s-1]);
	//		//}	
	//}
}

void App::updateAnimation(double dt) {
	for (int i = 0; i < 2; ++i) {
		// play animation queue
		if (!_footWalkAnimQueue[i].empty()) {
			//cout << (i == 0? "left animating = ":"right animating = ") << _footWalkAnim[i].animating << endl;
			if (!_footWalkAnim[i].animating) {
				_footWalkAnim[i] = _footWalkAnimQueue[i].front();
				_footWalkAnim[i].animating = true;
				_footWalkAnimQueue[i].pop_front();

				float dist = _footWalkAnim[i].newPos.z - _footWalkAnim[i].oldPos.z;
				
				if (dist > 0.6) { // the user reset position
					// QUICKFIX: issue #xxx origin changed while one foot is animating, causing it to jump far away.
					// IF bug still persists, separate _originAnim for left and right
					_originAnim = _origin;
				}				
			}
		}

		FootAnim &currAnim = _footWalkAnim[i];
		if (currAnim.animating) {
			static const float alphaRatio = 45 / 0.02;
			static const float thetaRatio = 75 / 0.06;

			if (currAnim.t0 <= 1) {				
				float takeOffDuration = (currAnim.contactDuration - currAnim.peak_t);
				if (takeOffDuration < 0) takeOffDuration = 0.1f;
				//if (i == 1) cout << "right t0 = " << currAnim.t0 << " , " << takeOffDuration << endl;

				if (currAnim.peak_t > 0) {					
					int size = _prevFoot[i]->drag.size();

					int idx = (int)(currAnim.t0 * (size - 1 - currAnim.peak_idx) + currAnim.peak_idx);
					//cout << "idx= " << idx << endl;
					
					//float limpnessFactor = 1.0f;
					//if (i == 1) limpnessFactor = 0.3f;
					if (idx >= 0 && idx < size) {
						float dy = currAnim.peak_y - (_prevFoot[i]->drag[idx].y - _prevFoot[i]->drag.front().y);
						_footDrawable[i].rot.x = dy * thetaRatio;// * limpnessFactor;
						_footDrawable[i].radius = _prevFoot[i]->drag[idx].radius;
					}
				}
				else {
					_footDrawable[i].rot.x = currAnim.t0 * 10;
				}

				if (currAnim.t0 >= 1) {
					currAnim.t0 = 2; // mark as finished
					currAnim.takeoffAngle = clamp(_footDrawable[i].rot.x, 10, 70);
					currAnim.landingAngle = clamp((currAnim.peak_y * alphaRatio), 10, 45);
				}
				else {
					currAnim.t0 += dt / takeOffDuration;
				
					if (currAnim.t0 > 1) {
						currAnim.t0 = 1;
					}
				}
			}
			else if (currAnim.t <= 1) {
				//if (i == 1) cout << "right t = " << currAnim.t << endl;
				// TODO: the z is flipped, maybe rotate the camera and make the character walk along z positive
				float dy = 0;
				float dx = 0;

				int other = (i + 1) % 2;

				//if (_prevFootWalkAnim[other].t > 0) { // we know previous other foot location, take reference from it, so they always relative to each other
				//	float ddy = (currAnim.newPos.z - _prevFootWalkAnim[other].newPos.z) * UNITSCALE_Y;
				//	float ddx = (currAnim.newPos.x - _prevFootWalkAnim[other].newPos.x) * UNITSCALE_X;
				//	float py = _footDrawable[other].lastPos.z + ddy;
				//	float px = _footDrawable[other].lastPos.x + ddx;
				//	dy = -(py - _footDrawable[i].lastPos.z);
				//	dx = px - _footDrawable[i].lastPos.x;
				//	cout << "other = " << dy << endl;
				//}
				//else {
				//	dy = -(currAnim.newPos.z - currAnim.oldPos.z) * UNITSCALE_Y; // 3 is a ratio between physical distance and distance in virtual environment (OpenGL rendering).						
				//	dx = (currAnim.newPos.x - currAnim.oldPos.x) * UNITSCALE_X;						
				//	cout << "self = " << dy << endl;
				//}


				//if (dy >= 0) {														
				//	_prev_dy = dy;
				//	_prev_dx = dx;	
				//}
				//else {
				//	if (fabs(dy) > 0.5) { // moving back half of screen, normalized value
				//		// use previous values, assuming the feet alternating correctly
				//		dy = _prev_dy;
				//		dx = _prev_dx;
				//		_prevFootWalkAnim[other].newPos.z -= (dy / UNITSCALE_Y);
				//		_prevFootWalkAnim[other].newPos.x -= (dx / UNITSCALE_X);
				//		currAnim.swingDuration = _prevFootWalkAnim[i].swingDuration;
				//	}
				//}
				
				float posx = (currAnim.newPos.x - 0.5) * UNITSCALE_X;
				float posy = currAnim.newPos.z * UNITSCALE_Y + _originAnim.y;
				dy = posy - _footDrawable[i].lastPos.z;
				dx = posx - _footDrawable[i].lastPos.x;

				float maxHeight = min(0.5 * currAnim.swingDuration, 1.0f) * 20;

				_footDrawable[i].pos.y = maxHeight * sinf(PI * currAnim.t);
				_footDrawable[i].pos.z = _footDrawable[i].lastPos.z + (currAnim.t * dy);
				_footDrawable[i].pos.x = _footDrawable[i].lastPos.x + (currAnim.t * dx);
				//if (currAnim.peak_t > 0) {
					
					_footDrawable[i].rot.x = (1 - currAnim.t) * currAnim.takeoffAngle + currAnim.t * -currAnim.landingAngle;
					//float t1 = currAnim.peakEndT - currAnim.peakT;
					//float t2 = 1 - currAnim.peakT;

					//if (currAnim.t < t2) {
					//	int idx = (int)(currAnim.t * _prevFoot[i]->drag.size());
					//	float dy = currAnim.peakY - (_prevFoot[i]->drag[idx].y - _prevFoot[i]->drag.front().y);
					//	_footDrawable[i].rot.x = dy * 60 / 0.08;
					//	//cout << _footDrawable[i].rot.x << endl;
					//}
					//else {
						//_footDrawable[i].rot.x = (1 - ((currAnim.t - t2)/ currAnim.peakT)) * currAnim.peakY * -alphaRatio;
						//cout << (1 - ((currAnim.t - t2)/ currAnim.peakT)) << endl;
						//cout << currAnim.t << ", " << t2 << ", " << currAnim.peakT << endl;
					//}


					//if (currAnim.t <= currAnim.peakT) {
					//	_footDrawable[i].rot.x = (1 - (currAnim.t / currAnim.peakT)) * currAnim.peakY * -alpha;
					//}
					//else if (currAnim.t <= currAnim.peakEndT) {
					//	_footDrawable[i].rot.x = (currAnim.t - currAnim.peakT) / (currAnim.peakEndT - currAnim.peakT) * currAnim.peakY * theta;
					//}
					//else {
					//}
				//}
				//else {
				//	_footDrawable[i].rot.x = (1 - currAnim.t) * 45 + currAnim.t * (0);
				//}
				
				//if (currAnim.t < 0.5) { 
				//	_footDrawable[i].rot.x = currAnim.t * 45;
				//}
				//else {
				//	_footDrawable[i].rot.x = (1 - currAnim.t) * 45;
				//}
				if (currAnim.t >= 1) {
					currAnim.t = 2; // mark as finished
					_prevFootWalkAnim[i] = currAnim; // copy to prev
				}
				else {
					currAnim.t += dt / currAnim.swingDuration;

					if (currAnim.t > 1) {
						currAnim.t = 1;
					}
				}				
			}
			else if (currAnim.t1 <= 1) {				
				//float landingDuration = clamp(currAnim.peak_t, 0.1, 1);
				//if (i == 1) cout << "right t1 = " << currAnim.t1 << " , " << landingDuration << endl;
				if (currAnim.peak_t > 0) {
					int idx = (int)(currAnim.t1 * currAnim.peak_idx) + 1;
					int size = _prevFoot[i]->drag.size();									

					if (idx >= 0 && idx < size) {
						float dy = (_prevFoot[i]->drag[idx].y - _prevFoot[i]->drag.front().y);
						//cout << dy << " / " << currAnim.peak_y << endl;
						_footDrawable[i].rot.x = (currAnim.peak_y - dy) * -alphaRatio;
						_footDrawable[i].radius = _prevFoot[i]->drag[idx].radius;
						//cout << _footDrawable[i].rot.x << endl;
					}					
				}
				else {
					_footDrawable[i].rot.x = (1 - currAnim.t1) * -10;
				}

				if (currAnim.t1 >= 1) {
					//if (i == 1) cout << "right t1 = finished" << endl;
					currAnim.t1 = 2; // mark as finished
					currAnim.animating = false;

					_footDrawable[i].rot.x = 0; // force to zero at the end of animation
				}
				else {
					currAnim.t1 += dt / currAnim.landingDuration;
					if (currAnim.t1 > 1) {
						currAnim.t1 = 1;						
					}
				}
			}
						
			if (!currAnim.animating) {
				if (i == 0) cout << "left finished" << endl;
				if (i == 1) cout << "right finished" << endl;
				// done animating
				_footDrawable[i].rot.x = 0;
				_footDrawable[i].lastPos = _footDrawable[i].pos;
			}
		}
	}	
}

void App::render() {
	//_glWindow->draw();
}

void App::draw() {
	//float leftY = 0;
	//float rightY = 0;

	//if (_currStepL.phase == 4) {
	//	leftY = 1;
	//	_currStepL.radius = 0;
	//}
	//if (_currStepR.phase == 4) {
	//	rightY = 1;
	//	_currStepR.radius = 0;
	//}
	
	//if (!_leftFootPrints.empty()) {
	//	FootStepModel &leftModel = _leftFootPrints.back();
	//	leftHeelAngle = leftModel.getHeelAngle();
	//	leftTiltAngle = leftModel.getTiltAngle();
	//	cout << leftTiltAngle <<endl;
	//}
	

	static const Color leftFootColor(1.0f, 0.2f, 0.2f, 1.0f);
	static const Color rightFootColor(0.2f, 1.0f, 0.2f, 1.0f);

	this->drawFoot(_footDrawable[FOOT_LEFT].pos , _footDrawable[FOOT_LEFT].rot, leftFootColor, _footDrawable[FOOT_LEFT].radius * 0.02);
	this->drawFoot(_footDrawable[FOOT_RIGHT].pos, _footDrawable[FOOT_RIGHT].rot, rightFootColor, _footDrawable[FOOT_RIGHT].radius * 0.02);
		
	//GLUtils::drawWireFrameBox(Point3d(0 + _currStepL.x, 0 + leftY, 0 + _currStepL.y * 4), Point3d(0.25 + _currStepL.x, 0.1 + leftY, 0.5 + _currStepL.y * 4));
	//GLUtils::drawWireFrameBox(Point3d(1 + _currStepR.x, 0 + rightY, 0 + _currStepR.y * 4), Point3d(1.25 + _currStepR.x, 0.1 + rightY, 0.5 + _currStepR.y * 4));

	//glColor4f(0.9f, 0.4f, 0.0f, 0.5f);
	//GLUtils::drawDisk(_currStepL.radius / 40.0f, Point3d(0.125 + _currStepL.x, 0.001, 0.25 + _currStepL.y * 4), Point3d(0, 1, 0));
	//GLUtils::drawDisk(_currStepR.radius / 40.0f, Point3d(1.125 + _currStepR.x, 0.001, 0.25 + _currStepR.y * 4), Point3d(0, 1, 0));

	
}

void App::drawFoot(Point3d center, Vector3d rot, Color color, float radius) {
	static const float footWidth = 0.25f * 20;	
	static const float footLength = 0.5f * 20;
	static const float footThickness = 0.1f * 20;

	Point3d boxMin(-0.5f * footWidth + center.x, center.y                , -0.5f * footLength + center.z);
	Point3d boxMax( 0.5f * footWidth + center.x, center.y + footThickness,  0.5f * footLength + center.z);
	
	glColor3f(color.r, color.g, color.b);

	glPushMatrix();
	{
		glTranslatef(center.x, center.y, center.z);
		glRotatef(-rot.x, 1, 0, 0);
		glRotatef(rot.y, 0, 1, 0);
		glRotatef(rot.z, 0, 0, 1);
		glTranslatef(-center.x, -center.y, -center.z);
		GLUtils::drawWireFrameBox(boxMin, boxMax);		
	}	
	glPopMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
	GLUtils::drawDisk(radius, Vector3d(center.x, 0.001, center.z), Point3d(0, 1, 0));
}
