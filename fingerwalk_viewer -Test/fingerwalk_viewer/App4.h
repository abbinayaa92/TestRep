#pragma once

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "GLWindow.h"
#include <vector>
#include <list>
#include <Utils/Timer.h>
#include <MathLib/Point3d.h>
#include <MathLib/Vector3d.h>
#include "StepsAnalyzer.h"
#include "ip/UdpSocket.h"
#include "ConcurrentQueue.h"
#include <iostream>
#include <fstream>
#include "Foot.h"
#include "StepsAnalyzer.h"
//#include "Walk.h"
//#include "Hop.h"
#include "animation2/PersonState.h"
#include "animation2/Maze.h"

class Animation;

using namespace osc;
using namespace std;

#define theApp App4::instance()

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  960
#define WINDOW_TITLE  "Fingerwalk"
#define PORT_UDP_IN   1234
#define MAX_STEPS_HISTORY 10

#define LOG_DATA 0

class App4 : public OscPacketListener, Drawable, GaitChangedListener {
protected:
	App4()//:EndPoint(IpEndpointName(ADDRESS, PORT_UDP_IN+1))
	{
		
	}
	static App4 *_instance;
	void drawFoot(Point3d center, Vector3d rot, Color color, float radius);

public:
	inline static App4& instance() {
		if (!_instance) _instance = new App4();
		return *_instance;
	}
	~App4();
	App4 *getThis() { return this; };

	// osc callback
	virtual void ProcessMessage(const ReceivedMessage& m, const IpEndpointName& remoteEndPoint);
	

	virtual void draw();
	bool init(bool online);
	void render(); // this is for render loop by OpenGL
	void update(double dt);
	void updateAnimation(double dt);
	void updateCamera(double dt);
	void updateFoot(double dt, int type);
	void updateFootUsingHistory(double dt, int type);
	void updateFootAnimation(double dt, int type);

	void quit();
	void onDisplay();
	void onIdle();
	void onReshape(int w, int h);
	void onMouse(int button, int state, int x, int y);
	void onMouseMoved(int x, int y);
	void onKeyEvent(int key);

	static void displayCallback() { theApp.onDisplay(); };
	static void idleCallback() { theApp.onIdle(); };
	static void reshapeCallback(int w, int h) { theApp.onReshape(w, h); };
	static void mouseCallback(int button, int state, int x, int y) { theApp.onMouse(button, state, x, y); };
	static void motionCallback(int x, int y) { theApp.onMouseMoved(x, y); };
	static void keyboardCallback(unsigned char key, int x, int y) { theApp.onKeyEvent(key); };

	virtual void gaitChanged(const LocomotionModel& m);
private:
	GLWindow *_glWindow;
	bool _online;

	Timer _timer;
	bool _firstStep;
	bool _userIsTouching;
	bool _startWalking;
	bool _stopWalking;
	bool jumpflag;
	bool escflag;
	bool sendflag;
	bool autoflag;
	bool endflag;
	std::vector<bool> ignoreflag;
	int flag;
	int mode;
	int numberSteps[2];
	int _firstStepId;
	int numid;
	int playmode;
	double _lastTime;
	double _currTime;
	double _elapsedTime;
	double _footStartTime;
	double _footStayTime;

	float last_angle;
	float last_angle_cam;
	std::vector<FootDrawable> _footDrawable;
	StepsAnalyzer _sa;
	//FootAnimator *_fa;
	//FootAnimator *_fa_next;

	//Walk _animWalk;
	//Tiptoe _animTiptoe;
	//Run _animRun;
	//Hop _animHop;

	vector<pair<float,float>> drawpoint;
	float x_pt , y_pt;
	typedef ConcurrentQueue<FootStepNew> FootPrintQueue;
	FootPrintQueue _leftFootPrints;
	FootPrintQueue _rightFootPrints;

	std::vector<FootPrintQueue*> _footPrints;

	typedef ConcurrentQueue<FootStepNew> FootPrintQueue1;
	ConcurrentQueue<AutoPoint> _newpoints;
	ConcurrentQueue<AutoPoint> _allpoints;
	FootPrintQueue1 _leftFootPrints1;
	FootPrintQueue1 _rightFootPrints1;
	std::vector<FootPrintQueue1*> _allPrints;
	vector<FootStepNew*> _currFoot;
	vector<FootStepNew*> _prevFoot;
	void setMaze();
	//std::vector<AutoPointQueue*> _newpoints;
	//std::vector<AutoPointQueue1*> _allpoints;

	Animation *_anim;
	Maze *_maze;
	int *motionMaze[35];
	PersonState *_personstate;
	GaitType _currGaitType, _nextGaitType, _prevGaitType;
	float _currStepLength, _nextStepLength;
	float _currStepDuration, _nextStepDuration;
	int type,dispType;
	int typeBegan, typeEnd;
	bool retbool;
	int walkflag,win,winmode;
	//IpEndpointName& EndPoint;
	IpEndpointName *EndPoint;
	float step;

};