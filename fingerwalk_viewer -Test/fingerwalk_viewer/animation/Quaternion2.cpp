#include <algorithm>
#include "LinearR4.h"
#include "Quaternion2.h"

const Quaternion2 Quaternion2::IdentityQuaternion2(0.0, 0.0, 0.0, 1.0);
Quaternion2 operator*(Quaternion2, const Quaternion2&);

void Quaternion2::GetAxisAngle(double &theta, double &xx, double &yy, double &zz)
{
    toUnit();
    theta = 2 * acos(w);
    double s = sqrt(1 - w * w); // assuming Quaternion2 normalised then w is less than 1, so term always positive.
    if (s < 0.001) { // test to avoid divide by zero, s is always positive due to sqrt
        // if s close to zero then direction of axis not important
        xx = x; // if it is important that axis is normalised then replace with x=1; y=z=0;
        yy = y;
        zz = z;
    } else {
        xx = x / s; // normalise axis
        yy = y / s;
        zz = z / s;
    }
}

double Quaternion2::DotProductWith(const Quaternion2 &other) const
{
    return this->w * other.w + this->x * other.x + this->y * other.y +
        this->z * other.z;
}

inline double safeACOS(double val){
    if (val<-1)
        return PI;
    if (val>1)
        return 0;
    return acos(val);
}

inline double max(double a, double b)
{
    return (a<b)?b:a;
}

Quaternion2& Quaternion2::operator *= (double scalar){
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    this->w *= scalar;

    return *this;
}

Quaternion2& Quaternion2::toUnit() {
    *this *= (1/this->getLength());
    return *this;
}

double Quaternion2::getLength() const {
    return sqrt(x * x + y * y + z * z + w * w);
}

VectorR3 Quaternion2::rotate(const VectorR3& u) const {
    VectorR3 v(x, y, z);
    VectorR3 t = u * w + v * u;
    return v * v.Dot(u) + t * w + v * t;
}

Quaternion2 Quaternion2::DecomposeRotationReverse(const VectorR3 vB) const {
    //we need to compute v in A's coordinates
    VectorR3 vA = this->rotate(vB);
    vA.Normalize();

    double temp = 0;

    //compute the rotation that aligns the vector v in the two coordinate frames (A and T)
    VectorR3 rotAxis = vA * vB;
    rotAxis.Normalize();
    double rotAngle = safeACOS(vA.Dot(vB));

    Quaternion2 AqT = getRotationQuaternion2(rotAngle, rotAxis);
    //return TqA * (*this);
    return AqT;
}

Quaternion2 Quaternion2::DecomposeRotationReverse2(const VectorR3 vB) const {
    //we need to compute v in A's coordinates
    VectorR3 vA = this->rotate(vB);
    vA.Normalize();

    double temp = 0;

    //compute the rotation that aligns the vector v in the two coordinate frames (A and T)
    VectorR3 rotAxis = vA * vB;
    rotAxis.Normalize();
    double rotAngle = safeACOS(vA.Dot(vB));

    Quaternion2 AqT = getRotationQuaternion2(rotAngle, rotAxis);
    //return TqA * (*this);
    return (*this) * AqT.Inverse();
}

/**
    Assume that the current Quaternion2 represents the relative orientation between two coordinate frames A and B.
    This method decomposes the current relative rotation into a twist of frame B around the axis v passed in as a
    parameter, and another more arbitrary rotation.

    AqB = AqT * TqB, where T is a frame that is obtained by rotating frame B around the axis v by the angle
    that is returned by this function.

    In the T coordinate frame, v is the same as in B, and AqT is a rotation that aligns v from A to that
    from T.

    It is assumed that vB is a unit vector!! This method returns TqB, which represents a twist about
    the axis vB.
*/
Quaternion2 Quaternion2::DecomposeRotation(const VectorR3 vB) const {
    //we need to compute v in A's coordinates
    VectorR3 vA = this->rotate(vB);
    vA.Normalize();

    double temp = 0;

    //compute the rotation that aligns the vector v in the two coordinate frames (A and T)
    VectorR3 rotAxis = vA * vB;
    rotAxis.Normalize();
    double rotAngle = -safeACOS(vA.Dot(vB));

    Quaternion2 TqA = getRotationQuaternion2(rotAngle, rotAxis*(-1));
    //return TqA * (*this);
    return TqA;
}

Quaternion2 Quaternion2::DecomposeRotation2(const VectorR3 vB) const {
    //we need to compute v in A's coordinates
    VectorR3 vA = this->rotate(vB);
    vA.Normalize();

    double temp = 0;

    //compute the rotation that aligns the vector v in the two coordinate frames (A and T)
    VectorR3 rotAxis = vA * vB;
    rotAxis.Normalize();
    double rotAngle = -safeACOS(vA.Dot(vB));

    Quaternion2 TqA = getRotationQuaternion2(rotAngle, rotAxis*(-1));
    return TqA * (*this);
}

Quaternion2 Quaternion2::getInverse() const {
    double length = this->getLength();
    return (this->getComplexConjugate() * (1/(length*length))); // what if length = 0
}

Quaternion2 Quaternion2::getComplexConjugate() const {
    return Quaternion2(-x, -y, -z, w);
}

Quaternion2 Quaternion2::SlerpWith(const Quaternion2 &other, double t) const
{
    //make sure that we return the same value if either of the Quaternion2s involved is q or -q
    if (this->DotProductWith(other) < 0){
        Quaternion2 temp;
        temp.w = -other.w;
        temp.x = other.x * (-1);
        temp.y = other.y * (-1);
        temp.z = other.z * (-1);
        return this->SlerpWith(temp, t);
    }

    if (t<0) t = 0;
    if (t>1) t = 1;
    double dotProduct = this->DotProductWith(other);
    double sinTheta = sqrt(max(0,1-dotProduct*dotProduct));
    double theta = safeACOS(dotProduct);
    if (sinTheta == 0)
        return (*this);
    return ((*this) * sin(theta * (1-t)) + other * sin(theta * t)) * (1/sin(theta));
}

Quaternion2 ToQuat( const VectorR4& v)
{
    return Quaternion2(v.x,v.y,v.z,v.w);
}

Quaternion2 Quaternion2::getRotationQuaternion2(double angle, const VectorR3 &axis) const
{
    VectorR3 temp  = axis * sin(angle / 2);
    Quaternion2 result(temp.x, temp.y, temp.z, cos(angle / 2));
    return result;
}

double Quaternion2::Norm()
{
    return sqrt(x*x + y*y + z*z);
}

double Quaternion2::Angle ()
{
    double halfAngle = asin(Norm());
    return halfAngle+halfAngle;
}

VectorR4::VectorR4 (const Quaternion2& q)
: x(q.x), y(q.y), z(q.z), w(q.w)
{
}

Quaternion2& Quaternion2::Set(const VectorR3& point)
{
    x = point.x;
    y = point.y;
    z = point.z;
    w = 0.0;
    return *this;
}

VectorR4& VectorR4::Set (const Quaternion2& q)
{
    x = q.x;  y = q.y;  z = q.z; w = q.w;
    return *this;
}

Quaternion2::Quaternion2(double xx, double yy, double zz, double ww)
: x(xx), y(yy), z(zz), w(ww)
{
}

Quaternion2::Quaternion2(const Quaternion2& q)
: x(q.x), y(q.y), z(q.z), w(q.w)
{
}

Quaternion2& Quaternion2::Set( double xx, double yy, double zz, double ww )
{
    x = xx;
    y = yy;
    z = zz;
    w = ww;
    return *this;
}

Quaternion2& Quaternion2::Set( const VectorR4& u)
{
    x = u.x;
    y = u.y;
    z = u.z;
    w = u.w;
    return *this;
}

/* Creates a rotation Quaternion2 from an angle and a rotational axis */
Quaternion2& Quaternion2::Set(double angle, const VectorR3& axis)
{
    w = cos(angle / 2.0);
    x = sin(angle / 2.0) * axis.x;
    y = sin(angle / 2.0) * axis.y;
    z = sin(angle / 2.0) * axis.z;

    return *this;
}

Quaternion2& Quaternion2::SetIdentity()
{
    x = y = z = 0.0;
    w = 1.0;
    return *this;
}

Quaternion2 operator*(Quaternion2 q1, const Quaternion2& q2)
{
    q1 *= q2;
    q1.Negate();
    return q1;
}

Quaternion2& Quaternion2::operator*=(const Quaternion2& q)
{
    double wNew = w * q.w - (x * q.x + y * q.y + z * q.z);
    double xNew = w * q.x + q.w * x + (y * q.z - z * q.y);
    double yNew = w * q.y + q.w * y + (z * q.x - x * q.z);
    double zNew = w * q.z + q.w * z + (x * q.y - y * q.x);
    w = wNew;
    x = xNew;
    y = yNew;
    z = zNew;
    Negate();
    return *this;
}

Quaternion2 Quaternion2::Inverse()	const	// Return the Inverse
{
    assert(abs(x * x + y * y + z * z + w * w - 1.0) < 0.001);
    Quaternion2 retVal(-x, -y, -z, w);
    retVal.Negate();
    return (retVal);
}

Quaternion2& Quaternion2::Invert()		// Invert this Quaternion2
{
    x = -x;
    y = -y;
    z = -z;
    Negate();
    return *this;
}