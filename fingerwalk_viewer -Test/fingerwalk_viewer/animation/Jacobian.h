#ifndef _CLASS_JACOBIAN
#define _CLASS_JACOBIAN

#include <vector>

#include "Node.h"
#include "Tree.h"
#include "MathMisc.h"
#include "LinearR3.h"
#include "VectorRn.h"
#include "MatrixRmn.h"

#ifdef _DYNAMIC
const double BASEMAXDIST = 0.02;
#else
const double MAXDIST = 0.08;	// optimal value for double Y shape : 0.08
#endif
const double DELTA = 0.4;
const long double LAMBDA = 2.0;		// only for DLS. optimal : 0.24
const double NEARZERO = 0.0000000001;

enum UpdateMode {
    JACOB_Undefined = 0,
    JACOB_JacobianTranspose = 1,
    JACOB_PseudoInverse = 2,
    JACOB_DLS = 3,
    JACOB_SDLS = 4 };

class Jacobian {
public:
    Jacobian(Tree*, vector<VectorR3> *targets);
    void Print();
    void ComputeJacobian();
    const MatrixRmn& ActiveJacobian() const { return *Jactive; }
    void SetJendActive() { Jactive = &Jend; }						// The default setting is Jend.
    void SetJtargetActive() { Jactive = &Jtarget; }

    void CalcDeltaThetas();			// Use this only if the Current Mode has been set.
    void ZeroDeltaThetas();
    void CalcDeltaThetasTranspose();
    void CalcDeltaThetasPseudoinverse();
    void CalcDeltaThetasDLS();
    void CalcDeltaThetasDLSwithSVD();
    void CalcDeltaThetasSDLS();

    void UpdateThetas();
    double UpdateErrorArray();		// Returns sum of errors
    const VectorRn& GetErrorArray() const { return errorArray; }
    void UpdatedSClampValue();
    void DrawEigenVectors() const;

    void SetCurrentMode( UpdateMode mode ) { CurrentUpdateMode = mode; }
    UpdateMode GetCurrentMode() const { return CurrentUpdateMode; }
    void SetDampingDLS( double lambda ) { DampingLambda = lambda; DampingLambdaSq = Square(lambda); }

    void Reset();

    static void CompareErrors( const Jacobian& j1, const Jacobian& j2, double* weightedDist1, double* weightedDist2 );
    static void CountErrors( const Jacobian& j1, const Jacobian& j2, int* numBetter1, int* numBetter2, int* numTies );

private:
    vector<VectorR3> *targets_;
    Tree* tree;			// tree associated with this Jacobian matrix
    int nEffector;		// Number of end effectors
    int nJoint;			// Number of joints
    int nRow;			// Total number of rows the real J (= 3*number of end effectors for now)
    int nCol;			// Total number of columns in the real J (= number of joints for now)

    MatrixRmn Jend;		// Jacobian matrix based on end effector positions
    MatrixRmn Jtarget;	// Jacobian matrix based on target positions
    MatrixRmn Jnorms;	// Norms of 3-vectors in active Jacobian (SDLS only)

    MatrixRmn U;		// J = U * Diag(w) * V^T	(Singular Value Decomposition)
    VectorRn w;
    MatrixRmn V;

    UpdateMode CurrentUpdateMode;

    VectorRn dS;			// delta s
    VectorRn dT;			// delta t		--  these are delta S values clamped to smaller magnitude
    VectorRn dSclamp;		// Value to clamp magnitude of dT at.
    VectorRn dTheta;		// delta theta
    VectorRn dPreTheta;		// delta theta for single eigenvalue  (SDLS only)

    VectorRn errorArray;	// Distance of end effectors from target after updating

    // Parameters for pseudoinverses
    static const double PseudoInverseThresholdFactor;		// Threshold for treating eigenvalue as zero (fraction of largest eigenvalue)

    // Parameters for damped least squares
    static const double DefaultDampingLambda;
    double DampingLambda;
    double DampingLambdaSq;
    //double DampingLambdaSDLS;

    // Cap on max. value of changes in angles in single update step
    static const double MaxAngleJtranspose;
    static const double MaxAnglePseudoinverse;
    static const double MaxAngleDLS;
    static const double MaxAngleSDLS;
    MatrixRmn* Jactive;

    void CalcdTClampedFromdS();
    static const double BaseMaxTargetDist;
};

#endif