#ifndef __QUATERNION3_H__
#define __QUATERNION3_H__

class Quaternion3
{
	double w, x, y, z;
	public :
		Quaternion3();
		Quaternion3(double a, double b, double c, double d);
		double getw();
		double getx();
		double gety();
		double getz();
		double absSquare();
		double abs();
		void normalize();
		Quaternion3 conj();
		Quaternion3 inv();
		Quaternion3 operator*(double scalar);
		Quaternion3 operator/(double scalar);
		void operator*=(double scalar);
		void operator/=(double scalar);
		Quaternion3 operator+(const Quaternion3& Q);
		Quaternion3 operator-(const Quaternion3& Q);
		Quaternion3 operator*(const Quaternion3& Q);
		Quaternion3 operator/(const Quaternion3& Q);
		void operator+=(const Quaternion3& Q);
		void operator-=(const Quaternion3& Q);
		void operator*=(const Quaternion3& Q);
		void operator/=(const Quaternion3& Q);
};

#endif