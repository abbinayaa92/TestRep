#ifndef QUATERNION_H_
#define QUATERNION_H_

class Quaternion {

public:
    double x, y, z, w;

public:
    Quaternion() : x(0.0), y(0.0), z(0.0), w(1.0) {};
    Quaternion(double, double, double, double);
    Quaternion(const Quaternion& q);

    Quaternion& Set(const VectorR3& point);
    Quaternion& Set(double xx, double yy, double zz, double ww);
    Quaternion& Set(const VectorR4&);
    Quaternion& Set(double angle, const VectorR3& axis);

    VectorR3 ToPoint() const { return VectorR3(x, y, z); }
    Quaternion& Set( const RotationMapR3& );
    Quaternion& SetRotate( const VectorR3& );

    Quaternion& SetIdentity();		// Set to the identity map
    Quaternion  Inverse() const;	// Return the Inverse
    Quaternion& Invert();			// Invert this quaternion

    double Angle();					// Angle of rotation
    double Norm();					// Norm of x,y,z component

    Quaternion& operator*=(const Quaternion&);
    Quaternion& operator=(const Quaternion& q);

    static const Quaternion IdentityQuaternion;
};

Quaternion operator*(Quaternion q1, const Quaternion& q2);

#endif