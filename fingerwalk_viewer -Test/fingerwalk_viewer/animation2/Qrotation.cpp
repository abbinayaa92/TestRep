#include <math.h>
#include "Qrotation.h"
#include "Quaternion3.h"

#ifndef PI
#define PI 3.1415926535897932384626433832795028841972
#endif

Qrotation::Qrotation()
{
	qRot = Quaternion3(1.0, 0.0, 0.0, 0.0);
	angle = 0;
	Xaxis = 0;
	Yaxis = 0;
	Zaxis = 0;
}

Qrotation::Qrotation(double angle, double Xaxis, double Yaxis, double Zaxis)
{
	this->angle = angle;
	double cosAngle = cos(angle*PI/360.0);
	double sinAngle = sin(angle*PI/360.0);
	double scalar = sqrt(Xaxis*Xaxis + Yaxis*Yaxis + Zaxis*Zaxis);
	this->Xaxis = Xaxis/scalar;
	this->Yaxis = Yaxis/scalar;
	this->Zaxis = Zaxis/scalar;
	double qRotw = cosAngle;
	double qRotx = this->Xaxis*sinAngle;
	double qRoty = this->Yaxis*sinAngle;
	double qRotz = this->Zaxis*sinAngle;
	if (scalar>0)
	{
		qRot = Quaternion3(qRotw, qRotx, qRoty, qRotz);
	}
	else
	{
		qRot = Quaternion3(1.0, 0, 0, 0);
	}
}

Quaternion3 Qrotation::getqRot()
{
	return qRot;
}

void Qrotation::rotate(double angle, double Xaxis, double Yaxis, double Zaxis)
{
	qRot = Qrotation(angle, Xaxis, Yaxis, Zaxis).getqRot()*qRot;
	qRot.normalize();
	this->angle = acos(qRot.getw())*360/PI;
	this->Xaxis = qRot.getx()/sin(this->angle*PI/360.0);
	this->Yaxis = qRot.gety()/sin(this->angle*PI/360.0);
	this->Zaxis = qRot.getz()/sin(this->angle*PI/360.0);
}

double Qrotation::getAngle()
{
	return angle;
}

double Qrotation::getXaxis()
{
	return Xaxis;
}

double Qrotation::getYaxis()
{
	return Yaxis;
}

double Qrotation::getZaxis()
{
	return Zaxis;
}

void Qrotation::rotatePoint(double x, double y, double z, double rotatedPoint[])
{
	double cosA = cos(angle*PI/180);
	double sinA = sin(angle*PI/180);
	rotatedPoint[0] = (cosA+Xaxis*Xaxis*(1-cosA))*x + (Xaxis*Yaxis*(1-cosA)-Zaxis*sinA)*y + (Xaxis*Zaxis*(1-cosA)+Yaxis*sinA)*z;
	rotatedPoint[1] = (Yaxis*Xaxis*(1-cosA)+Zaxis*sinA)*x + (cosA+Yaxis*Yaxis*(1-cosA))*y + (Yaxis*Zaxis*(1-cosA)-Xaxis*sinA)*z;
	rotatedPoint[2] = (Zaxis*Xaxis*(1-cosA)-Yaxis*sinA)*x + (Zaxis*Yaxis*(1-cosA)+Xaxis*sinA)*y + (cosA+Zaxis*Zaxis*(1-cosA))*z;
}

Qrotation Qrotation::pow(double t)
{
	return Qrotation(t*angle, Xaxis, Yaxis, Zaxis);
}

Qrotation Qrotation::slerp(Qrotation qrot2, double t)
{
	Quaternion3 qt = Qrotation(-angle, Xaxis, Yaxis, Zaxis).getqRot();
	//if (Xaxis*qrot2.getXaxis() + Yaxis*qrot2.getYaxis() + Zaxis*qrot2.getZaxis()<0) qrot2 = Qrotation(360-qrot2.getAngle(), qrot2.getXaxis(), qrot2.getYaxis(), qrot2.getZaxis());
	Quaternion3 qt2 = qrot2.getqRot();
	//if (qt.getw()*qt2.getw()<0) Quaternion3(-qt2.getw(), -qt2.getx(), -qt2.gety(), -qt2.getz());
	//if (qt.getw()*qt2.getw()<0 && (Xaxis*qt2.getx()+Yaxis*qt2.gety()+Zaxis*qt2.getz())*sin(acos(qt2.getw()))>=0) qt2 = Quaternion3(-qt2.getw(), -qt2.getx(), -qt2.gety(), -qt2.getz());
	//else if (qt.getw()*qt2.getw()>0 && (Xaxis*qt2.getx()+Yaxis*qt2.gety()+Zaxis*qt2.getz())*sin(acos(qt2.getw()))<0) qt2 = Quaternion3(-qt2.getw(), -qt2.getx(), -qt2.gety(), -qt2.getz());
	//if (qt.getw()*qt2.getw()<0 && (Xaxis*qt2.getx()+Yaxis*qt2.gety()+Zaxis*qt2.getz())*sin(acos(qt2.getw()))<0) qt2 = Quaternion3(-qt2.getw(), -qt2.getx(), -qt2.gety(), -qt2.getz());
	double cosHalfTheta = qt.getw() * qt2.getw() + qt.getx() * qt2.getx() + qt.gety() * qt2.gety() + qt.getz() * qt2.getz();
	//if (cosHalfTheta<0) qt2 = Quaternion3(-qt2.getw(), -qt2.getx(), -qt2.gety(), -qt2.getz());

	/*Qrotation qrot;
	if (angle>180) qrot = Qrotation(angle-360, Xaxis, Yaxis, Zaxis);
	else if (angle<-180) qrot = Qrotation(-angle-360, Xaxis, Yaxis, Zaxis);
	else qrot = Qrotation(-angle, -Xaxis, -Yaxis, -Zaxis);
	if (qrot2.getAngle()>180) qrot2 = Qrotation(360-qrot2.getAngle(), -qrot2.getXaxis(), -qrot2.getYaxis(), -qrot2.getZaxis());
	else if (qrot2.getAngle()<-180) qrot2 = Qrotation(360+qrot2.getAngle(), -qrot2.getXaxis(), -qrot2.getYaxis(), -qrot2.getZaxis());
	qt = qrot.getqRot();
	qt2 = qrot2.getqRot();*/
	qt = qt2*qt;
	qt.normalize();
	double ang = acos(qt.getw());
	double x, y, z;
	if (abs(ang)>1e-16)
	{
		x = qt.getx()/sin(ang);
		y = qt.gety()/sin(ang);
		z = qt.getz()/sin(ang);
	}
	else
	{
		x = 1;
		y = 0;
		z = 0;
	}
	qt = Quaternion3(cos(ang*t), x*sin(ang*t), y*sin(ang*t), z*sin(ang*t))*Qrotation(angle, Xaxis, Yaxis, Zaxis).getqRot();
	qt.normalize();
	ang = acos(qt.getw())*360/PI;
	x = qt.getx()/sin(ang*PI/360.0);
	y = qt.gety()/sin(ang*PI/360.0);
	z = qt.getz()/sin(ang*PI/360.0);
	return Qrotation(ang, x, y, z);
}