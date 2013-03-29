// TODO: the clamping may be inappropriate when adding 3 more dof to the root

#include <GL/glut.h>
#include "Animation.h"
#include "Jacobian.h"

using namespace std;

void Arrow(const VectorR3& tail, const VectorR3& head);

extern int RestPositionOn;

// Optimal damping values have to be determined in an ad hoc manner  (Yuck!)
// const double Jacobian::DefaultDampingLambda = 0.6;		// Optimal for the "Y" shape (any lower gives jitter)
const double Jacobian::DefaultDampingLambda = 0.5;			// Optimal for the DLS "double Y" shape (any lower gives jitter)
// const double Jacobian::DefaultDampingLambda = 0.7;			// Optimal for the DLS "double Y" shape with distance clamping (lower gives jitter)

const double Jacobian::PseudoInverseThresholdFactor = 0.01;
const double Jacobian::MaxAngleJtranspose = 30.0 * DegreesToRadians;
const double Jacobian::MaxAnglePseudoinverse = 5.0 * DegreesToRadians;
const double Jacobian::MaxAngleDLS = 30.0 * DegreesToRadians;
const double Jacobian::MaxAngleSDLS = 30.0 * DegreesToRadians;
const double Jacobian::BaseMaxTargetDist = 0.4;

Jacobian::Jacobian(Tree *tree, vector<VectorR3> *targets)
{
    Jacobian::targets_ = targets;
    Jacobian::tree = tree;
    nEffector = tree->GetNumEffector();
    nJoint = tree->GetNumJoint();
    nRow = 3 * nEffector;
    nCol = 3 + 3 * nJoint;

    Jend.SetSize(nRow, nCol);				// The Jocobian matrix
    Jend.SetZero();
    Jtarget.SetSize(nRow, nCol);			// The Jacobian matrix based on target positions
    Jtarget.SetZero();
    SetJendActive();

    U.SetSize(nRow, nRow);				// The U matrix for SVD calculations
    w .SetLength(Min(nRow, nCol));
    V.SetSize(nCol, nCol);				// The V matrix for SVD calculations

    dS.SetLength(nRow);			// (Target positions) - (End effector positions)
    dTheta.SetLength(nCol);		// Changes in joint angles
    dPreTheta.SetLength(nCol);

    // Used by Jacobian transpose method & DLS & SDLS
    dT.SetLength(nRow);			// Linearized change in end effector positions based on dTheta

    // Used by the Selectively Damped Least Squares Method
    //dT.SetLength(nRow);
    dSclamp.SetLength(nEffector);
    errorArray.SetLength(nEffector);
    Jnorms.SetSize(nEffector, nCol);		// Holds the norms of the active J matrix

    Reset();
}

void Jacobian::Reset()
{
    // Used by Damped Least Squares Method
    DampingLambda = DefaultDampingLambda;
    DampingLambdaSq = Square(DampingLambda);
    // DampingLambdaSDLS = 1.5*DefaultDampingLambda;

    dSclamp.Fill(HUGE_VAL);
    //Jend.SetZero();
    //Jtarget.SetZero();
    //Jnorms.SetZero();
}

void Jacobian::Print()
{
    printf("--------------------------------\n");
    for (int i = 0; i < nRow; ++i) {
        for (int j = 0; j < nCol; ++j) {
            printf("%4.2f ", Jend.Get(i, j));
        }
        printf("\n");
    }
    printf("--------------------------------\n");
}

// Compute the deltaS vector, dS, (the error in end effector positions
// Compute the J and K matrices (the Jacobians)
void Jacobian::ComputeJacobian()
{
    // Traverse tree to find all end effectors
    VectorR3 temp, temp2;
    const vector<Node*> allNodes = tree->GetAllNodes();
    for (vector<Node*>::const_iterator it = allNodes.begin();
                                       it < allNodes.end();
                                       ++it) {
        Node *n = *it;
        if (n->IsEffector()) {
            int i = n->GetEffectorId();
            const VectorR3& targetPos = (*targets_)[i];

            // Compute the delta S value (differences from end effectors to target positions.
            temp = targetPos;
            temp -= n->GetGlobalPosition();
            dS.SetTriple(i, temp);

            // Find all ancestors (they will usually all be joints)
            // Set the corresponding entries in the Jacobians J, K.
            //Node *m = tree->GetParent(n);
            Node *m = n;
            while (m) {
                if (tree->IsRoot(m)) {
                    if (m->IsFrozen()) {
                        Jend.Set(i * 3 + 0, 0, 0);
                        Jend.Set(i * 3 + 0, 1, 0);
                        Jend.Set(i * 3 + 0, 2, 0);
                        Jend.Set(i * 3 + 1, 0, 0);
                        Jend.Set(i * 3 + 1, 1, 0);
                        Jend.Set(i * 3 + 1, 2, 0);
                        Jend.Set(i * 3 + 2, 0, 0);
                        Jend.Set(i * 3 + 2, 1, 0);
                        Jend.Set(i * 3 + 2, 2, 0);

                        Jtarget.Set(i * 3 + 0, 0, 0);
                        Jtarget.Set(i * 3 + 0, 1, 0);
                        Jtarget.Set(i * 3 + 0, 2, 0);
                        Jtarget.Set(i * 3 + 1, 0, 0);
                        Jtarget.Set(i * 3 + 1, 1, 0);
                        Jtarget.Set(i * 3 + 1, 2, 0);
                        Jtarget.Set(i * 3 + 2, 0, 0);
                        Jtarget.Set(i * 3 + 2, 1, 0);
                        Jtarget.Set(i * 3 + 2, 2, 0);
                    } else {
                        Jend.Set(i * 3 + 0, 0, 1);
                        Jend.Set(i * 3 + 0, 1, 0);
                        Jend.Set(i * 3 + 0, 2, 0);
                        Jend.Set(i * 3 + 1, 0, 0);
                        Jend.Set(i * 3 + 1, 1, 1);
                        Jend.Set(i * 3 + 1, 2, 0);
                        Jend.Set(i * 3 + 2, 0, 0);
                        Jend.Set(i * 3 + 2, 1, 0);
                        Jend.Set(i * 3 + 2, 2, 1);

                        Jtarget.Set(i * 3 + 0, 0, 1);
                        Jtarget.Set(i * 3 + 0, 1, 0);
                        Jtarget.Set(i * 3 + 0, 2, 0);
                        Jtarget.Set(i * 3 + 1, 0, 0);
                        Jtarget.Set(i * 3 + 1, 1, 1);
                        Jtarget.Set(i * 3 + 1, 2, 0);
                        Jtarget.Set(i * 3 + 2, 0, 0);
                        Jtarget.Set(i * 3 + 2, 1, 0);
                        Jtarget.Set(i * 3 + 2, 2, 1);
                    }
                }

                if (m->IsJoint()) {
                    int j = m->GetJointId();
                    assert (0 <=i && i < nEffector && 0 <= j && j < nJoint);
                    temp = m->GetGlobalPosition();			// joint pos.
                    temp -= n->GetGlobalPosition();			// -(end effector pos. - joint pos.)
                    Jend.SetTriple(i, 3 + j * 3, temp * m->GetGlobalRotationAxisX());
                    VectorR3 tt = temp * m->GetGlobalRotationAxisX();
                    if (m->GetDOF() >= 2) {
                        Jend.SetTriple(i, 3 + j * 3 + 1, temp * m->GetGlobalRotationAxisY());
                    } else
                        Jend.SetTriple(i, 3 + j * 3 + 1, VectorR3::Zero);
                    if (m->GetDOF() >= 3)
                        Jend.SetTriple(i, 3 + j * 3 + 2, temp * m->GetGlobalRotationAxisZ());
                    else
                        Jend.SetTriple(i, 3 + j * 3 + 2, VectorR3::Zero);

                    temp = m->GetGlobalPosition();			// joint pos.
                    temp -= targetPos;		// -(target pos. - joint pos.)
                    Jtarget.SetTriple(i, 3 + j * 3, temp * m->GetGlobalRotationAxisX());
                    if (m->GetDOF() >= 2)
                        Jtarget.SetTriple(i, 3 + j * 3 + 1, temp * m->GetGlobalRotationAxisY());
                    else
                        Jtarget.SetTriple(i, 3 + j * 3 + 1, VectorR3::Zero);
                    if (m->GetDOF() >= 3)
                        Jtarget.SetTriple(i, 3 + j * 3 + 2, temp * m->GetGlobalRotationAxisZ());
                    else
                        Jtarget.SetTriple(i, 3 + j * 3 + 2, VectorR3::Zero);
                }
                m = tree->GetParent(m);
            }
        }
    }
}

// The delta theta values have been computed in dTheta array
// Apply the delta theta values to the joints
// Nothing is done about joint limits for now.
void Jacobian::UpdateThetas()
{
    // Traverse the tree to find all joints
    // Update the joint angles
    const vector<Node*> allNodes = tree->GetAllNodes();
	for (vector<Node*>::const_iterator it = allNodes.begin(); it < allNodes.end(); ++it) {
        Node *n = *it;
        if (n->IsJoint()) {
            int i = n->GetJointId();
            n->AddToTheta(dTheta[3 + i * 3], dTheta[3 + i * 3 + 1], dTheta[3 + i * 3 + 2]);
        }

        if (tree->IsRoot(n)) {
            n->AddToTranslation(dTheta[0], dTheta[1], dTheta[2]);
        }
    }

    // Update the positions and rotation axes of all joints/effectors
    tree->Compute();
}

void Jacobian::CalcDeltaThetas()
{
    switch (CurrentUpdateMode) {
        case JACOB_Undefined:
            ZeroDeltaThetas();
            break;
        case JACOB_JacobianTranspose:
            CalcDeltaThetasTranspose();
            break;
        case JACOB_PseudoInverse:
            CalcDeltaThetasPseudoinverse();
            break;
        case JACOB_DLS:
            CalcDeltaThetasDLS();
            break;
        case JACOB_SDLS:
            CalcDeltaThetasSDLS();
            break;
    }
}

void Jacobian::ZeroDeltaThetas()
{
    dTheta.SetZero();
}

// Find the delta theta values using inverse Jacobian method
// Uses a greedy method to decide scaling factor
void Jacobian::CalcDeltaThetasTranspose()
{
    const MatrixRmn& J = ActiveJacobian();

    J.MultiplyTranspose(dS, dTheta);

    // Scale back the dTheta values greedily
    J.Multiply(dTheta, dT);    // dT = J * dTheta
    double alpha = Dot(dS, dT) / dT.NormSq();
    assert (alpha > 0.0);
    // Also scale back to be have max angle change less than MaxAngleJtranspose
    double maxChange = dTheta.MaxAbs();
    double beta = MaxAngleJtranspose/maxChange;
    dTheta *= Min(alpha, beta);
}

void Jacobian::CalcDeltaThetasPseudoinverse()
{
    MatrixRmn& J = const_cast<MatrixRmn&>(ActiveJacobian());

    // Compute Singular Value Decomposition
    //	This an inefficient way to do Pseudoinverse, but it is convenient since we need SVD anyway

    J.ComputeSVD( U, w, V );

    // Next line for debugging only
    assert(J.DebugCheckSVD(U, w , V));

    // Calculate response vector dTheta that is the DLS solution.
    //	Delta target values are the dS values
    //  We multiply by Moore-Penrose pseudo-inverse of the J matrix
    double pseudoInverseThreshold = PseudoInverseThresholdFactor*w.MaxAbs();

    long diagLength = w.GetLength();
    double* wPtr = w.GetPtr();
    dTheta.SetZero();
    for ( long i=0; i<diagLength; i++ ) {
        double dotProdCol = U.DotProductColumn( dS, i );		// Dot product with i-th column of U
        double alpha = *(wPtr++);
        if ( fabs(alpha)>pseudoInverseThreshold ) {
            alpha = 1.0/alpha;
            MatrixRmn::AddArrayScale(V.GetNumRows(), V.GetColumnPtr(i), 1, dTheta.GetPtr(), 1, dotProdCol*alpha );
        }
    }

    // Scale back to not exceed maximum angle changes
    double maxChange = dTheta.MaxAbs();
    if ( maxChange>MaxAnglePseudoinverse ) {
        dTheta *= MaxAnglePseudoinverse/maxChange;
    }
}

void Jacobian::CalcDeltaThetasDLS()
{
    const MatrixRmn& J = ActiveJacobian();

    MatrixRmn::MultiplyTranspose(J, J, U);		// U = J * (J^T)
    U.AddToDiagonal( DampingLambdaSq );

    // Use the next four lines instead of the succeeding two lines for the DLS method with clamped error vector e.
    // CalcdTClampedFromdS();
    // VectorRn dTextra(3*nEffector);
    // U.Solve( dT, &dTextra );
    // J.MultiplyTranspose( dTextra, dTheta );

    // Use these two lines for the traditional DLS method
    U.Solve( dS, &dT );
    J.MultiplyTranspose( dT, dTheta );

    // Scale back to not exceed maximum angle changes
    double maxChange = dTheta.MaxAbs();
    if ( maxChange>MaxAngleDLS ) {
        dTheta *= MaxAngleDLS/maxChange;
    }
}

void Jacobian::CalcDeltaThetasDLSwithSVD()
{
    const MatrixRmn& J = ActiveJacobian();

    // Compute Singular Value Decomposition
    //	This an inefficient way to do DLS, but it is convenient since we need SVD anyway

    J.ComputeSVD(U, w, V);

    // Next line for debugging only
    assert(J.DebugCheckSVD(U, w , V));

    // Calculate response vector dTheta that is the DLS solution.
    //	Delta target values are the dS values
    //  We multiply by DLS inverse of the J matrix
    long diagLength = w.GetLength();
    double* wPtr = w.GetPtr();
    dTheta.SetZero();
    for (long i = 0; i < diagLength; i++) {
        double dotProdCol = U.DotProductColumn(dS, i);		// Dot product with i-th column of U
        double alpha = *(wPtr++);
        alpha = alpha / (Square(alpha) + DampingLambdaSq);
        MatrixRmn::AddArrayScale(V.GetNumRows(), V.GetColumnPtr(i), 1, dTheta.GetPtr(), 1, dotProdCol * alpha);
    }

    // Scale back to not exceed maximum angle changes
    double maxChange = dTheta.MaxAbs();
    if (maxChange>MaxAngleDLS) {
        dTheta *= MaxAngleDLS/maxChange;
    }
}

void Jacobian::CalcDeltaThetasSDLS()
{
    const MatrixRmn& J = ActiveJacobian();

    // Compute Singular Value Decomposition

    J.ComputeSVD( U, w, V );

    // Next line for debugging only
    assert(J.DebugCheckSVD(U, w , V));

    // Calculate response vector dTheta that is the SDLS solution.
    //	Delta target values are the dS values
    int nRows = J.GetNumRows();
    int numEndEffectors = tree->GetNumEffector();		// Equals the number of rows of J divided by three
    int nCols = J.GetNumColumns();
    dTheta.SetZero();

    // Calculate the norms of the 3-vectors in the Jacobian
    long i;
    const double *jx = J.GetPtr();
    double *jnx = Jnorms.GetPtr();
    for ( i=nCols*numEndEffectors; i>0; i-- ) {
        double accumSq = Square(*(jx++));
        accumSq += Square(*(jx++));
        accumSq += Square(*(jx++));
        *(jnx++) = sqrt(accumSq);
    }

    // Clamp the dS values
    CalcdTClampedFromdS();

    // Loop over each singular vector
    for ( i=0; i<nRows; i++ ) {
        double wiInv = w[i];
        if ( NearZero(wiInv,1.0e-10) ) {
            continue;
        }
        wiInv = 1.0/wiInv;

        double N = 0.0;						// N is the quasi-1-norm of the i-th column of U
        double alpha = 0.0;					// alpha is the dot product of dT and the i-th column of U

        const double *dTx = dT.GetPtr();
        const double *ux = U.GetColumnPtr(i);
        long j;
        for ( j=numEndEffectors; j>0; j-- ) {
            double tmp;
            alpha += (*ux)*(*(dTx++));
            tmp = Square( *(ux++) );
            alpha += (*ux)*(*(dTx++));
            tmp += Square(*(ux++));
            alpha += (*ux)*(*(dTx++));
            tmp += Square(*(ux++));
            N += sqrt(tmp);
        }

        // M is the quasi-1-norm of the response to angles changing according to the i-th column of V
        //		Then is multiplied by the wiInv value.
        double M = 0.0;
        double *vx = V.GetColumnPtr(i);
        jnx = Jnorms.GetPtr();
        for ( j=nCols; j>0; j-- ) {
            double accum=0.0;
            for ( long k=numEndEffectors; k>0; k-- ) {
                accum += *(jnx++);
            }
            M += fabs((*(vx++)))*accum;
        }
        M *= fabs(wiInv);

        double gamma = MaxAngleSDLS;
        if ( N<M ) {
            gamma *= N/M;				// Scale back maximum permissable joint angle
        }

        // Calculate the dTheta from pure pseudoinverse considerations
        double scale = alpha*wiInv;			// This times i-th column of V is the psuedoinverse response
        dPreTheta.LoadScaled( V.GetColumnPtr(i), scale );
        // Now rescale the dTheta values.
        double max = dPreTheta.MaxAbs();
        double rescale = (gamma)/(gamma+max);
        dTheta.AddScaled(dPreTheta,rescale);
    }

    // Scale back to not exceed maximum angle changes
    double maxChange = dTheta.MaxAbs();
    if ( maxChange>MaxAngleSDLS ) {
        dTheta *= MaxAngleSDLS/(MaxAngleSDLS+maxChange);
    }
}

void Jacobian::CalcdTClampedFromdS()
{
    long len = dS.GetLength();
    long j = 0;
    for ( long i=0; i<len; i+=3, j++ ) {
        double normSq = Square(dS[i])+Square(dS[i+1])+Square(dS[i+2]);
        if ( normSq>Square(dSclamp[j]) ) {
            double factor = dSclamp[j]/sqrt(normSq);
            dT[i] = dS[i]*factor;
            dT[i+1] = dS[i+1]*factor;
            dT[i+2] = dS[i+2]*factor;
        }
        else {
            dT[i] = dS[i];
            dT[i+1] = dS[i+1];
            dT[i+2] = dS[i+2];
        }
    }
}

double Jacobian::UpdateErrorArray()
{
    double totalError = 0.0;

    // Traverse tree to find all end effectors
    VectorR3 temp;
    const vector<Node*> allNodes = tree->GetAllNodes();
    for (vector<Node*>::const_iterator it = allNodes.begin(); it < allNodes.end(); ++it) {
        Node *n = *it;
        if ( n->IsEffector() ) {
            int i = n->GetEffectorId();
            const VectorR3& targetPos = (*targets_)[i];
            temp = targetPos;
            temp -= n->GetGlobalPosition();
            double err = temp.Norm();
            errorArray[i] = err;
            totalError += err;
        }
    }
    return totalError;
}

void Jacobian::UpdatedSClampValue()
{
    // Traverse tree to find all end effectors
    VectorR3 temp;
    const vector<Node*> allNodes = tree->GetAllNodes();
    for (vector<Node*>::const_iterator it = allNodes.begin(); it < allNodes.end(); ++it) {
        Node *n = *it;
        if ( n->IsEffector() ) {
            int i = n->GetEffectorId();
            const VectorR3& targetPos = (*targets_)[i];

            // Compute the delta S value (differences from end effectors to target positions.
            // While we are at it, also update the clamping values in dSclamp;
            temp = targetPos;
            temp -= n->GetGlobalPosition();
            double normSi = sqrt(Square(dS[i])+Square(dS[i+1])+Square(dS[i+2]));
            double changedDist = temp.Norm()-normSi;
            if ( changedDist>0.0 ) {
                dSclamp[i] = BaseMaxTargetDist + changedDist;
            } else {
                dSclamp[i] = BaseMaxTargetDist;
            }
        }
    }
}