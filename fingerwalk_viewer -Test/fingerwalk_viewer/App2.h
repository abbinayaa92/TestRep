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
#include "ConcurrentQueue.h"
#include <iostream>
#include <fstream>
#include "Foot.h"

using namespace osc;

class Animation;

#define theApp App2::instance()

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  960
#define WINDOW_TITLE  "Fingerwalk"
#define PORT_UDP_IN   1234
#define MAX_STEPS_HISTORY 5

#define LOG_DATA 0

//enum {
//	FOOT_LEFT = 0,
//	FOOT_RIGHT = 1
//};
//
//struct FootStep {
//	int stepId;
//	int phase;
//	int type;	
//	float x;
//	float y;
//	float radius;
//	double timestamp;
//
//	FootStep() : stepId(-1), phase(-1), type(0), x(0), y(0), radius(0.1f), timestamp(0) {};
//};
//
//class FootContact {
//public:
//	float x, y, radius;
//	double timestamp;
//#if LOG_DATA
//	float angle;
//#endif
//	FootContact() : x(0), y(0), radius(0), timestamp(0) {};
//	FootContact(float x_, float y_, float r, double t = 0) : x(x_), y(y_), radius(r), timestamp(t) {}
//};
//
//class FootStepModel {
//public:
//	int stepId;
//	bool down;
//	double startTime;
//	double endTime;
//	double timeFromPreviousStep;
//	std::vector<FootContact> drag;
//public:
//	FootStepModel() : stepId(-1), down(true), startTime(0), endTime(0), timeFromPreviousStep(0) {};
//	
//	inline float getHeelAngle() {
//		float ret = 0;
//		int size = drag.size();
//
//		if (size > 2) {
//			ret = (drag.back().y - drag.front().y) * 1000; // displacement from the first contact point
//		}		
//
//		return ret;
//	};
//	inline float getTiltAngle() {
//		float ret = 0;
//		int size = drag.size();
//
//		if (size > 2) {
//			ret = (drag.back().x - drag.front().x) * 1000; // displacement from the first contact point
//		}		
//
//		return ret;
//	};
//};
//
//struct FootAnim {
//	bool animating;
//	float swingDuration;
//	float contactDuration;
//	float t; // keyframe time position
//	float t0; // take off animation
//	float t1; // landing animation
//	Point3d newPos;
//	Point3d oldPos;
//
//	// heel data
//	float peak_y; // y at max
//	float peak_t; // time when y at max
//	float peak_idx; // index when y at max
//	float peakend_t; // time when peak ends
//	float peakend_idx; // index when peak ends
//
//	float takeoffAngle;
//	float landingAngle;
//
//	FootAnim() : 
//		animating(false), swingDuration(0), contactDuration(0), t(0), t0(0), t1(0), peak_y(0), peak_t(0), peak_idx(0), peakend_t(0), peakend_idx(0), takeoffAngle(45), landingAngle(0) {};
//	
//};
//
//struct Color {
//	float r, g, b, a;
//
//	Color() : r(0), g(0), b(0), a(0) {}
//	Color(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_) {}
//};
//
//struct FootDrawable {
//	Point3d pos;
//	Vector3d rot;
//	Color color;
//	Point3d toPos;
//	Point3d lastPos;
//	float radius;
//	float last_dy;
//
//	FootDrawable() : radius(0), last_dy(0) {}
//};
//
//struct FootRecord {
//	Point3d pos;
//	float dy;
//	float angle;
//};

class App2 : public OscPacketListener, Drawable {
protected:
	App2() {};
	static App2 *_instance;
	bool readDataFromInputStream(std::istream& in);
	void drawFoot(Point3d center, Vector3d rot, Color color, float radius);
public:
	inline static App2& instance() {
		if (!_instance) _instance = new App2();
		return *_instance;
	}
	~App2();
	App2 *getThis() { return this; };
	
	// osc callback
	virtual void ProcessMessage(const ReceivedMessage& m, const IpEndpointName& remoteEndPoint);

	// inherits from Drawable
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
	static void keyboardCallback(unsigned char key, int x, int y) { theApp.onKeyEvent((int)key); };
private:
	GLWindow *_glWindow;
	bool _online; // whether we use online or offline data.

	Timer _timer;	

	bool _firstStep;
	int _firstStepId;

	double _lastTime;
	double _currTime;
	double _elapsedTime;
	double _footStartTime;
	double _t; // used for walk animation interpolation

	//bool _animatingLeftFoot;
	//bool _animatingRightFoot;	
	
	std::vector<FootDrawable> _footDrawable;
	
	// TODO: use pointer to FootStepModel but must define copy operator to clone objects.
	ConcurrentQueue<FootStepModel*> _leftFootQ;
	ConcurrentQueue<FootStepModel*> _rightFootQ;
	ConcurrentQueue<FootStepModel> _leftFootPrints;
	ConcurrentQueue<FootStepModel> _rightFootPrints;

	std::vector<FootStepModel*> _foot;
	std::vector<FootStepModel*> _prevFoot;

	typedef std::pair<FootStepModel, FootStepModel> StepPair;
	//std::vector<StepPair> _footWalk;
	std::vector<std::list<FootAnim> > _footWalkAnimQueue;
	std::vector<FootAnim> _footWalkAnim;
	std::vector<FootAnim> _prevFootWalkAnim;

	std::vector<ConcurrentQueue<FootStepModel*>* > _footQ; // immediate process queue	
	std::vector<ConcurrentQueue<FootStepModel>* > _footPrints; // history
	//vector<pair<int, int>> _pairs;

	bool _usingHistory;
	bool _userIsTouching;
	int _lastFootTypeOnGround;
	float _lastSwingDuration;
	double _timeSinceLastContact;
	bool _startWalking;

	// animation helper
	float _prev_dy;
	float _prev_dx;
	
	//typedef std::vector<FootRecord> FootHistory;

	
	typedef std::list<FootStepModel> FootHistory;
	std::vector<FootHistory> _footHistory;

	std::vector<FootHistory::iterator> _footHistoryItr;
	std::vector<FootHistory::iterator> _footHistoryItrStart;

	double _historyStartTime;
	double _historyElapsedTime;
	double _historyOffsetTime;

	Animation *_animation;

#if LOG_DATA
	std::ofstream _outfile;
	std::vector<FootContact> _contacts;
#endif

	Point3d _origin; // absolute space origin
	Point3d _originAnim;
};
