/*
 *
 * RayTrace Software Package, release 1.0.1,  May 9, 2002.
 *
 * Mathematics Subpackage (VrMath)
 *
 * Author: Samuel R. Buss
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

//
// Spherical Operations Classes
//
//
// B. SphericalInterpolator
//
// OrientationR3
//
// A. Quaternion		
//
// B. RotationMapR3		// Elsewhere
//
// C. EulerAnglesR3		// TO DO
//
//
// Functions for spherical operations
// A. Many routines for rotation and averaging on a sphere
//

#ifndef SPHERICAL_H
#define SPHERICAL_H

#include "LinearR3.h"
#include "LinearR4.h"
#include "MathMisc.h"

class SphericalInterpolator;		// Spherical linear interpolation of vectors
class SphericalBSpInterpolator;	// Spherical Bspline interpolation of vector
class Quaternion;			// Quaternion (x,y,z,w) values.
class EulerAnglesR3;		// Euler Angles

// *****************************************************
// SphericalInterpolator class                         *
//    - Does linear interpolation (slerp-ing)		   *
// * * * * * * * * * * * * * * * * * * * * * * * * * * *


class SphericalInterpolator {

private:
    VectorR3 startDir, endDir;	// Unit vectors (starting and ending)
    double startLen, endLen;	// Magnitudes of the vectors
    double rotRate;				// Angle between start and end vectors

public:
    SphericalInterpolator( const VectorR3& u, const VectorR3& v );

    VectorR3 InterValue ( double frac ) const;
};


// ***************************************************************
// * Quaternion class - prototypes								 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *




// ****************************************************************
// Solid Geometry Routines										  *
// ****************************************************************

// Compute the angle formed by two geodesics on the unit sphere.
//	Three unit vectors u,v,w specify the geodesics u-v and v-w which
//  meet at vertex uv.  The angle from v-w to v-u is returned.  This
//  is always in the range [0, 2PI).
double SphereAngle( const VectorR3& u, const VectorR3& v, const VectorR3& w );

//  Compute the area of a triangle on the unit sphere.  Three unit vectors
//		specify the corners of the triangle in COUNTERCLOCKWISE order.
inline double SphericalTriangleArea(
                        const VectorR3& u, const VectorR3& v, const VectorR3& w )
{
    double AngleA = SphereAngle( u,v,w );
    double AngleB = SphereAngle( v,w,u );
    double AngleC = SphereAngle( w,u,v );
    return ( AngleA+AngleB+AngleC - PI );
}


// ****************************************************************
// Spherical Mean routines										  *
// ****************************************************************

// Weighted sum of vectors
VectorR3 WeightedSum( long Num, const VectorR3 vv[], const double weights[] );
VectorR4 WeightedSum( long Num, const VectorR4 vv[], const double weights[] );

// Weighted average of vectors on the sphere.
//		Sum of weights should equal one (but no checking is done)
VectorR3 ComputeMeanSphere( long Num, const VectorR3 vv[], const double weights[],
                          double tolerance = 1.0e-15, double bkuptolerance = 1.0e-13 );
VectorR3 ComputeMeanSphere( long Num, const VectorR3 vv[], const double weights[],
                          const VectorR3& InitialVec,
                          double tolerance = 1.0e-15, double bkuptolerance = 1.0e-13 );
VectorR4 ComputeMeanSphere( long Num, const VectorR4 vv[], const double weights[],
                          double tolerance = 1.0e-15, double bkuptolerance = 1.0e-13 );
Quaternion ComputeMeanQuat( long Num, const Quaternion qq[], const double weights[],
                           double tolerance = 1.0e-15, double bkuptolerance = 1.0e-13 );

// Next functions mostly for internal use.
//		It takes an initial estimate InitialVec (and a flag for
//		indicating quaternions).
VectorR4 ComputeMeanSphere( long Num, const VectorR4 vv[], const double weights[],
                           const VectorR4& InitialVec, int QuatFlag=0,
                           double tolerance = 1.0e-15, double bkuptolerance = 1.0e-13 );
const int SPHERICAL_NOTQUAT=0;
const int SPHERICAL_QUAT=1;

// Slow version, mostly for testing
VectorR3 ComputeMeanSphereSlow( long Num, const VectorR3 vv[], const double weights[],
                            double tolerance = 1.0e-16, double bkuptolerance = 5.0e-16 );
VectorR4 ComputeMeanSphereSlow( long Num, const VectorR4 vv[], const double weights[],
                            double tolerance = 1.0e-16, double bkuptolerance = 5.0e-16 );
VectorR3 ComputeMeanSphereOld( long Num, const VectorR3 vv[], const double weights[],
                          double tolerance = 1.0e-15, double bkuptolerance = 1.0e-13 );
VectorR4 ComputeMeanSphereOld( long Num, const VectorR4 vv[], const double weights[],
                           const VectorR4& InitialVec, int QuatFlag,
                           double tolerance = 1.0e-15, double bkuptolerance = 1.0e-13 );

// Solves a system of spherical-mean equalities, where the system is
// given as a tridiagonal matrix.
void SolveTriDiagSphere ( int numPoints,
                           const double* tridiagvalues, const VectorR3* c,
                           VectorR3* p,
                           double accuracy=1.0e-15, double bkupaccuracy=1.0e-13 );
void SolveTriDiagSphere ( int numPoints,
                           const double* tridiagvalues, const VectorR4* c,
                           VectorR4* p,
                           double accuracy=1.0e-15, double bkupaccuracy=1.0e-13 );

//  The "Slow" version uses a simpler but slower iteration with a linear rate of
//		convergence.  The base version uses a Newton iteration with a quadratic
//		rate of convergence.
void SolveTriDiagSphereSlow ( int numPoints,
                           const double* tridiagvalues, const VectorR3* c,
                           VectorR3* p,
                           double accuracy=1.0e-15, double bkupaccuracy=5.0e-15 );
void SolveTriDiagSphereSlow ( int numPoints,
                           const double* tridiagvalues, const VectorR4* c,
                           VectorR4* p,
                           double accuracy=1.0e-15, double bkupaccuracy=5.0e-15 );

// The "Unstable" version probably shouldn't be used except for very short sequences
//		of knots.  Mostly it's used for testing purposes now.
void SolveTriDiagSphereUnstable ( int numPoints,
                           const double* tridiagvalues, const VectorR3* c,
                           VectorR3* p,
                           double accuracy=1.0e-15, double bkupaccuracy=1.0e-13 );
void SolveTriDiagSphereHelperUnstable ( int numPoints,
                                const double* tridiagvalues, const VectorR3 *c,
                                const VectorR3& p0value,
                                VectorR3 *p,
                                double accuracy=1.0e-15, double bkupaccuracy=1.0e-13 );



// ***************************************************************
// * Quaternion class - inlined member functions				 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *




#endif // SPHERICAL_H