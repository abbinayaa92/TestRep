#include <algorithm>
#include <GL/freeglut.h>
#include "Node.h"

using namespace std;

const double Node::kEpsilon = 0.000001;

// Default constructor, does nothing
Node::Node()
{
}

Node::Node(int dof, const VectorR3& attachmentPoint, const VectorR3 localRotationAxes[],
           const double rotationAngles[], double size, Purpose purpose, string name) :
    name_(name),
    dof_(dof),
    isSelected_(false),
    attachmentPoint_(attachmentPoint),
    size_(size),
    purpose_(purpose),
    isFrozen_(false),
    jointId_(-1),
    effectorId_(-1),
    localPosition_(attachmentPoint),
    parent_(NULL)
{
    rotationAngles_[X] = rotationAngles[X] * DegreesToRadians;
    rotationAngles_[Y] = dof_ >= 2 ? rotationAngles[Y] * DegreesToRadians : 0.0;
    rotationAngles_[Z] = dof_ >= 3 ? rotationAngles[Z] * DegreesToRadians : 0.0;

    localRotationAxes_[X] = localRotationAxes[X];
    localRotationAxes_[Y] = localRotationAxes[Y];
    localRotationAxes_[Z] = localRotationAxes[Z];
}

Quaternion2 Node::GetQuaternion() const
{
    return q;
}

Quaternion2 Node::GetGlobalQuaternion() const
{
	Quaternion2 globalq = q;
	Node* y = this->parent_;
	while (y)
	{
		globalq = y->q*globalq;
		y = y->parent_;
	}

	return globalq;
}

void Node::SetQuaternion(const Quaternion2 &p)
{
    q.w = p.w;
    q.x = p.x;
    q.y = p.y;
    q.z = p.z;
}

void Node::ComputeQuaternion()
{
    Quaternion2 qX;
    Quaternion2 qY;
    Quaternion2 qZ;
    qX.Set(rotationAngles_[X], VectorR3::UnitX);
    qY.Set(rotationAngles_[Y], VectorR3::UnitY);
    qZ.Set(rotationAngles_[Z], VectorR3::UnitZ);
    q = qX * qY * qZ;
}

void Node::ComputeMatrix() // from Quaternion2
{
    ComputeQuaternion();
    q = q.toUnit();

    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    m[0] = 1.0f - 2.0f * (y2 + z2);
    m[1] = 2.0f * (xy + wz);
    m[2] = 2.0f * (xz - wy);
    m[3] = 0.0f;
    m[4] = 2.0f * (xy - wz);
    m[5] = 1.0f - 2.0f * (x2 + z2);
    m[6] = 2.0f * (yz + wx);
    m[7] = 0.0f;
    m[8] = 2.0f * (xz + wy);
    m[9] = 2.0f * (yz - wx);
    m[10] = 1.0f - 2.0f * (x2 + y2);
    m[11] = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;

    /*m[0] = 1.0f - 2.0f * (y2 + z2);
    m[1] = 2.0f * (xy - wz);
    m[2] = 2.0f * (xz + wy);
    m[3] = 0.0f;
    m[4] = 2.0f * (xy + wz);
    m[5] = 1.0f - 2.0f * (x2 + z2);
    m[6] = 2.0f * (yz - wx);
    m[7] = 0.0f;
    m[8] = 2.0f * (xz - wy);
    m[9] = 2.0f * (yz + wx);
    m[10] = 1.0f - 2.0f * (x2 + y2);
    m[11] = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;*/
}

void Node::DrawUsingMatrix(bool isRoot)
{
    if (!isRoot) {
        DrawBox();
    }

    glTranslatef(localPosition_.x, localPosition_.y, localPosition_.z);
    double theta, x, y, z;
    q.GetAxisAngle(theta, x, y, z);
    glRotatef(theta * RadiansToDegrees, x, y, z);
    //glMultMatrixf(m);
    glutSolidSphere(size_, 12 ,12);
}

void Node::AddToTranslation(double translateX, double translateY, double translateZ)
{
    localPosition_.x += translateX;
    localPosition_.y += translateY;
    localPosition_.z += translateZ;
}

void Node::AddToTheta(double deltaAngleX, double deltaAngleY, double deltaAngleZ)
{
    if (dof_ < 2) {
        assert(abs(deltaAngleY) < kEpsilon && abs(deltaAngleZ) < kEpsilon);
    }

    if (dof_ < 3) {
        assert(abs(deltaAngleZ) < kEpsilon);
    }

    rotationAngles_[X] += deltaAngleX;
    rotationAngles_[Y] = dof_ >= 2 ? rotationAngles_[Y] + deltaAngleY : 0.0;
    rotationAngles_[Z] = dof_ >= 3 ? rotationAngles_[Z] + deltaAngleZ : 0.0;
}

/*
 * Computes the node's global position and rotation axes
 * TODO: use a faster method
 */
void Node::ComputeGlobalData()
{
    // express the local position in the coordinate frame of the parent
    globalPosition_ = localPosition_;

    // express the three local rotation axes in the coordinate frame of the parent
    globalRotationAxes_[X] = localRotationAxes_[X];
    globalRotationAxes_[Y] = localRotationAxes_[Y].Rotate(rotationAngles_[X],
                                                          localRotationAxes_[X]);
    globalRotationAxes_[Z] = localRotationAxes_[Z].Rotate(rotationAngles_[Y],
                                                          localRotationAxes_[Y]);
    globalRotationAxes_[Z] = globalRotationAxes_[Z].Rotate(rotationAngles_[X],
                                                           localRotationAxes_[X]);

    Node *y = this->parent_;
    while (y) {
       /* globalPosition_ = globalPosition_.Rotate(y->rotationAngles_[Z],
                                                 y->localRotationAxes_[Z]);
        globalPosition_ = globalPosition_.Rotate(y->rotationAngles_[Y],
                                                 y->localRotationAxes_[Y]);
        globalPosition_ = globalPosition_.Rotate(y->rotationAngles_[X],
                                                 y->localRotationAxes_[X]);*/
        globalPosition_ = y->q.rotate(globalPosition_);
        globalPosition_ += y->localPosition_;

        globalRotationAxes_[X] = globalRotationAxes_[X].Rotate(y->rotationAngles_[Z],
                                                               y->localRotationAxes_[Z]);
        globalRotationAxes_[X] = globalRotationAxes_[X].Rotate(y->rotationAngles_[Y],
                                                               y->localRotationAxes_[Y]);
        globalRotationAxes_[X] = globalRotationAxes_[X].Rotate(y->rotationAngles_[X],
                                                               y->localRotationAxes_[X]);
        globalRotationAxes_[Y] = globalRotationAxes_[Y].Rotate(y->rotationAngles_[Z],
                                                               y->localRotationAxes_[Z]);
        globalRotationAxes_[Y] = globalRotationAxes_[Y].Rotate(y->rotationAngles_[Y],
                                                               y->localRotationAxes_[Y]);
        globalRotationAxes_[Y] = globalRotationAxes_[Y].Rotate(y->rotationAngles_[X],
                                                               y->localRotationAxes_[X]);
        globalRotationAxes_[Z] = globalRotationAxes_[Z].Rotate(y->rotationAngles_[Z],
                                                               y->localRotationAxes_[Z]);
        globalRotationAxes_[Z] = globalRotationAxes_[Z].Rotate(y->rotationAngles_[Y],
                                                               y->localRotationAxes_[Y]);
        globalRotationAxes_[Z] = globalRotationAxes_[Z].Rotate(y->rotationAngles_[X],
                                                               y->localRotationAxes_[X]);

        y = y->parent_;
    }
}

// TODO: probably needs refactor
void Node::DrawBox() const
{
    glPushMatrix();

    if (localPosition_.z != 0.0 || localPosition_.x != 0.0) {
        double alpha = atan2(localPosition_.z, localPosition_.x);
        glRotatef(alpha * RadiansToDegrees, 0.0f, -1.0f, 0.0f);
    }

    if (localPosition_.y != 0.0) {
        double beta = atan2(localPosition_.y,
                            sqrt(localPosition_.x * localPosition_.x +
                                 localPosition_.z * localPosition_.z));
        glRotatef(beta * RadiansToDegrees, 0.0f, 0.0f, 1.0f);
    }

    double length = localPosition_.Norm();
    glScalef(length / size_, 1.0f, 1.0f);
    glTranslatef(size_ / 2, 0.0f, 0.0f);

    glColor3f(0.12f, 0.56f, 1.00f);
    glutSolidCube(size_);

    glPopMatrix();
}

void Node::Print(int level) const
{
    char *frozenStatus = isFrozen_ ? "Frozen" : "Free";
    char *type = (purpose_ == JOINT) ? (purpose_ == EFFECTOR ? "B" : "J") :
                                       (purpose_ == EFFECTOR ? "E" : "U");
    printf("%s[%d] %s %s %s (%.2f %.2f %.2f)\n", string(level, '.').c_str(), id_, name_.c_str(),
            type, frozenStatus, globalPosition_.x, globalPosition_.y,
            globalPosition_.z);
}

void Node::DrawCylinder(VectorR3 axis, float size) const
{
    glPushMatrix();
    axis.Normalize();
    double angle = acos(axis.Dot(VectorR3::UnitZ));
    axis = axis * VectorR3::UnitZ;
    glRotatef(-angle * RadiansToDegrees, axis.x, axis.y, axis.z);
    glTranslatef(0.0f, 0.0f, -size / 2.0f);
    SelectColor();
    glutSolidCylinder(0.06f, size, 12, 12);
    glPopMatrix();
}

void Node::DrawAxis(const float color[], const VectorR3 &axis, float size) const
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0);
    glColor3fv(color);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(axis.x * size, axis.y * size, axis.z * size);
    glEnd();
    glLineWidth(1.0);
    glEnable(GL_LIGHTING);
}

void Node::SelectColor() const
{
    if (id_ == 0)
        glColor3f(0.00f, 1.00f, 0.00f);
    else if (IsEffector())
        glColor3f(0.00f, 1.00f, 1.00f);
    else
        glColor3f(0.12f, 0.56f, 1.00f);
}

void Node::Draw(bool isRoot) const
{
    float          kRed[] = { 1.00f, 0.00f, 0.00f, 1.00f };
    float         kBlue[] = { 0.00f, 0.00f, 1.00f, 1.00f };
    float        kGreen[] = { 0.00f, 1.00f, 0.00f, 1.00f };

    if (!isRoot) {
        DrawBox();
    }

    glTranslatef(localPosition_.x, localPosition_.y, localPosition_.z);

    if (dof_ == 1) {
        DrawAxis(kRed, localRotationAxes_[X], size_ * 2.0f);
        DrawCylinder(localRotationAxes_[X], size_ * 1.5f);

        glRotatef(rotationAngles_[X] * RadiansToDegrees, localRotationAxes_[X].x,
                  localRotationAxes_[X].y, localRotationAxes_[X].z);
    } else if (dof_ == 2) {
        DrawAxis(kRed, localRotationAxes_[X], size_ * 2.0f);
        DrawCylinder(localRotationAxes_[X], size_ * 1.5f);

        glRotatef(rotationAngles_[X] * RadiansToDegrees, localRotationAxes_[X].x,
                  localRotationAxes_[X].y, localRotationAxes_[X].z);
        DrawAxis(kGreen, localRotationAxes_[Y], size_ * 2.0f);
        DrawCylinder(localRotationAxes_[Y], size_ * 1.5f);

        glRotatef(rotationAngles_[Y] * RadiansToDegrees, localRotationAxes_[Y].x,
                  localRotationAxes_[Y].y, localRotationAxes_[Y].z);
    } else if (dof_ == 3) {
        DrawAxis(kRed, localRotationAxes_[X], size_ * 2.0f);

        glRotatef(rotationAngles_[X] * RadiansToDegrees, localRotationAxes_[X].x,
                  localRotationAxes_[X].y, localRotationAxes_[X].z);
        DrawAxis(kGreen, localRotationAxes_[Y], size_ * 2.0f);

        glRotatef(rotationAngles_[Y] * RadiansToDegrees, localRotationAxes_[Y].x,
                  localRotationAxes_[Y].y, localRotationAxes_[Y].z);
        DrawAxis(kBlue, localRotationAxes_[Z], size_ * 2.0f);

        glRotatef(rotationAngles_[Z] * RadiansToDegrees, localRotationAxes_[Z].x,
                  localRotationAxes_[Z].y, localRotationAxes_[Z].z);
        SelectColor();
        glutSolidSphere(size_, 12 ,12);
    }

    /*if (isSelected_) {
        glColor3fv(kRed);
        glutWireCube(0.2f);
    }*/
}

void Node::DrawDebug(const vector<VectorR3> &targets) const
{
    if (IsEffector()) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.0f);
        const VectorR3 &myTarget = targets[effectorId_];
        glPushAttrib(GL_ENABLE_BIT);
        glLineStipple(1, 0xAAAA);
        glEnable(GL_LINE_STIPPLE);
        glBegin(GL_LINES);
        glVertex3f(globalPosition_.x, globalPosition_.y, globalPosition_.z);
        glVertex3f(myTarget.x, myTarget.y, myTarget.z);
        glEnd();
        glPopAttrib();
        glEnable(GL_LIGHTING);
    }
}