#ifndef __QROTATION_H__
#define __QROTATION_H__

#include "Quaternion3.h"

class Qrotation
{
	Quaternion3 qRot;
	double angle;
	double Xaxis;
	double Yaxis;
	double Zaxis;
	
	public:
		Qrotation();
		Qrotation(double angle, double Xaxis, double Yaxis, double Zaxis);
		Quaternion3 getqRot();
		void rotate(double angle, double Xaxis, double Yaxis, double Zaxis);
		double getAngle();
		double getXaxis();
		double getYaxis();
		double getZaxis();
		void rotatePoint(double x, double y, double z, double rotatedPoint[]);
		Qrotation pow(double t);
		Qrotation slerp(Qrotation qrot2, double t);
};

#endif