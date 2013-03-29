#include "LinearR3.h"
#include "LinearR4.h"

#ifndef Quaternion2_H_
#define Quaternion2_H_

    class Quaternion2 {
    public:
        double x, y, z, w;

    public:
        Quaternion2& operator = (const Quaternion2 &rhs){
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
            this->w = rhs.w;
            return *this;
        }
        void GetAxisAngle(double &theta, double &x, double &y, double &z);
        Quaternion2 DecomposeRotationReverse2(const VectorR3 vB) const;
        Quaternion2 DecomposeRotationReverse(const VectorR3 vB) const;
        Quaternion2 getComplexConjugate() const;
        Quaternion2 getInverse() const;
        Quaternion2 DecomposeRotation2(const VectorR3 vB) const;
        Quaternion2 getRotationQuaternion2(double angle, const VectorR3 &axis) const;
        VectorR3 rotate(const VectorR3& u) const;
        Quaternion2& operator *= (double scalar);
        Quaternion2& toUnit();
        double getLength() const;
        Quaternion2() : x(0.0), y(0.0), z(0.0), w(1.0) {};
        Quaternion2 DecomposeRotation(const VectorR3 vB) const;
        Quaternion2 SlerpWith(const Quaternion2 &other, double t) const;
        double DotProductWith(const Quaternion2 &other) const;
        Quaternion2(double, double, double, double);
        Quaternion2(const Quaternion2& q);

        Quaternion2& Set(const VectorR3& point);
        Quaternion2& Set(double xx, double yy, double zz, double ww);
        Quaternion2& Set(const VectorR4&);
        Quaternion2& Set(double angle, const VectorR3& axis);

        VectorR3 ToPoint() const { return VectorR3(x, y, z); }
        Quaternion2& Set( const RotationMapR3& );
        Quaternion2& SetRotate( const VectorR3& );

        Quaternion2 &Negate() { if (w < 0.0) {w = -w; x = -x; y =-y; z = -z; } return *this; };
        Quaternion2& SetIdentity();		// Set to the identity map
        Quaternion2  Inverse() const;	// Return the Inverse
        Quaternion2& Invert();			// Invert this Quaternion2

        double Angle();					// Angle of rotation
        double Norm();					// Norm of x,y,z component

        Quaternion2& operator*=(const Quaternion2&);
        //Quaternion2& operator=(const Quaternion2& q);
        Quaternion2 operator * (double scalar) const{
            return Quaternion2(x * scalar, y * scalar, z * scalar, w * scalar);
        }
        Quaternion2 operator + (const Quaternion2 &other) const{
            return Quaternion2(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        static const Quaternion2 IdentityQuaternion2;
    };

    Quaternion2 operator*(Quaternion2 q1, const Quaternion2& q2);

#endif