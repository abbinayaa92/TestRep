#include <math.h>
#include "Quaternion3.h"

Quaternion3::Quaternion3()
{
	w = 0;
	x = 0;
	y = 0;
	z = 0;
}

Quaternion3::Quaternion3(double a, double b, double c, double d)
{
	w = a;
	x = b;
	y = c;
	z = d;
}

double Quaternion3::getw()
{
	return w;
}

double Quaternion3::getx()
{
	return x;
}

double Quaternion3::gety()
{
	return y;
}

double Quaternion3::getz()
{
	return z;
}

double Quaternion3::absSquare()
{
	return w*w + x*x + y*y + z*z;
}

double Quaternion3::abs()
{
	return sqrt(this->absSquare());
}

void Quaternion3::normalize()
{
	double scalar = this->abs();
	w /= scalar;
	x /= scalar;
	y /= scalar;
	z /= scalar;
}

Quaternion3 Quaternion3::conj()
{
	return Quaternion3(w, -x, -y, -z);
}

Quaternion3 Quaternion3::inv()
{
	return (this->conj())/(this->absSquare());
}

Quaternion3 Quaternion3::operator*(double scalar)
{
	double wtemp = w*scalar;
	double xtemp = x*scalar;
	double ytemp = y*scalar;
	double ztemp = z*scalar;
	return Quaternion3(wtemp, xtemp, ytemp, ztemp);
}

Quaternion3 Quaternion3::operator/(double scalar)
{
	double wtemp = w/scalar;
	double xtemp = x/scalar;
	double ytemp = y/scalar;
	double ztemp = z/scalar;
	return Quaternion3(wtemp, xtemp, ytemp, ztemp);
}

void Quaternion3::operator*=(double scalar)
{
	(*this) = (*this) * scalar;
}

void Quaternion3::operator/=(double scalar)
{
	(*this) = (*this) / scalar;
}

Quaternion3 Quaternion3::operator+(const Quaternion3& Q)
{
	Quaternion3 Qtemp = (Quaternion3)Q;
	double wtemp = w + Qtemp.getw();
	double xtemp = x + Qtemp.getx();
	double ytemp = y + Qtemp.gety();
	double ztemp = z + Qtemp.getz();
	return Quaternion3(wtemp, xtemp, ytemp, ztemp);
}

Quaternion3 Quaternion3::operator-(const Quaternion3& Q)
{
	Quaternion3 Qtemp = (Quaternion3)Q;
	double wtemp = w - Qtemp.getw();
	double xtemp = x - Qtemp.getx();
	double ytemp = y - Qtemp.gety();
	double ztemp = z - Qtemp.getz();
	return Quaternion3(wtemp, xtemp, ytemp, ztemp);
}

Quaternion3 Quaternion3::operator*(const Quaternion3& Q)
{
	Quaternion3 Qtemp = (Quaternion3)Q;
	double wtemp = w*Qtemp.getw() - x*Qtemp.getx() - y*Qtemp.gety() - z*Qtemp.getz();
	double xtemp = w*Qtemp.getx() + x*Qtemp.getw() + y*Qtemp.getz() - z*Qtemp.gety();
	double ytemp = w*Qtemp.gety() - x*Qtemp.getz() + y*Qtemp.getw() + z*Qtemp.getx();
	double ztemp = w*Qtemp.getz() + x*Qtemp.gety() - y*Qtemp.getx() + z*Qtemp.getw();
	return Quaternion3(wtemp, xtemp, ytemp, ztemp);
}

Quaternion3 Quaternion3::operator/(const Quaternion3& Q)
{
	return (*this)*((Quaternion3)Q).inv();
}

void Quaternion3::operator+=(const Quaternion3& Q)
{
	(*this) = (*this) + Q;
}

void Quaternion3::operator-=(const Quaternion3& Q)
{
	(*this) = (*this) - Q;
}

void Quaternion3::operator*=(const Quaternion3& Q)
{
	(*this) = (*this) * Q;
}

void Quaternion3::operator/=(const Quaternion3& Q)
{
	(*this) = (*this) / Q;
}

