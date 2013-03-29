#include "LinearR4.h"
#include "Quaternion.h"

const Quaternion Quaternion::IdentityQuaternion(0.0, 0.0, 0.0, 1.0);
Quaternion operator*(Quaternion, const Quaternion&);

Quaternion ToQuat( const VectorR4& v)
{
    return Quaternion(v.x,v.y,v.z,v.w);
}

double Quaternion::Norm()
{
    return sqrt(x*x + y*y + z*z);
}

double Quaternion::Angle ()
{
    double halfAngle = asin(Norm());
    return halfAngle+halfAngle;
}

VectorR4::VectorR4 (const Quaternion& q)
: x(q.x), y(q.y), z(q.z), w(q.w)
{
}

Quaternion& Quaternion::Set(const VectorR3& point)
{
    x = point.x;
    y = point.y;
    z = point.z;
    w = 0.0;
    return *this;
}

VectorR4& VectorR4::Set (const Quaternion& q)
{
    x = q.x;  y = q.y;  z = q.z; w = q.w;
    return *this;
}

Quaternion::Quaternion(double xx, double yy, double zz, double ww)
: x(xx), y(yy), z(zz), w(ww)
{
}

Quaternion::Quaternion(const Quaternion& q)
: x(q.x), y(q.y), z(q.z), w(q.w)
{
}

Quaternion& Quaternion::Set( double xx, double yy, double zz, double ww )
{
    x = xx;
    y = yy;
    z = zz;
    w = ww;
    return *this;
}

Quaternion& Quaternion::Set( const VectorR4& u)
{
    x = u.x;
    y = u.y;
    z = u.z;
    w = u.w;
    return *this;
}

/* Creates a rotation quaternion from an angle and a rotational axis */
Quaternion& Quaternion::Set(double angle, const VectorR3& axis)
{
    w = cos(angle / 2.0);
    x = sin(angle / 2.0) * axis.x;
    y = sin(angle / 2.0) * axis.y;
    z = sin(angle / 2.0) * axis.z;

    return *this;
}

Quaternion& Quaternion::SetIdentity()
{
    x = y = z = 0.0;
    w = 1.0;
    return *this;
}

Quaternion operator*(Quaternion q1, const Quaternion& q2)
{
    return q1 *= q2;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
    double wNew = w * q.w - (x * q.x + y * q.y + z * q.z);
    double xNew = w * q.x + q.w * x + (y * q.z - z * q.y);
    double yNew = w * q.y + q.w * y + (z * q.x - x * q.z);
    double zNew = w * q.z + q.w * z + (x * q.y - y * q.x);
    w = wNew;
    x = xNew;
    y = yNew;
    z = zNew;
    return *this;
}

Quaternion& Quaternion::operator=(const Quaternion& q)
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
    return *this;
}

Quaternion Quaternion::Inverse()	const	// Return the Inverse
{
    assert(abs(x * x + y * y + z * z + w * w - 1.0) < 0.001);
    return (Quaternion(-x, -y, -z, w));
}

Quaternion& Quaternion::Invert()		// Invert this quaternion
{
    w = -w;
    return *this;
}