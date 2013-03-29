#include "App3.h"
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
#define USE_ANIMATION 0

// scaling factor from touch space to animation space
static const float UNITSCALE_Y = 8.0f * 20;
static const float UNITSCALE_X = 6.0f * 20;

App3 *App3::_instance = NULL;

App3::~App3() {
}

bool App3::init(bool online) {
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	// set up callback methods
	glutDisplayFunc(App3::displayCallback);
	glutIdleFunc(App3::idleCallback);
	glutReshapeFunc(App3::reshapeCallback);
	glutMouseFunc(App3::mouseCallback);
	glutMotionFunc(App3::motionCallback);
	//glutPassiveMotionFunc(onMouseMovedPassive);
	glutKeyboardFunc(App3::keyboardCallback);

	_glWindow = new GLWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	_online = online;

	if (!_online) {
	}

	_glWindow->addDrawable(this);
	_timer.restart();
	_lastTime = _currTime = _timer.timeEllapsed();
	_elapsedTime = -1;

	_footPrints.push_back(&_leftFootPrints);
	_footPrints.push_back(&_rightFootPrints);
	_footDrawable.resize(2);
	_currFoot.resize(2);
	_prevFoot.resize(2);

	_currFoot[0] = _currFoot[1] = NULL;
	_prevFoot[0] = _prevFoot[1] = NULL;

	_firstStep = true;
	_startWalking = false;

	_sa.addGaitChangedListener(this);
	_fa = &_animWalk;
	_fa->setLeftFoot(&_footDrawable[0]);
	_fa->setRightFoot(&_footDrawable[1]);

	_fa_next = NULL;

	_animHop.setLeftFoot(&_footDrawable[0]);
	_animHop.setRightFoot(&_footDrawable[1]);

	_footDrawable[FOOT_LEFT].pos = _footDrawable[FOOT_LEFT].lastPos = _footDrawable[FOOT_LEFT].toPos =  Point3d(-0.5 * 20, 0, -UNITSCALE_Y);
	_footDrawable[FOOT_RIGHT].pos = _footDrawable[FOOT_RIGHT].lastPos = _footDrawable[FOOT_RIGHT].toPos = Point3d(0.5 * 20, 0, -UNITSCALE_Y);
	return true;
}

void App3::quit() {
#if LOG_DATA
#endif
}

void App3::gaitChanged(const LocomotionModel& m) {
	GaitType g = m.gaitType; 
	switch (g) {
		case kGaitWalking:
			cout << "[App3] I'm walking.. with alpha = " << m.alpha << endl;
			_fa_next = &_animWalk;
			break;
		case kGaitHop:
			cout << "[App3] I'm hopping.." << endl;
			_fa_next = &_animHop;
			break;
		case kGaitRunning:
			cout << "[App3] I'm running.. with alpha = " << m.alpha << endl;
//			_fa = &_animRun;
			_fa_next = &_animWalk;
			break;
		case kGaitTiptoeing:
			cout << "[App3] I'm tiptoeing.." << endl;
			break;
	}
}

void App3::ProcessMessage(const ReceivedMessage& m, const IpEndpointName& remoteEndPoint) {
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
			
			
			if (type == 0 || type == 1) {
				if (phase == 1) { // TOUCH_BEGAN
					FootStepModel m;
					m.stepId = stepId;
					m.down = true;
					m.drag.push_back(c);

					if (type == FOOT_LEFT && _firstStep) {
						_footStartTime = c.timestamp;
						_firstStep = false;
						_firstStepId = stepId;
					}

					m.startTime = c.timestamp - _footStartTime;

					if (!_footPrints[type]->empty()) {
						// invalidate previous step
						FootStepModel &last = _footPrints[type]->back();
						last.down = false;
					}

					//if (_footPrints[type]->size() > MAX_STEPS_HISTORY) {
					//	_footPrints[type]->pop_front();
					//}

					_footPrints[type]->push_back(m);
				}
				else if (phase == 2) { // TOUCH_MOVED
					if (!_footPrints[type]->empty()) {
						FootStepModel &m = _footPrints[type]->back();
						m.drag.push_back(c);
					}
				}
				else if (phase == 4) { // TOUCH_ENDED					
					if (!_footPrints[type]->empty()) {
						FootStepModel &m = _footPrints[type]->back();
						m.drag.push_back(c);
						m.endTime = c.timestamp - _footStartTime;
						m.down = false;
					}
				}
			}			
		}
	}
	catch (osc::Exception& e) {
		// any parsing errors such as unexpected argument types, or
		// missing arguments get thrown as exceptions.
		cout << "error while parsing message: "
			<< m.AddressPattern() << ": " << e.what() << endl;
	}
}

void App3::onDisplay() {
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

void App3::onIdle() {
	//ensureMainWindowSelected();
	glutPostRedisplay();
}

void App3::onReshape(int w, int h) {
	glViewport(0, 0, w, h);

	_glWindow->onResize(w, h);
}

void App3::onMouse(int button, int state, int x, int y) {
	int btn = MOUSE_LBUTTON;
	int evt = MOUSE_UP;

	if (button == GLUT_RIGHT_BUTTON) btn = MOUSE_RBUTTON;
	else if (button == GLUT_MIDDLE_BUTTON) btn = MOUSE_MBUTTON;
	else if (button == GLUT_WHEEL_DOWN) btn = MOUSE_WHEEL_DOWN;
	else if (button == GLUT_WHEEL_UP) btn = MOUSE_WHEEL_UP;
	
	if (state == GLUT_DOWN) evt = MOUSE_DOWN;

	_glWindow->onMouseEvent(evt, btn, x, y);
}

void App3::onMouseMoved(int x, int y) {
	_glWindow->onMouseEvent(MOUSE_DRAG, -1, x, y);
}

void App3::onKeyEvent(int key) {
}

void App3::updateCamera(double dt) {
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

void App3::update(double dt) {
	if (_startWalking) {
		_elapsedTime += dt;		
	}

	this->updateCamera(dt);

	this->updateFoot(dt, FOOT_LEFT);
	this->updateFoot(dt, FOOT_RIGHT);

	if (_fa_next) {
		if (_fa->stop()) {
			_fa = _fa_next;
			_fa->reset();
			_fa_next = NULL;
		}
	}
	
	_fa->update(dt);	
	

	//float swingDuration = 0.7;
	//float stepLength = 20;

	//_fa->setFootSwingDuration(swingDuration);
	//_fa->setFootSwingLength(stepLength);
}

void App3::updateFoot(double dt, int type) {
	if (_currFoot[type] != NULL) {
		if (_currFoot[type]->down) {

			static const double footStayThreshold = 1.2;
			if (_footStayTime < footStayThreshold) {
				_footStayTime += dt;
				if (_footStayTime > footStayThreshold) {					
					_fa->setFootSwingLength(0);
					_stopWalking = true;
				}
			}		
		}
		else {
			//_prevFoot[type] = _currFoot[type];
			_prevFoot[type] = _sa.addFootStep(type, *_currFoot[type]);
			_currFoot[type] = NULL;
			
			_footPrints[type]->pop_front();
			_footStayTime = 0;
			//cout << "pop = " << _prevFoot[type]->stepId << endl;
		}
	}
	else {
		if (!_footPrints[type]->empty()) {
			FootStepModel &foot = _footPrints[type]->front();

			if (!foot.down) {
				cout << "Foot was up before picked from the queue." << endl;
				//return;
			}

			if (!_startWalking && foot.stepId == _firstStepId) {
				_startWalking = true;
				_elapsedTime = 0;
			}

			_currFoot[type] = &foot;
			
			if (_prevFoot[type] != NULL) {
				if (_stopWalking) {
					_fa->reset();
					_stopWalking = false;
				}

				const FootContact &c1 = _prevFoot[type]->drag.back();
				const FootContact &c2 = _currFoot[type]->drag.front();

				float delta = fabs(c2.y - c1.y);

				
				if (delta > 0.5) { // dont bother, the finger moves back from up to bottom
					
				}
				else {
					delta = clamp(delta, 0, 0.2);
					float swingDuration = _currFoot[type]->startTime - _prevFoot[type]->endTime;
					float stepLength = delta * UNITSCALE_Y;

					//cout << "duration = " << swingDuration << ", length = " << stepLength << endl;
					_fa->setFootSwingDuration(swingDuration);
					_fa->setFootSwingLength(stepLength);

					const LocomotionModel& lm = _sa.getLocomotionModel();

					if (lm.gaitType == kGaitWalking) {
						//cout << "walking with alpha = " << lm.alpha << " contact = " << lm.contactDuration << endl;
						if (lm.alpha >= 0) {
							((Walk*)_fa)->setContactDuration(lm.contactDuration);
							((Walk*)_fa)->setOverlapAlpha(lm.alpha);
						}												
					}
					else if (lm.gaitType == kGaitRunning) {
						cout << "running with alpha = " << lm.alpha << endl;
						if (lm.alpha >= -0.55) {
							((Walk*)_fa)->setContactDuration(lm.contactDuration);
							((Walk*)_fa)->setOverlapAlpha(lm.alpha);
						}
					}
					else if (lm.gaitType == kGaitHop) {
						((Hop*)_fa)->setContactDuration(lm.contactDuration);
						((Hop*)_fa)->setAlpha(lm.alpha);
					}
				}
			}												
		}
	}
}

void App3::updateAnimation(double dt) {
}

void App3::updateFootAnimation(double dt, int type) {
}

void App3::updateFootUsingHistory(double dt, int type) {
}

void App3::render() {
	//_glWindow->draw();
}

void App3::draw() {
	static const Color leftFootColor(1.0f, 0.2f, 0.2f, 1.0f);
	static const Color rightFootColor(0.2f, 1.0f, 0.2f, 1.0f);

	this->drawFoot(_footDrawable[FOOT_LEFT].pos , _footDrawable[FOOT_LEFT].rot, leftFootColor, _footDrawable[FOOT_LEFT].radius * 0.02);
	this->drawFoot(_footDrawable[FOOT_RIGHT].pos, _footDrawable[FOOT_RIGHT].rot, rightFootColor, _footDrawable[FOOT_RIGHT].radius * 0.02);	
}

void App3::drawFoot(Point3d center, Vector3d rot, Color color, float radius) {
	static const float footWidth = 0.25f * 20;	
	static const float footLength = 0.5f * 20;
	static const float footThickness = 0.1f * 20;

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
