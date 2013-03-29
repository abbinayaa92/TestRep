#ifndef MAIN_H_
#define MAIN_H_

#include <map>
#include <string>
#include <vector>
#include <Eigen/dense>
#include <GL/freeglut.h>
#include "Node.h"
#include "Tree.h"
#include "Jacobian.h"
#include <GL/glui.h>
#ifdef IKINEMA
#include "IKSolver.h"
#endif

#define RADIAN(X)	((X)*RadiansToDegrees)

const char *WINDOWTITLE = { "Kinematics -- Sam Buss and Jin-Su Kim" };
const char *GLUITITLE   = { "User Interface Window" };

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

#define ROTATE		0
#define SCALE		1

const float MINSCALE = { 0.05f };

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

const int ORTHO = { false };
const int PERSP = { true  };

enum Method { TRANSPOSE, PSEUDOINVERSE, DLS, SDLS };

#define PAUSE    0
#define QUIT		1
#define RUNTEST		2
#define PREV        3
#define NEXT        4
#define FIX         5
#define PAUSE       6

const float          kRed[] = { 1.00f, 0.00f, 0.00f, 1.00f };
const float         kBlue[] = { 0.00f, 0.00f, 1.00f, 1.00f };
const float        kGreen[] = { 0.00f, 1.00f, 0.00f, 1.00f };
const float         kCyan[] = { 0.00f, 1.00f, 1.00f, 1.00f };
const float       kYellow[] = { 1.00f, 1.00f, 0.00f, 1.00f };
const float        kWhite[] = { 1.00f, 1.00f, 1.00f, 1.00f };
const float         kGray[] = { 0.50f, 0.50f, 0.50f, 1.00f };
const float kNightSkyBlue[] = { 0.12f, 0.56f, 1.00f, 1.00f };

const float  globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
const float  light0Ambient[] = { 0.2f , 0.2f, 0.2f , 1.0f };
const float  light0Diffuse[] = { 0.8f , 0.8f, 0.8f , 1.0f };
const float light0Specular[] = { 1.0f , 1.0f, 1.0f , 1.0f };
const float light0Position[] = { 1.0f , 1.0f, 1.0f , 0.0f };
const float light1Position[] = { -1.0f, 1.0f, -1.0f, 0.0f };

const double kNoRotation[3] = { 0.0, 0.0, 0.0 };
const VectorR3 kRotationAxes[3] = { VectorR3::UnitX, VectorR3::UnitY, VectorR3::UnitZ };
const VectorR3 kUnitzzz[3] = { VectorR3::UnitZ, VectorR3::UnitZ, VectorR3::UnitZ };
const VectorR3 kUnitxxx[3] = { VectorR3::UnitX, VectorR3::UnitX, VectorR3::UnitX };
const VectorR3 kUnitxzz[3] = { VectorR3::UnitX, VectorR3::UnitZ, VectorR3::UnitZ };
const VectorR3 kUnityyy[3] = { VectorR3::UnitY, VectorR3::UnitY, VectorR3::UnitY };

const float BACKCOLOR[] = { 0., 0., 0., 0. };

const float AXES_COLOR[] = { 1., .5, 0. };
const float AXES_WIDTH   = { 3. };

int	ActiveButton;		/* current button that is down		*/
GLUI *Glui;			/* instance of glui window		*/
int	GluiWindow;		/* the glut id for the glui window	*/
int	GrWindow;		/* window id for graphics window	*/
int	LeftButton;		/* either ROTATE or SCALE		*/
float RotMatrix[4][4];	/* set by glui rotation widget		*/
float Scale, Scale2;		/* scaling factors			*/
int method_;
int test_;
int	Xmouse, Ymouse;		/* mouse values				*/
float Xrot, Yrot;		/* rotation angles in degrees		*/
float TransXYZ[3];		/* set by glui translation widgets	*/
int useJacobianTargets_;
bool isPaused_;

void RunTest19();
void UpdateTargets19(double t);
void BuildSkeletonFromBVH(const char *bvhFile);
void DrawCylinder(VectorR3 axis, float size);
void ProcessEulerAngles(vector<vector<VectorR3>> &trajectory);
void CreateTasks21();
void RunTest17();
void UpdateTargets17(double t);
void UpdateTargets16(double t);
void RunTest16();
void BuildTree7();
void UpdateTargets15(double t);
void RunTest15();
void RunTest14();
void UpdateTargets14(double t);
void Solve();
void SetTargets();
void LoadSkeleton(const char *strFileSkeleton);
void LoadLimit(const char *strFileLimits);
#ifdef IKINEMA
void DrawSkeleton(FIK::IKSolver &solver);
#endif
void ConvertEToQ(float xRot, float yRot, float zRot, float *w, float *x, float *y, float *z);
void HelloIKinema(const char* strFileSkeleton, const char *strFileLimits,
                  const char* strFileResultTutorial1);
void RunTest18();
void UpdateTargets18(double t);
void BuildVectorM();
void BuildVectorN();
void UpdateTargets13(double t);
void RunTest13();
void BuildTree6();
void BuildMatrixZ();
void BuildMatrixX();
void BuildMatrixB();
void RunTest11();
void UpdateTargets11(double t);
void InitStates();
void BuildTree5();
void BuildTree4();
void BuildTree3();
void BuildTree2();
void BuildTree1();
void RebuildJacobian();
void Animate( void );
void Axes( float length );
void Buttons( int );
void GlutDisplay();
void GlutIdle();
void InitGlui();
void InitGraphics();
void GlutKeyboard(unsigned char, int, int);
void GlutMouse(int, int, int, int);
void GlutMotion(int, int);
void GlutReshape(int, int);
void DrawAxes(float size);
void DrawGrid(float size, float step, float y);
void Reset( void );
void RunTest(int test);
void RunTest0();
void RunTest1();
void DoUpdateStep();
void Pause();
void DrawTargets();
void UpdateTargets0(double t);
void UpdateTargets1(double t);
void RunTest2();
void RunTest3();
void UpdateTargets2(double t);
void UpdateTargets3(double t);
void TogglePause();
void RenderString(float x, float y, void *font, const char *string, const VectorR3& rgb);
void SelectNode(int id);
void FixNode(int id);
void RunTest4();
void UpdateTargets4(double t);
void RunTest5();
void UpdateTargets5(double t);
void RunTest6();
void UpdateTargets6(double t);
void RunTest7();
void UpdateTargets7(double t);
void RunTest8();
void UpdateTargets8(double t);
void RunTest9();
void UpdateTargets9(double t);
void RunTest10();
void UpdateTargets10(double t);
void BuildTree0();
void RunTest12();
void UpdateTargets12(double t);
void AssignIndex();

void NextStep();
void ConvertQToE(float w, float x, float y, float z, float *h, float *p, float *b);
void CreateTasks();

#define MAX_NUM_NODE	1000
#define MAX_NUM_THETA	1000
#define MAX_NUM_EFFECT	100

Tree tree_;
Jacobian *jacobian_;
vector<VectorR3> targets_;
vector<Node> node_;
int currentNode_;
vector<vector<VectorR3>> trajectory_;
vector<vector<VectorR3>> jointTrajectory_;
vector<string> markers_;
map<string, int> makersNameToInt_;

Eigen::MatrixXf B_;
Eigen::MatrixXf X_;
Eigen::MatrixXf Z_;
Eigen::VectorXf M_; // foot trajectory
Eigen::VectorXf N_; // joint angles
int n_;
int k_;
int m_;

#ifdef IKINEMA
FIK::IKPositionTask *leftHandTask_;
FIK::IKPositionTask *rightHandTask_;
FIK::IKPositionTask *headTask_;
FIK::IKPositionTask *leftFootTask_;
FIK::IKPositionTask *rightFootTask_;
FIK::IKSolver *g_pIKinemaSolver = 0;
#endif

void (*UpdateTargets)(double t);
VectorR3 focusPoint_;

#endif    // MAIN_H_