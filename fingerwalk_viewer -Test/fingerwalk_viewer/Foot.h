#pragma once
#include <vector>
#include <MathLib/Point3d.h>
#include <MathLib/Vector3d.h>

enum {
	FOOT_LEFT = 0,
	FOOT_RIGHT = 1
};

struct FootStep {
	int stepId;
	int phase;
	int type;	
	float x;
	float y;
	float radius;
	double timestamp;

	FootStep() : stepId(-1), phase(-1), type(0), x(0), y(0), radius(0.1f), timestamp(0) {};
};

class FootContact {
public:
	float x, y, radius;
	double timestamp;
#if LOG_DATA
	float angle;
#endif
	FootContact() : x(0), y(0), radius(0), timestamp(0) {};
	FootContact(float x_, float y_, float r, double t = 0) : x(x_), y(y_), radius(r), timestamp(t) {}
};

class FootStepModel {
public:
	int stepId;
	bool down;
	double startTime;
	double endTime;
	double timeFromPreviousStep;
	std::vector<FootContact> drag;
public:
	FootStepModel() : stepId(-1), down(true), startTime(0), endTime(0), timeFromPreviousStep(0) {};
	
	inline float getHeelAngle() {
		float ret = 0;
		int size = drag.size();

		if (size > 2) {
			ret = (drag.back().y - drag.front().y) * 1000; // displacement from the first contact point
		}		

		return ret;
	};
	inline float getTiltAngle() {
		float ret = 0;
		int size = drag.size();

		if (size > 2) {
			ret = (drag.back().x - drag.front().x) * 1000; // displacement from the first contact point
		}		

		return ret;
	};
};

class FootStepNew {
public:
	float steplength;
	double stepduration;
	bool down;
	float startTime;
	float endTime;
	float x;
	float y;
	float endx;
	float endy;
	int mode;
	int type;
	float radius;
	float radius_end;
	float angle;
	float currstart;
	float currend;

public:
	FootStepNew() : steplength(0),stepduration(2.5), down(true), startTime(0), endTime(0), x(0), y(0), type(0), radius(0), angle(0) {};
};

class AutoPoint {
public:
	int x1;
	int y1;
	int x2;
	int y2;

public:
	AutoPoint() :x1(11),y1(0),x2(11),y2(0) {};
};

struct FootAnim {
	bool animating;
	float swingDuration;
	float swingLength;
	float contactDuration;
	float landingDuration;
	float t; // keyframe time position
	float t0; // take off animation
	float t1; // landing animation
	float ct; // contact keyframe time position
	Point3d newPos;
	Point3d oldPos;

	// heel data
	float peak_y; // y at max
	float peak_t; // time when y at max
	float peak_idx; // index when y at max
	float peakend_t; // time when peak ends
	float peakend_idx; // index when peak ends

	float takeoffAngle;
	float landingAngle;

	bool doneAnimating;
	FootAnim() : 
		animating(false), swingDuration(0), swingLength(0), contactDuration(0), landingDuration(0), t(0), t0(0), t1(0), ct(0), peak_y(0), peak_t(0), peak_idx(0), peakend_t(0), peakend_idx(0), takeoffAngle(45), landingAngle(0), doneAnimating(true) {};
	
};

struct Color {
	float r, g, b, a;

	Color() : r(0), g(0), b(0), a(0) {}
	Color(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_) {}
};

struct FootDrawable {
	Point3d pos;
	Vector3d rot;
	Color color;
	Point3d toPos;
	Point3d lastPos;
	float radius;
	float last_dy;

	FootDrawable() : radius(0), last_dy(0) {}
};

struct FootRecord {
	Point3d pos;
	float dy;
	float angle;
};