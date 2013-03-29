#ifndef NODE_H_
#define NODE_H_

#include "Quaternion2.h"
#include <vector>
#include "LinearR3.h"

enum Purpose { JOINT, EFFECTOR, BOTH };

extern const float kRed[];
extern const float kBlue[];
extern const float kGreen[];
extern const float kCyan[];
extern const float kYellow[];
extern const float kNightSkyBlue[];

class Node
{
    friend class Tree;

public:
    VectorR3 GetLocalPosition() const { return localPosition_; };
    Quaternion2 GetQuaternion() const;
	Quaternion2 GetGlobalQuaternion() const;
    Node();
    Node(int dof, const VectorR3& attachmentPoint, const VectorR3 localRotationAxes[], const double rotationAngles[], double size, Purpose purpose, string name = "joint");
    void AddToTranslation(double translateX, double translateY, double translateZ);
    void AddToTheta(double deltaAngleX, double deltaAngleY, double deltaAngleZ);

    void SetQuaternion(const Quaternion2 &p);
    void Draw(bool isRoot) const;
    void ComputeGlobalData();
    void Print(int level) const;
    int GetEffectorId() const { return effectorId_; }
    int GetJointId() const { return jointId_; }
    bool IsEffector() const { return purpose_ == EFFECTOR || purpose_ == BOTH; }
    bool IsJoint() const { return purpose_ == JOINT || purpose_ == BOTH; }
    bool IsFrozen() const { return isFrozen_; }
    const VectorR3& GetGlobalRotationAxisX() const { return globalRotationAxes_[X]; };
    const VectorR3& GetGlobalRotationAxisY() const { return globalRotationAxes_[Y]; };
    const VectorR3& GetGlobalRotationAxisZ() const { return globalRotationAxes_[Z]; };
    const VectorR3& GetGlobalPosition() const { return globalPosition_; }
    void PrintDebug() const;
    void DrawDebug(const vector<VectorR3> &targets) const;
    int GetDOF() const { return dof_; };
    int GetId() const { return id_; };
    void Freeze() { isFrozen_ = true; };
    void Unfreeze() { isFrozen_ = false; };
    int CountFrozenChildren() const;
    int CountFrozenNeighbors() const;
    void Select() { isSelected_ = true; }
    void Unselect() { isSelected_ = false; }
    void MakeJoint() { purpose_ = JOINT; }
    void MakeEffector() { purpose_ = EFFECTOR; }
    void MakeBoth() { purpose_ = BOTH; }
    void SetIndex(int index) { index_ = index; }
    int GetIndex() const { return index_; }
    void ComputeQuaternion(); // from euler angles
    void ComputeMatrix(); // from Quaternion2
    void DrawUsingMatrix(bool isRoot);
    void SetPosition(const VectorR3 &pos) { localPosition_ = pos; }
    void SetPosition(double x, double y, double z) { localPosition_.Set(x, y, z); }
    void SetRotation(const VectorR3 &rot) { rotationAngles_[X] = rot.x;
                                            rotationAngles_[Y] = rot.y;
                                            rotationAngles_[Z] = rot.z; }
    void SetRotation(double x, double y, double z) { rotationAngles_[X] = x;
                                                     rotationAngles_[Y] = y;
                                                     rotationAngles_[Z] = z;
                                                   }

    VectorR3 GetRotationAngles() const { return VectorR3(rotationAngles_[X],
                                                     rotationAngles_[Y],
                                                     rotationAngles_[Z]);}
private:
    Quaternion2 q;
    float m[16]; // column-major transformation matrix
    static const double kEpsilon;
    enum Component { X = 0, Y = 1, Z = 2 };

    int index_;                        // a general index that can be used to
                                       // index this node in some list
    int id_;
    int dof_;                          // degree of freedom (1, 2, or 3)
    bool isSelected_;                  //
    bool isFrozen_;                    // true if the node is frozen
    int jointId_;                      // sequence number if this node is a joint
    int effectorId_;                   // sequence number if this node is an effector
    double size_;                      // size
    Purpose purpose_;                  // joint / effector / both
    VectorR3 localRotationAxes_[3];    // local rotation axes (there are 3 of them at most)
    VectorR3 globalRotationAxes_[3];   // global rotation axes (there are 3 of them at most)
    double rotationAngles_[3];         // rotation angles (there are 3 of them at most)
    VectorR3 attachmentPoint_;         // attachment point
    VectorR3 localPosition_;           // relative position vector
    VectorR3 globalPosition_;          // global position
    Node *parent_;                     // pointer to real parent
    vector<Node*> children_;           // a list of children
    string name_;

    void DrawBox() const;
    void DrawCylinder(VectorR3 axis, float size) const;
    void DrawShape() const;
    void DrawAxis(const float color[], const VectorR3 &axis, float size) const;
    void SelectColor() const ;
};

#endif    // NODE_H_