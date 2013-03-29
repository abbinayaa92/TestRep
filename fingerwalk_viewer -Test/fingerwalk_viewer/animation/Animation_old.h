#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include "Eigen/dense"
#include "LinearR3.h"
#include "Node.h"
#include "Tree.h"
#include "Jacobian.h"
#include "Quaternion2.h"
#include "../GLWindow.h"
#include "../Foot.h"

#define LEFT_TOE 5
#define RIGHT_TOE 10
#define LEFT_FOOT 4
#define RIGHT_FOOT 9
#define LEFT_ANKLE 3
#define RIGHT_ANKLE 8
#define LEFT_WRIST 19
#define RIGHT_WRIST 26

#define MODEL_COUNT 35
#define WALK_INPLACE 0
#define WALK_FORWARD 1
#define WALK_BACKWARD 2
#define RUN_INPLACE 3
#define RUN_FAST 4
#define JUMP_SHORT_INPLACE 5
#define JUMP_SHORT_FORWARD 6
#define TIPTOE_INPLACE 7
#define TIPTOE_FORWARD 8
#define TIPTOE_BACKWARD 9
#define PINO_WALK_INPLACE 10
#define PINO_WALK_FORWARD 11
#define PINO_WALK_BACKWARD 12
#define LIMP_INPLACE 13
#define LIMP_FORWARD 14
#define LIMP_BACKWARD 15
#define JUMP_LONG_INPLACE 16
#define JUMP_LONG_FORWARD 17
#define JOG 18
#define RUN_NORMAL 19
#define CATWALK_FORWARD 20
#define MOONWALK_FORWARD 21
#define ONE_HOPPING 22
#define TAP_DANCE 23
#define BALLET_SPIN 24
#define SKATE_FORWARD 25
#define DUCK 26
#define KICK_FORWARD 27
#define MARCH_FORWARD 28
#define SIDE_WALK 29
#define SNEAK_FORWARD 30
#define SPLIT_FORWARD 31
#define SIT_FORWARD 32
#define SIDE_JUMP 33
#define BRIDGE 34

#define MOTION_COUNT 28
#define WALKING 0
#define WALKING_BACKWARDS 1
#define RUNNING_FAST 2
#define JUMPING_SHORT 3
#define TIPTOEING 4
#define TIPTOEING_BACKWARDS 5
#define PINO_WALKING 6
#define PINO_WALKING_BACKWARDS 7
#define LIMPING 8
#define LIMPING_BACKWARDS 9
#define JUMPING_LONG 10
#define JOGGING 11
#define RUNNING_NORMAL 12
#define CATWALK_NORMAL 13
#define MOONWALK 14
#define ONE_HOP 15
#define TAP 16
#define SPIN 17
#define SKATING 18
#define DUCKING 19
#define KICKING 20
#define MARCHING 21
#define SIDE_WALKING 22
#define SNEAKING 23
#define SPLITTING 24
#define SITTING 25
#define SIDE_JUMPING 26
#define BRIDGING 27

class Animation : public Drawable
{
public:
    void GetRootPosition(double *x, double *y, double *z);
	Animation(FootDrawable *left, FootDrawable *right);
	~Animation();
    bool DoUpdateStep(int motionType, double stepLengthRatio, double stepDurationRatio);
	//added by david
	void rotate_character(double angle);
	bool Update(int motionType, double stepLengthRatio, double stepDurationRatio);
    vector<VectorR3> targets_;
private:
    double rootHeight[MODEL_COUNT];
    /*float          kRed[] = { 1.00f, 0.00f, 0.00f, 1.00f };
    float         kBlue[] = { 0.00f, 0.00f, 1.00f, 1.00f };
    float        kGreen[] = { 0.00f, 1.00f, 0.00f, 1.00f };
    float         kCyan[] = { 0.00f, 1.00f, 1.00f, 1.00f };
    float       kYellow[] = { 1.00f, 1.00f, 0.00f, 1.00f };
    float        kWhite[] = { 1.00f, 1.00f, 1.00f, 1.00f };
    float         kGray[] = { 0.50f, 0.50f, 0.50f, 1.00f };
    float kNightSkyBlue[] = { 0.12f, 0.56f, 1.00f, 1.00f };*/

    enum Method { TRANSPOSE, PSEUDOINVERSE, DLS, SDLS };

    int step_;
	double character_orientation;

    vector<vector<VectorR3>> jointTrajectories_[MODEL_COUNT];
    vector<vector<Quaternion2>> jointQuaternions_[MODEL_COUNT];

	FootDrawable *left_;
	FootDrawable *right_;
    Jacobian *jacobian_;
    int useJacobianTargets_;
    int method_;

	Tree tree_;
	std::vector<Node> node_;

    static const float kScale;
    static const float kScaleX;
    static const float kTranslateX;
    static const float kTranslateZ;

	std::vector<std::string> markers_;
	std::vector<std::vector<VectorR3>> trajectory_; // feet positions
	std::vector<std::vector<VectorR3>> jointTrajectory_;
    std::vector<std::vector<VectorR3>> learnedTargets_;

    vector<vector<Quaternion2>> currentMotion_;
    vector<vector<Quaternion2>> oldMotion_;

	Eigen::MatrixXf B_;
	Eigen::MatrixXf X_;
	Eigen::MatrixXf Z_;
	Eigen::VectorXf M_; // foot trajectory
	Eigen::VectorXf N_; // joint angles
	int n_;
	int k_;
	int m_;

    void ForceNoWristRotation(vector<vector<Quaternion2>> *jointQuaternion);
    void ScaleJumpingHeight(double ratio, vector<vector<Quaternion2>> *jointQuaternion, int begin, int end);
    void SimpleRetarget(double rootHeightTarget, double rootHeightSource, vector<vector<Quaternion2>> *jointQuaternion);
    double GetRootHeight();
    void ConvertAbsoluteToRelative(vector<vector<Quaternion2>> *jointQuaternion);
    void ConvertRelativetoAbsolute(vector<vector<Quaternion2>> *jointQuaternion);
    vector<vector<Quaternion2>> SpeedUp(double ratio, vector<vector<Quaternion2>> &source);
    vector<vector<Quaternion2>> GetTransition(int firstBegin, int firstEnd,
        vector<vector<Quaternion2>> &firstMotion, int secondBegin, int secondEnd,
        vector<vector<Quaternion2>> &secondMotion);
    void InterpolateMotions(vector<vector<Quaternion2>> *firstMotion,
        vector<vector<Quaternion2>> *secondMotion,
        vector<vector<Quaternion2>> *currentMotion, double t);
    vector<vector<Quaternion2>> MakeLoop(vector<vector<Quaternion2>> &motion, int frameCount, int begin, int end);
    void ConvertFromRadianToDegree(vector<vector<VectorR3>> *jointTrajectory);
    void ConvertFromDegreeToRadian(vector<vector<VectorR3>> *jointTrajectory);
    void TranslateToOrigin(vector<vector<VectorR3>> *jointTrajectory);
    Quaternion2 ConvertEulerAnglesToQuaternion(const VectorR3 &v);
    void Downsample(const vector<vector<Quaternion2>> &first, vector<vector<Quaternion2>> *second);
    void RotateFrameBtoOriginal(vector<vector<VectorR3>> *jointTrajectory, vector<vector<Quaternion2>> *jointQuaternion);
    void ConvertJointAnglesToQuaternion(const vector<vector<VectorR3>> &source, vector<vector<Quaternion2>> &target);

    void ConvertTrainingDatatoWilliam();
    void ConvertFootDataBacktoMine();

    void BuildSkeletonFromBVH(const char *fileName);
	void BuildSkeleton(float size);
	void RunTest();

	void BuildVectorM();
	void BuildVectorN();
	void BuildMatrixZ();
	void BuildMatrixX();
	void BuildMatrixB();

	void AssignIndex();
	void ReadMarkers(std::vector<std::vector<VectorR3>> &trajectory, const char *fileName);
	void TransformMarkers(std::vector<std::vector<VectorR3>> &trajectory, VectorR3 &scale, VectorR3 &translate);
	void PrintMarkers(std::vector<std::vector<VectorR3>> &trajectory, char *fileName);
	void ReadBVHFile(vector<vector<VectorR3>> *trajectory, const char *fileName);
	void ProcessEulerAngles(vector<vector<VectorR3>> *trajectory);

	void draw();
    void Reset();
    void RebuildJacobian();
    void InitStates();
    void DrawAxes(float size);

	
};

#endif