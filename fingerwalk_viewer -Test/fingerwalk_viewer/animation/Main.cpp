//// TODO: support loading skeleton from file
//// TODO: support changing end effector on the fly
//
//#include <cstdio>
//#include <cstdlib>
//#include <cmath>
//#include <cassert>
//#include <cctype>
//#include <iostream>
//#include <fstream>
//#include <list>
//#include <string>
//#include <sstream>
//#include <algorithm>
//#include <iomanip>
//#include <tuple>
//#include <GL/glut.h>
//#include <GL/glui.h>
//
//#include "Main.h"
//
//using namespace std;
//using namespace Eigen;
//using namespace FIK;
//
//const int SlowdownFactor = 1;
//const int SleepsPerStep = 0;
//int SleepCounter = 0;
//const double Tstep = 0.002 * (double) SlowdownFactor;
//double T = -Tstep;
//int step = 0;
//
///*
// * Main
// */
///*
//int main( int argc, char *argv[] )
//{
//    glutInit(&argc, argv);
//    InitStates();
//    InitGraphics();
//
//    test_ = 14;
//    Reset();
//    InitGlui();
//    RunTest(test_);
//
//    glutMainLoop();
//
//    float xRot, yRot, zRot;
//    float w, x, y, z;
//    cout << "Convert euler angles to quaternion: " << endl;
//    cout << "xRot = "; cin >> xRot;
//    cout << "yRot = "; cin >> yRot;
//    cout << "zRot = "; cin >> zRot;
//    ConvertEToQ(xRot * DegreesToRadians,
//                yRot * DegreesToRadians,
//                zRot * DegreesToRadians,
//                &w, &x, &y, &z);
//    cout << x << " " << y << " " << z << " " << w << endl;
//
//    return 0;
//}
//*/
//
///*
// * Handles the control
// */
//void Buttons(int id)
//{
//    switch (id) {
//    case PAUSE:
//        isPaused_ = !isPaused_;
//        break;
//    case QUIT:
//        Glui->close();
//        glFinish();
//        glutDestroyWindow(GrWindow);
//        exit(EXIT_SUCCESS);
//    case RUNTEST:
//        Reset();
//        RunTest(test_);
//        break;
//    }
//    Glui->sync_live();
//}
//
///*
// * Draws a grid on the floor
// */
//void DrawGrid(float size, float step, float y)
//{
//    glDisable(GL_LIGHTING);
//
//    glBegin(GL_LINES);
//        glColor3f(0.3f, 0.3f, 0.3f);
//        for(float i = 0.0f; i <= size; i += step) {
//            glVertex3f(-size, y,  i);   // lines parallel to X-axis
//            glVertex3f( size, y,  i);
//            glVertex3f(-size, y, -i);   // lines parallel to X-axis
//            glVertex3f( size, y, -i);
//
//            glVertex3f( i, y, -size);   // lines parallel to Z-axis
//            glVertex3f( i, y,  size);
//            glVertex3f(-i, y, -size);   // lines parallel to Z-axis
//            glVertex3f(-i, y,  size);
//        }
//    glEnd();
//
//    glEnable(GL_LIGHTING);
//}
//
///*
// * Draws the targets
// */
//void DrawTargets()
//{
//    glColor3fv(kRed);
//
//    for (int i = 0; i < targets_.size(); i++) {
//        glPushMatrix();
//        glTranslatef(targets_[i].x, targets_[i].y, targets_[i].z);
//        glutSolidSphere(0.08, 12, 12);
//        glPopMatrix();
//    }
//
//    glColor3fv(kBlue);
//}
//
///*
// * Glut's idle callback
// */
//void GlutIdle()
//{
//    glutSetWindow(GrWindow);
//    glutPostRedisplay();
//}
//
///*
// * Glut's display callback
// */
//void GlutDisplay()
//{
//    if (!isPaused_) {
//        if (test_ != 18) // TODO: ...
//            DoUpdateStep();
//        else
//            UpdateTargets(0);
//        //isPaused_ = true;
//    }
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);
//
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//
//    gluLookAt(0.0f, 2.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
//
//    glRotatef(Xrot, 1.0, 0.0, 0.0);
//    glRotatef(Yrot, 0.0, 1.0, 0.0);
//
//    glScalef(Scale, Scale, Scale);
//
//    DrawGrid(20.0f, 1.0f, -1.92f);
//    DrawAxes(10.0f);
//    DrawTargets();
//
//    #ifdef IKINEMA
//    if (test_ == 18)
//        DrawSkeleton(*g_pIKinemaSolver);
//    #endif
//
//    if (test_ != 18) {
//        tree_.Draw();
//        tree_.DrawDebug(targets_);
//    }
//
//    glutSwapBuffers();
//}
//
///*
// * Creates the control panel
// */
//void InitGlui(void)
//{
//    GLUI_Panel *panel;
//    GLUI_RadioGroup *group;
//
//    Glui = GLUI_Master.create_glui((char *) GLUITITLE, 0, 0, 0);
//
//    Glui->add_statictext((char *) GLUITITLE);
//    Glui->add_separator();
//
//    // Tests
//    panel = Glui->add_panel("Tests");
//        group = Glui->add_radiogroup_to_panel(panel, &test_, 0, NULL);
//            Glui->add_radiobutton_to_group(group, "0 . All ball joints");
//            Glui->add_radiobutton_to_group(group, "1 . Internal end effectors");
//            Glui->add_radiobutton_to_group(group, "2 . Universal and hinge joints");
//            Glui->add_radiobutton_to_group(group, "3 . Y shape, multiple joint types");
//            Glui->add_radiobutton_to_group(group, "4 . 3 joints, 2 links, movable root");
//            Glui->add_radiobutton_to_group(group, "5 . Lower body swaying");
//            Glui->add_radiobutton_to_group(group, "6 . Lower body walking (sine-wave)");
//            Glui->add_radiobutton_to_group(group, "7 . Swaying in place");
//            Glui->add_radiobutton_to_group(group, "8 . Walking (numerical IK)");
//            Glui->add_radiobutton_to_group(group, "9 . Backward walking (numerical IK)");
//            Glui->add_radiobutton_to_group(group, "10. Running (numerical IK)");
//            Glui->add_radiobutton_to_group(group, "11. Jumping (numerical IK)");
//            Glui->add_radiobutton_to_group(group, "12. Backward walking (BVH)");
//            Glui->add_radiobutton_to_group(group, "13. Backward walking (linear least square)");
//            Glui->add_radiobutton_to_group(group, "14. Forward walking (linear least square)");
//            Glui->add_radiobutton_to_group(group, "15. Inplace walking (numerical IK, 5 targets)");
//            Glui->add_radiobutton_to_group(group, "16. Inplace walking (numerical IK, 9 targets)");
//            Glui->add_radiobutton_to_group(group, "17. Running (linear least square)");
//            #ifdef IKINEMA
//            Glui->add_radiobutton_to_group(group, "18. Inplace walking (IKinema, 5 targets)");
//            #endif
//
//    // Methods
//    panel = Glui->add_panel("Methods");
//        group = Glui->add_radiogroup_to_panel(panel, &method_, 0, NULL);
//            Glui->add_radiobutton_to_group(group, "Jacobian Transpose");
//            Glui->add_radiobutton_to_group(group, "Pure Pseudoinverse");
//            Glui->add_radiobutton_to_group(group, "Damped Least Squares");
//            Glui->add_radiobutton_to_group(group, "Selectively Damped Least Squares");
//
//    // Other options
//    panel = Glui->add_panel("Options");
//        Glui->add_checkbox_to_panel(panel, "Jacobian Targets",
//                                    &useJacobianTargets_, 0, NULL);
//
//    // Select node
//    panel = Glui->add_panel("Select Node");
//    Glui->add_button_to_panel(panel, "<---", PREV, (GLUI_Update_CB) SelectNode);
//    Glui->add_button_to_panel(panel, "--->", NEXT, (GLUI_Update_CB) SelectNode);
//
//    // Control
//    panel = Glui->add_panel("Control");
//    Glui->add_button_to_panel(panel, "Run Test", RUNTEST, (GLUI_Update_CB) Buttons);
//    Glui->add_column_to_panel(panel, FALSE);
//    Glui->add_button_to_panel(panel, "Toggle Pause", PAUSE, (GLUI_Update_CB) Buttons);
//    Glui->add_button_to_panel(panel, "Quit", QUIT, (GLUI_Update_CB) Buttons);
//    Glui->set_main_gfx_window(GrWindow);
//}
//
///*
// * Renders a text string on the screen
// */
//void RenderString(float x, float y, void *font, const char *string,
//                  const VectorR3& rgb)
//{
//    glDisable(GL_LIGHTING);
//    glColor3f(rgb.x, rgb.y, rgb.z);
//    glRasterPos2f(x, y);
//
//    glutBitmapString(font, (const unsigned char*) string);
//    glEnable(GL_LIGHTING);
//}
//
///*
// * Draw the 3 axes
// */
//void DrawAxes(float size)
//{
//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_LIGHTING);
//
//    glLineWidth(3.0f);
//    glBegin(GL_LINES);
//        glColor3fv(kRed);
//        glVertex3f(0.0f, 0.0f, 0.0f);
//        glVertex3f(size, 0.0f, 0.0f);
//        glColor3fv(kGreen);
//        glVertex3f(0.0f, 0.0f, 0.0f);
//        glVertex3f(0.0f, size, 0.0f);
//        glColor3fv(kBlue);
//        glVertex3f(0.0f, 0.0f, 0.0f);
//        glVertex3f(0.0f, 0.0f, size);
//    glEnd();
//    glLineWidth(1.0f);
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//}
//
///*
// * Selects a node using its id
// * Currently useless
// */
//void SelectNode(int id)
//{
//    node_[currentNode_].Unselect();
//
//    if (id == PREV)
//        --currentNode_;
//    else if (id == NEXT)
//        ++currentNode_;
//
//    if (currentNode_ < 0)
//        currentNode_ = node_.size() - 1;
//    else if (currentNode_ >= node_.size())
//        currentNode_ = 0;
//
//    node_[currentNode_].Select();
//}
//
///*
// * Initializes some OpenGL states
// */
//void InitGraphics()
//{
//    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
//    glutInitWindowSize(800, 800);
//    glutInitWindowPosition(300, 100);
//    GrWindow = glutCreateWindow("IK");
//
//    glutDisplayFunc(GlutDisplay);
//    glutIdleFunc(GlutIdle);
//    glutMouseFunc(GlutMouse);
//    glutMotionFunc(GlutMotion);
//    glutKeyboardFunc(GlutKeyboard);
//    glutReshapeFunc(GlutReshape);
//
//    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
//
//    glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);
//    glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
//
//    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kWhite);
//    //glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 96);
//
//    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//
//    glEnable(GL_COLOR_MATERIAL);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//
//    glShadeModel(GL_SMOOTH);
//}
//
///*
// * Glut's reshape callback
// */
//void GlutReshape (int w, int h)
//{
//    glViewport(0, 0, w, h);
//
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(75.0f, 1.0f, 0.1f, 1000.0f);
//    glMatrixMode(GL_MODELVIEW);
//}
//
///*
// * Glut's mouse callback
// */
//void GlutMouse(int button, int state, int x, int y)
//{
//    int b;
//
//    switch (button) {
//    case GLUT_LEFT_BUTTON:
//        b = LEFT;
//        break;
//    case GLUT_MIDDLE_BUTTON:
//        b = MIDDLE;
//        break;
//    case GLUT_RIGHT_BUTTON:
//        b = RIGHT;
//        break;
//    default:
//        b = 0;
//        cerr << "Unknown mouse button: " << button << "\n";
//    }
//
//    if (state == GLUT_DOWN) {
//        Xmouse = x;
//        Ymouse = y;
//        ActiveButton |= b;
//    } else {
//        ActiveButton &= ~b;
//    }
//}
//
///*
// * Glut's motion callback
// */
//void GlutMotion(int x, int y)
//{
//    int dx = x - Xmouse;
//    int dy = y - Ymouse;
//
//    if (ActiveButton & LEFT) {
//        Xrot += (ANGFACT * dy);
//        Yrot += (ANGFACT * dx);
//    }
//
//    if (ActiveButton & MIDDLE) {
//        Scale += SCLFACT * (float) (dx - dy);
//        if (Scale < MINSCALE)
//            Scale = MINSCALE;
//    }
//
//    Xmouse = x;
//    Ymouse = y;
//}
//
///*
// * Glut's keyboard callback
// */
//void GlutKeyboard(unsigned char c, int x, int y)
//{
//    Glui->sync_live();
//    glutSetWindow(GrWindow);
//    glutPostRedisplay();
//}
//
///*
// * Reset the data before running each test
// */
//void Reset()
//{
//    step = 0;
//    T = -Tstep;
//    trajectory_.clear();
//    jointTrajectory_.clear();
//    isPaused_ = false;
//}
//
///*
// * Initializes some data
// */
//void InitStates()
//{
//    ActiveButton = 0;
//    LeftButton = ROTATE;
//    Scale  = 1.0;
//    Xrot = Yrot = 0.0;
//    method_ = DLS;
//    useJacobianTargets_ = false;
//}
//
///*
// * Updates the targets and recalculates the Jacobian
// */
//void DoUpdateStep() {
//    if (SleepCounter == 0) {
//        T += Tstep;
//        UpdateTargets(T);
//    }
//
//    if (targets_.empty())
//        return;
//
//    if (useJacobianTargets_)
//        jacobian_->SetJtargetActive();
//    else
//        jacobian_->SetJendActive();
//
//    jacobian_->ComputeJacobian();
//
//    switch (method_) {
//        case TRANSPOSE:
//            jacobian_->CalcDeltaThetasTranspose();
//            break;
//        case DLS:
//            jacobian_->CalcDeltaThetasDLS();
//            break;
//        case PSEUDOINVERSE:
//            jacobian_->CalcDeltaThetasPseudoinverse();
//            break;
//        case SDLS:
//            jacobian_->CalcDeltaThetasSDLS();
//            break;
//        default:
//            jacobian_->ZeroDeltaThetas();
//            break;
//    }
//
//    if (SleepCounter == 0) {
//        jacobian_->UpdateThetas();
//        jacobian_->UpdatedSClampValue();
//        SleepCounter = SleepsPerStep;
//    } else {
//        SleepCounter--;
//    }
//}
//
///*
// * Runs the given test
// */
//void RunTest(int test)
//{
//    switch (test) {
//    case 0:
//        RunTest0();
//        break;
//    case 1:
//        RunTest1();
//        break;
//    case 2:
//        RunTest2();
//        break;
//    case 3:
//        RunTest3();
//        break;
//    case 4:
//        RunTest4();
//        break;
//    case 5:
//        RunTest5();
//        break;
//    case 6:
//        RunTest6();
//        break;
//    case 7:
//        RunTest7();
//        break;
//    case 8:
//        RunTest8();
//        break;
//    case 9:
//        RunTest9();
//        break;
//    case 10:
//        RunTest10();
//        break;
//    case 11:
//        RunTest11();
//        break;
//    case 12:
//        RunTest12();
//        break;
//    case 13:
//        RunTest13();
//        break;
//    case 14:
//        RunTest14();
//        break;
//    case 15:
//        RunTest15();
//        break;
//    case 16:
//        RunTest16();
//        break;
//    case 17:
//        RunTest17();
//        break;
//#ifdef IKINEMA
//    case 18:
//        RunTest18();
//        break;
//#endif
//    default:
//        assert(false);
//    }
//}
//
///*
// * Recomputes the Jacobian
// */
//void RebuildJacobian()
//{
//    delete jacobian_;
//    jacobian_ = new Jacobian(&tree_);
//    jacobian_->Reset();
//    jacobian_->ComputeJacobian();
//}
//
///*
// * Y shape, all ball joints
// */
//void BuildTree0()
//{
//    tree_.Reset();
//    node_.resize(9);
//
//    node_[0] = Node(3, VectorR3(0.0f, -0.5f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRoot(&node_[0], true);
//    node_[1] = Node(3, VectorR3(0.0f, 0.9f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(3, VectorR3(0.0f, 0.9f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRightSibling(&node_[1], &node_[2]);
//    node_[3] = Node(3, VectorR3(0.5f, 0.6f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[1], &node_[3]);
//    node_[4] = Node(3, VectorR3(-0.5f, 0.6f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[2], &node_[4]);
//    node_[5] = Node(3, VectorR3(0.2f, 0.3f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[3], &node_[5]);
//    node_[6] = Node(3, VectorR3(-0.3f, 0.5f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[4], &node_[6]);
//    node_[7] = Node(3, VectorR3(0.0f, 0.7f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[5], &node_[7]);
//    node_[8] = Node(3, VectorR3(0.0f, 0.4f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[6], &node_[8]);
//}
//
///*
// * Y shape, all ball joints, 2 effectors
// */
//void RunTest0()
//{
//    BuildTree0();
//    tree_.MakeEffector(&node_[7]);
//    tree_.MakeEffector(&node_[8]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    node_[currentNode_].Select();
//
//    targets_.resize(2);
//    UpdateTargets = UpdateTargets0;
//
//    RebuildJacobian();
//}
//
///*
// * 2 targets
// */
//void UpdateTargets0(double t) {
//    targets_[0].Set( 0.6f + 1.5f * sin(6.0f * t),
//                     1.2f + 0.2f * sin(7.0f * t),
//                     0.1f + 0.2f * sin(8.0f * t));
//    targets_[1].Set(-0.7f + 0.4f * sin(4.0f * t),
//                     1.8f + 0.3f * sin(4.0f * t),
//                     0.1f + 0.2f * sin(3.0f * t));
//}
//
///*
// * Y shape, 3 effectors, one of which is also a joint
// */
//void RunTest1()
//{
//    BuildTree0();
//    tree_.MakeJointAndEffector(&node_[3]);
//    tree_.MakeEffector(&node_[7]);
//    tree_.MakeEffector(&node_[8]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(3);
//    UpdateTargets = UpdateTargets1;
//
//    RebuildJacobian();
//}
//
///*
// * 3 targets
// */
//void UpdateTargets1(double t)
//{
//    targets_[0].Set( 0.7f + 0.4f * sin( 6.0f * t),
//                     1.2f + 0.4f * sin( 8.0f * t),
//                     0.2f + 0.6f * sin(10.0f * t));
//    targets_[1].Set( 0.7f + 0.4f * sin( 6.0f * t),
//                     2.0f + 0.6f * sin( 8.0f * t),
//                     0.2f + 0.8f * sin(10.0f * t));
//    targets_[2].Set(-1.0f + 0.4f * sin( 6.0f * t),
//                     2.1f + 0.6f * sin( 8.0f * t),
//                     0.2f + 0.8f * sin(10.0f * t));
//}
//
///*
// * One universal and one hinge joints
// */
//void BuildTree1()
//{
//    tree_.Reset();
//    node_.resize(4);
//
//    node_[0] = Node(1, VectorR3(0.0f, 0.4f, 0.0f), kUnitzzz, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRoot(&node_[0], true);
//    node_[1] = Node(2, VectorR3(0.0f, 0.5f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(1, VectorR3(0.0f, 0.4f, 0.0f), kUnitxxx, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[1], &node_[2]);
//    node_[3] = Node(1, VectorR3(0.0f, 0.7f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, EFFECTOR);
//    tree_.InsertChild(&node_[2], &node_[3]);
//}
//
///*
// * One universal and one hinge joints, 1 effector
// */
//void RunTest2()
//{
//    BuildTree1();
//    tree_.MakeEffector(&node_[3]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(1);
//    UpdateTargets = UpdateTargets2;
//
//    RebuildJacobian();
//}
//
///*
// * 1 target
// */
//void UpdateTargets2(double t)
//{
//    targets_[0].Set(-0.7f + 0.7f * sin( 6.0f * t),
//                     1.3f + 0.2f * sin( 8.0f * t),
//                    -0.5f + 0.2f * sin(10.0f * t));
//}
//
///*
// * Y shape, multiple joint types
// */
//void BuildTree2()
//{
//    tree_.Reset();
//    node_.resize(9);
//
//    node_[0] = Node(3, VectorR3(0.0f, -0.5f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRoot(&node_[0], true);
//    node_[1] = Node(1, VectorR3(0.0f, 0.9f, 0.0f), kUnitxxx, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[3] = Node(2, VectorR3(0.0f, 0.6f, 0.0f), kUnitxzz, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[1], &node_[3]);
//    node_[2] = Node(1, VectorR3(0.0f, 0.9f, 0.0f), kUnitzzz, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRightSibling(&node_[1], &node_[2]);
//    node_[4] = Node(2, VectorR3(0.0f, 0.6f, 0.0f), kUnitxzz, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[2], &node_[4]);
//    node_[5] = Node(1, VectorR3(0.0f, 0.3f, 0.0f), kUnitxxx, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[3], &node_[5]);
//    node_[6] = Node(1, VectorR3(0.0f, 0.5f, 0.0f), kUnitzzz, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[4], &node_[6]);
//    node_[7] = Node(1, VectorR3(0.0f, 0.7f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[5], &node_[7]);
//    node_[8] = Node(1, VectorR3(0.0f, 0.4f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[6], &node_[8]);
//}
//
///*
// * Y shape, multiple joint types
// */
//void RunTest3()
//{
//    BuildTree2();
//    tree_.MakeEffector(&node_[7]);
//    tree_.MakeEffector(&node_[8]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(2);
//    UpdateTargets = UpdateTargets3;
//
//    RebuildJacobian();
//}
//
///*
// * 3 targets
// */
//void UpdateTargets3(double t)
//{
//    targets_[0].Set( 2.0f + 0.6f * sin( 6.0f * t),
//                     1.2f + 0.6f * sin( 8.0f * t),
//                     0.3f + 0.7f * sin(10.0f * t));
//    targets_[1].Set(-0.7f + 0.7f * sin( 6.0f * t),
//                     0.8f + 0.5f * sin( 8.0f * t),
//                    -0.2f + 0.6f * sin(10.0f * t));
//}
//
///*
// * 3 joints, 2 links, movable root
// */
//void BuildTree3()
//{
//    tree_.Reset();
//    node_.resize(3);
//
//    node_[0] = Node(3, VectorR3(0.0f, -0.5f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRoot(&node_[0], false);
//    node_[1] = Node(1, VectorR3(0.0f, 0.5f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(3, VectorR3(0.0f, 1.0f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[1], &node_[2]);
//}
//
///*
// * 3 joints, 2 links, movable root
// */
//void RunTest4()
//{
//    BuildTree3();
//    tree_.MakeJointAndEffector(&node_[1]);
//    tree_.MakeEffector(&node_[2]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(2);
//    UpdateTargets = UpdateTargets4;
//
//    RebuildJacobian();
//}
//
///*
// * 2 targets
// */
//void UpdateTargets4(double t)
//{
//    targets_[0].Set(0.0f, 0.0f, 0.0f);
//    targets_[1].Set(0.0f + 0.5f * sin(5.0f * t),
//                    1.0f + 0.5f * sin(7.0f * t),
//                    0.0f + 0.5f * sin(9.0f * t));
//}
//
///*
// * Lower body
// */
//void BuildTree4()
//{
//    tree_.Reset();
//    node_.resize(9);
//
//    node_[0] = Node(3, VectorR3(0.0f, 1.5f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRoot(&node_[0], false);
//    node_[1] = Node(3, VectorR3(-0.3f, -0.3f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(3, VectorR3(0.3f, -0.3f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertRightSibling(&node_[1], &node_[2]);
//    node_[3] = Node(3, VectorR3(0.0f, -0.6f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[1], &node_[3]);
//    node_[4] = Node(3, VectorR3(0.0f, -0.6f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[2], &node_[4]);
//    node_[5] = Node(3, VectorR3(0.0f, -0.6f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[3], &node_[5]);
//    node_[6] = Node(3, VectorR3(0.0f, -0.6f, 0.0f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[4], &node_[6]);
//    node_[7] = Node(3, VectorR3(0.0f, 0.0f, 0.3f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[5], &node_[7]);
//    node_[8] = Node(3, VectorR3(0.0f, 0.0f, 0.3f), kRotationAxes, kNoRotation,
//                    0.08, JOINT);
//    tree_.InsertChild(&node_[6], &node_[8]);
//}
//
///*
// * Lower body swaying
// */
//void RunTest5()
//{
//    BuildTree4();
//    tree_.MakeJointAndEffector(&node_[0]);
//    tree_.MakeJointAndEffector(&node_[5]);
//    tree_.MakeJointAndEffector(&node_[6]);
//    tree_.MakeEffector(&node_[7]);
//    tree_.MakeEffector(&node_[8]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(5);
//    UpdateTargets = UpdateTargets5;
//
//    RebuildJacobian();
//}
//
///*
// * 5 targets
// */
//void UpdateTargets5(double t)
//{
//    targets_[0].Set(0.0f + 0.4f * sin(12.0f * t),
//                    1.5f,
//                    0.0f + 0.4f * cos(12.0f * t));
//    targets_[1].Set(-0.3f, 0.0f, 0.0f);
//    targets_[2].Set( 0.3f, 0.0f, 0.0f);
//    targets_[3].Set(-0.3f, 0.0f, 0.3f);
//    targets_[4].Set( 0.3f, 0.0f, 0.3f);
//}
//
///*
// * Lower body "walking" using a sine-wave trajectory
// */
//void RunTest6()
//{
//    BuildTree4();
//    tree_.MakeEffector(&node_[7]);
//    tree_.MakeEffector(&node_[8]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(2);
//    UpdateTargets = UpdateTargets6;
//
//    RebuildJacobian();
//}
//
///*
// * 2 targets
// */
//void UpdateTargets6(double t)
//{
//    targets_[0].Set(-0.3f, 0.3f + 0.3f * (sin(12.0f * t)),         0.3f + t);
//    targets_[1].Set( 0.3f, 0.3f + 0.3f * (sin(12.0f * t + 3.14f)), 0.3f + t);
//}
//
///*
// * Skeleton
// */
//void BuildTree6()
//{
//    const double  x15[3] = {  15.0, 0.0,   0.0 };
//    const double xm15[3] = { -15.0, 0.0,   0.0 };
//    const double  z90[3] = {   0.0, 0.0,  90.0 };
//    const double zm90[3] = {   0.0, 0.0, -90.0 };
//
//    tree_.Reset();
//    node_.resize(30);
//
//    node_[0] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT);  // Root
//    tree_.InsertRoot(&node_[0], false);
//    node_[1] = Node(3, VectorR3(0.17f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left femur
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(3, VectorR3(0.0f, -0.82f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left tibia
//    tree_.InsertChild(&node_[1], &node_[2]);
//    node_[3] = Node(3, VectorR3(0.0f, -0.79f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left foot
//    tree_.InsertChild(&node_[2], &node_[3]);
//    node_[4] = Node(3, VectorR3(0.0f, -0.07f, 0.27f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left toe
//    tree_.InsertChild(&node_[3], &node_[4]);
//    node_[5] = Node(3, VectorR3(0.0f, 0.0f, 0.15f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left toe end
//    tree_.InsertChild(&node_[4], &node_[5]);
//    node_[6] = Node(3, VectorR3(-0.17f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right femur
//    tree_.InsertChild(&node_[0], &node_[6]);
//    node_[7] = Node(3, VectorR3(0.0f, -0.82f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right tibia
//    tree_.InsertChild(&node_[6], &node_[7]);
//    node_[8] = Node(3, VectorR3(0.0f, -0.79f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right foot
//    tree_.InsertChild(&node_[7], &node_[8]);
//    node_[9] = Node(3, VectorR3(0.0f, -0.07f, 0.27f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right toe
//    tree_.InsertChild(&node_[8], &node_[9]);
//    node_[10] = Node(3, VectorR3(0.0f, 0.0f, 0.15f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right toe end
//    tree_.InsertChild(&node_[9], &node_[10]);
//    node_[11] = Node(3, VectorR3(0.0f, 0.002f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Lower back
//    tree_.InsertChild(&node_[0], &node_[11]);
//    node_[12] = Node(3, VectorR3(0.0f, 0.32f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Thorax
//    tree_.InsertChild(&node_[11], &node_[12]);
//    node_[13] = Node(3, VectorR3(0.0f, 0.6f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Neck
//    tree_.InsertChild(&node_[12], &node_[13]);
//    node_[14] = Node(3, VectorR3(0.0f, 0.37f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Head
//    tree_.InsertChild(&node_[13], &node_[14]);
//    node_[15] = Node(3, VectorR3(0.0f, 0.23f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Head end
//    tree_.InsertChild(&node_[14], &node_[15]);
//    node_[16] = Node(3, VectorR3(0.0f, 0.6f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left collar
//    tree_.InsertChild(&node_[12], &node_[16]);
//    node_[17] = Node(3, VectorR3(0.41f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left humerus
//    tree_.InsertChild(&node_[16], &node_[17]);
//    node_[18] = Node(3, VectorR3(0.52f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left elbow
//    tree_.InsertChild(&node_[17], &node_[18]);
//    node_[19] = Node(3, VectorR3(0.51f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left wrist
//    tree_.InsertChild(&node_[18], &node_[19]);
//    node_[20] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left thumb
//    tree_.InsertChild(&node_[19], &node_[20]);
//    node_[21] = Node(3, VectorR3(0.0f, 0.0f, 0.2f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left thumb end
//    tree_.InsertChild(&node_[20], &node_[21]);
//    node_[22] = Node(3, VectorR3(0.2f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left wrist end
//    tree_.InsertChild(&node_[19], &node_[22]);
//    node_[23] = Node(3, VectorR3(0.0f, 0.6f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right collar
//    tree_.InsertChild(&node_[12], &node_[23]);
//    node_[24] = Node(3, VectorR3(-0.41f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right humerus
//    tree_.InsertChild(&node_[23], &node_[24]);
//    node_[25] = Node(3, VectorR3(-0.52f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right elbow
//    tree_.InsertChild(&node_[24], &node_[25]);
//    node_[26] = Node(3, VectorR3(-0.51f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right wrist
//    tree_.InsertChild(&node_[25], &node_[26]);
//    node_[27] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right thumb
//    tree_.InsertChild(&node_[26], &node_[27]);
//    node_[28] = Node(3, VectorR3(0.0f, 0.0f, 0.2f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right thumb end
//    tree_.InsertChild(&node_[27], &node_[28]);
//    node_[29] = Node(3, VectorR3(-0.27f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right wrist end
//    tree_.InsertChild(&node_[26], &node_[29]);
//}
//
///*
// * Forward walking (linear constraints)
// */
//void RunTest14()
//{
//    trajectory_.clear();
//    jointTrajectory_.clear();
//
//    ReadMarkers(trajectory_, "./data/forwards_walking.txt");
//    ReadBVHFile(jointTrajectory_, "./data/forwards_walking.bvh");
//
//    ProcessEulerAngles(jointTrajectory_);
//
//    for (int i = 0; i < jointTrajectory_.size(); ++i) {
//        jointTrajectory_[i][0] *= 0.06;
//    }
//
//    m_ = jointTrajectory_[0].size();
//    k_ = 2;
//    n_ = trajectory_.size();
//
//    BuildTree7();
//    AssignIndex();
//
//    node_[currentNode_].Select();
//
//    VectorR3 temp(jointTrajectory_[0][0]);
//
//    for (int i = 0; i < jointTrajectory_.size(); ++i)
//    {
//        jointTrajectory_[i][0] -= temp;
//
//        for (int j = 1; j < jointTrajectory_[i].size(); ++j) {
//            jointTrajectory_[i][j] *= DegreesToRadians;
//        }
//    }
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(0);
//
//    UpdateTargets = UpdateTargets14;
//
//    BuildMatrixX();
//    BuildMatrixZ();
//    BuildMatrixB();
//}
//
///*
// * Solves linear constraints
// */
//void UpdateTargets14(double t)
//{
//    BuildVectorM();
//    BuildVectorN();
//
//    node_[0].SetPosition(N_(0), N_(1), N_(2));
//    node_[0].SetRotation(N_(3), N_(4), N_(5));
//
//    for (int i = 1; i < 30; ++i) {
//        int j;
//        if ((j = node_[i].GetIndex()) != -1)
//            node_[i].SetRotation(N_(3 * j), N_(3 * j + 1), N_(3 * j + 2));
//    }
//
//    if (step < trajectory_.size() - 1)
//        step += 1;
//}
//
///*
// * Full-body  Skeleton
// */
//void BuildTree7()
//{
//    const double  x15[3] = {  15.0, 0.0,   0.0 };
//    const double xm15[3] = { -15.0, 0.0,   0.0 };
//    const double  z90[3] = {   0.0, 0.0,  90.0 };
//    const double zm90[3] = {   0.0, 0.0, -90.0 };
//
//    tree_.Reset();
//    node_.resize(30);
//
//    node_[0] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT);  // Root
//    tree_.InsertRoot(&node_[0], false);
//    node_[1] = Node(3, VectorR3(0.2f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left femur
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(3, VectorR3(0.0f, -0.87f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left tibia
//    tree_.InsertChild(&node_[1], &node_[2]);
//    node_[3] = Node(3, VectorR3(0.0f, -0.85f, 0.0f), kRotationAxes, x15,
//        0.05, JOINT); // Left foot
//    tree_.InsertChild(&node_[2], &node_[3]);
//    node_[4] = Node(3, VectorR3(0.0f, -0.08f, 0.32f), kRotationAxes, xm15,
//        0.05, JOINT); // Left toe
//    tree_.InsertChild(&node_[3], &node_[4]);
//    node_[5] = Node(3, VectorR3(0.0f, 0.0f, 0.16f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left toe end
//    tree_.InsertChild(&node_[4], &node_[5]);
//    node_[6] = Node(3, VectorR3(-0.2f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right femur
//    tree_.InsertChild(&node_[0], &node_[6]);
//    node_[7] = Node(3, VectorR3(0.0f, -0.87f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right tibia
//    tree_.InsertChild(&node_[6], &node_[7]);
//    node_[8] = Node(3, VectorR3(0.0f, -0.85f, 0.0f), kRotationAxes, x15,
//        0.05, JOINT); // Right foot
//    tree_.InsertChild(&node_[7], &node_[8]);
//    node_[9] = Node(3, VectorR3(0.0f, -0.085f, 0.32f), kRotationAxes, xm15,
//        0.05, JOINT); // Right toe
//    tree_.InsertChild(&node_[8], &node_[9]);
//    node_[10] = Node(3, VectorR3(0.0f, 0.0f, 0.16f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right toe end
//    tree_.InsertChild(&node_[9], &node_[10]);
//    node_[11] = Node(3, VectorR3(0.0f, 0.002f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Lower back
//    tree_.InsertChild(&node_[0], &node_[11]);
//    node_[12] = Node(3, VectorR3(0.0f, 0.56f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Thorax
//    tree_.InsertChild(&node_[11], &node_[12]);
//    node_[13] = Node(3, VectorR3(0.0f, 0.43f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Neck
//    tree_.InsertChild(&node_[12], &node_[13]);
//    node_[14] = Node(3, VectorR3(0.0f, 0.38f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Head
//    tree_.InsertChild(&node_[13], &node_[14]);
//    node_[15] = Node(3, VectorR3(0.0f, 0.25f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Head end
//    tree_.InsertChild(&node_[14], &node_[15]);
//    node_[16] = Node(3, VectorR3(0.0f, 0.43f, 0.0f), kRotationAxes, zm90,
//        0.05, JOINT); // Left collar
//    tree_.InsertChild(&node_[12], &node_[16]);
//    node_[17] = Node(3, VectorR3(0.37f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left humerus
//    tree_.InsertChild(&node_[16], &node_[17]);
//    node_[18] = Node(3, VectorR3(0.61f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left elbow
//    tree_.InsertChild(&node_[17], &node_[18]);
//    node_[19] = Node(3, VectorR3(0.47f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left wrist
//    tree_.InsertChild(&node_[18], &node_[19]);
//    node_[20] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left thumb
//    tree_.InsertChild(&node_[19], &node_[20]);
//    node_[21] = Node(3, VectorR3(0.0f, 0.0f, 0.22f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left thumb end
//    tree_.InsertChild(&node_[20], &node_[21]);
//    node_[22] = Node(3, VectorR3(0.22f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left wrist end
//    tree_.InsertChild(&node_[19], &node_[22]);
//    node_[23] = Node(3, VectorR3(0.0f, 0.43f, 0.0f), kRotationAxes, z90,
//        0.08, JOINT); // Right collar
//    tree_.InsertChild(&node_[12], &node_[23]);
//    node_[24] = Node(3, VectorR3(-0.37f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right humerus
//    tree_.InsertChild(&node_[23], &node_[24]);
//    node_[25] = Node(3, VectorR3(-0.61f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right elbow
//    tree_.InsertChild(&node_[24], &node_[25]);
//    node_[26] = Node(3, VectorR3(-0.47f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right wrist
//    tree_.InsertChild(&node_[25], &node_[26]);
//    node_[27] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right thumb
//    tree_.InsertChild(&node_[26], &node_[27]);
//    node_[28] = Node(3, VectorR3(0.0f, 0.0f, 0.22f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right thumb end
//    tree_.InsertChild(&node_[27], &node_[28]);
//    node_[29] = Node(3, VectorR3(-0.3f, 0.0, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right wrist end
//    tree_.InsertChild(&node_[26], &node_[29]);
//}
//
///*
// * Full-body skeleton
// */
//void BuildTree8()
//{
//    const double  x15[3] = {  15.0, 0.0,   0.0 };
//    const double xm15[3] = { -15.0, 0.0,   0.0 };
//    const double  z90[3] = {   0.0, 0.0,  90.0 };
//    const double zm90[3] = {   0.0, 0.0, -90.0 };
//
//    tree_.Reset();
//    node_.resize(30);
//
//    node_[0] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT);  // Root
//    tree_.InsertRoot(&node_[0], false);
//    node_[1] = Node(3, VectorR3(0.2f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left femur
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(1, VectorR3(0.0f, -0.87f, 0.0f), kUnitxxx, kNoRotation,
//        0.05, JOINT); // Left tibia
//    tree_.InsertChild(&node_[1], &node_[2]);
//    node_[3] = Node(3, VectorR3(0.0f, -0.85f, 0.0f), kRotationAxes, x15,
//        0.05, JOINT); // Left foot
//    tree_.InsertChild(&node_[2], &node_[3]);
//    node_[4] = Node(1, VectorR3(0.0f, -0.08f, 0.32f), kUnitxxx, xm15,
//        0.05, JOINT); // Left toe
//    tree_.InsertChild(&node_[3], &node_[4]);
//    node_[5] = Node(3, VectorR3(0.0f, 0.0f, 0.16f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left toe end
//    tree_.InsertChild(&node_[4], &node_[5]);
//    node_[6] = Node(3, VectorR3(-0.2f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right femur
//    tree_.InsertChild(&node_[0], &node_[6]);
//    node_[7] = Node(1, VectorR3(0.0f, -0.87f, 0.0f), kUnitxxx, kNoRotation,
//        0.05, JOINT); // Right tibia
//    tree_.InsertChild(&node_[6], &node_[7]);
//    node_[8] = Node(3, VectorR3(0.0f, -0.85f, 0.0f), kRotationAxes, x15,
//        0.05, JOINT); // Right foot
//    tree_.InsertChild(&node_[7], &node_[8]);
//    node_[9] = Node(1, VectorR3(0.0f, -0.085f, 0.32f), kUnitxxx, xm15,
//        0.05, JOINT); // Right toe
//    tree_.InsertChild(&node_[8], &node_[9]);
//    node_[10] = Node(3, VectorR3(0.0f, 0.0f, 0.16f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Right toe end
//    tree_.InsertChild(&node_[9], &node_[10]);
//    node_[11] = Node(3, VectorR3(0.0f, 0.002f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Lower back
//    tree_.InsertChild(&node_[0], &node_[11]);
//    node_[12] = Node(3, VectorR3(0.0f, 0.56f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Thorax
//    tree_.InsertChild(&node_[11], &node_[12]);
//    node_[13] = Node(3, VectorR3(0.0f, 0.43f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Neck
//    tree_.InsertChild(&node_[12], &node_[13]);
//    node_[14] = Node(3, VectorR3(0.0f, 0.38f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Head
//    tree_.InsertChild(&node_[13], &node_[14]);
//    node_[15] = Node(3, VectorR3(0.0f, 0.25f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Head end
//    tree_.InsertChild(&node_[14], &node_[15]);
//    node_[16] = Node(3, VectorR3(0.0f, 0.43f, 0.0f), kRotationAxes, zm90,
//        0.05, JOINT); // Left collar
//    tree_.InsertChild(&node_[12], &node_[16]);
//    node_[17] = Node(3, VectorR3(0.37f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left humerus
//    tree_.InsertChild(&node_[16], &node_[17]);
//    node_[18] = Node(1, VectorR3(0.61f, 0.0f, 0.0f), kUnityyy, kNoRotation,
//        0.05, JOINT); // Left elbow
//    tree_.InsertChild(&node_[17], &node_[18]);
//    node_[19] = Node(3, VectorR3(0.47f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left wrist
//    tree_.InsertChild(&node_[18], &node_[19]);
//    node_[20] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left thumb
//    tree_.InsertChild(&node_[19], &node_[20]);
//    node_[21] = Node(3, VectorR3(0.0f, 0.0f, 0.22f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left thumb end
//    tree_.InsertChild(&node_[20], &node_[21]);
//    node_[22] = Node(3, VectorR3(0.22f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.05, JOINT); // Left wrist end
//    tree_.InsertChild(&node_[19], &node_[22]);
//    node_[23] = Node(3, VectorR3(0.0f, 0.43f, 0.0f), kRotationAxes, z90,
//        0.08, JOINT); // Right collar
//    tree_.InsertChild(&node_[12], &node_[23]);
//    node_[24] = Node(3, VectorR3(-0.37f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right humerus
//    tree_.InsertChild(&node_[23], &node_[24]);
//    node_[25] = Node(1, VectorR3(-0.61f, 0.0f, 0.0f), kUnityyy, kNoRotation,
//        0.08, JOINT); // Right elbow
//    tree_.InsertChild(&node_[24], &node_[25]);
//    node_[26] = Node(3, VectorR3(-0.47f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right wrist
//    tree_.InsertChild(&node_[25], &node_[26]);
//    node_[27] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right thumb
//    tree_.InsertChild(&node_[26], &node_[27]);
//    node_[28] = Node(3, VectorR3(0.0f, 0.0f, 0.22f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right thumb end
//    tree_.InsertChild(&node_[27], &node_[28]);
//    node_[29] = Node(3, VectorR3(-0.3f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//        0.08, JOINT); // Right wrist end
//    tree_.InsertChild(&node_[26], &node_[29]);
//}
//
///*
// * Full-body skeleton
// */
//void BuildTree5()
//{
//    const double  x15[3] = {  15.0, 0.0,   0.0 };
//    const double xm15[3] = { -15.0, 0.0,   0.0 };
//    const double  z90[3] = {   0.0, 0.0,  90.0 };
//    const double zm90[3] = {   0.0, 0.0, -90.0 };
//
//    tree_.Reset();
//    node_.resize(30);
//
//    node_[0] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//                    0.05, JOINT);  // Root
//    tree_.InsertRoot(&node_[0], false);
//    node_[1] = Node(3, VectorR3(0.2f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//                    0.05, JOINT); // Left femur
//    tree_.InsertChild(&node_[0], &node_[1]);
//    node_[2] = Node(1, VectorR3(0.0f, -1.0f, 0.0f), kRotationAxes, kNoRotation,
//                    0.05, JOINT); // Left tibia
//    tree_.InsertChild(&node_[1], &node_[2]);
//    node_[3] = Node(3, VectorR3(0.0f, -0.92f, 0.0f), kRotationAxes, x15,
//                    0.05, JOINT); // Left foot
//    tree_.InsertChild(&node_[2], &node_[3]);
//    node_[4] = Node(1, VectorR3(0.0f, 0.0f, 0.35f), kRotationAxes, xm15,
//                    0.05, JOINT); // Left toe
//    tree_.InsertChild(&node_[3], &node_[4]);
//    node_[5] = Node(3, VectorR3(0.0f, 0.0f, 0.15f), kRotationAxes, kNoRotation,
//                    0.05, JOINT); // Left toe end
//    tree_.InsertChild(&node_[4], &node_[5]);
//    node_[6] = Node(3, VectorR3(-0.2f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//                    0.05, JOINT); // Right femur
//    tree_.InsertChild(&node_[0], &node_[6]);
//    node_[7] = Node(1, VectorR3(0.0f, -1.0f, 0.0f), kRotationAxes, kNoRotation,
//                    0.05, JOINT); // Right tibia
//    tree_.InsertChild(&node_[6], &node_[7]);
//    node_[8] = Node(3, VectorR3(0.0f, -0.92f, 0.0f), kRotationAxes, x15,
//                    0.05, JOINT); // Right foot
//    tree_.InsertChild(&node_[7], &node_[8]);
//    node_[9] = Node(1, VectorR3(0.0f, 0.0f, 0.35f), kRotationAxes, xm15,
//                    0.05, JOINT); // Right toe
//    tree_.InsertChild(&node_[8], &node_[9]);
//    node_[10] = Node(3, VectorR3(0.0f, 0.0f, 0.15f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Right toe end
//    tree_.InsertChild(&node_[9], &node_[10]);
//    node_[11] = Node(3, VectorR3(0.0f, 0.002f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Lower back
//    tree_.InsertChild(&node_[0], &node_[11]);
//    node_[12] = Node(3, VectorR3(0.0f, 0.5f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Thorax
//    tree_.InsertChild(&node_[11], &node_[12]);
//    node_[13] = Node(3, VectorR3(0.0f, 0.5f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Neck
//    tree_.InsertChild(&node_[12], &node_[13]);
//    node_[14] = Node(3, VectorR3(0.0f, 0.23f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Head
//    tree_.InsertChild(&node_[13], &node_[14]);
//    node_[15] = Node(3, VectorR3(0.0f, 0.23f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Head end
//    tree_.InsertChild(&node_[14], &node_[15]);
//    node_[16] = Node(3, VectorR3(0.0f, 0.5f, 0.0f), kRotationAxes, zm90,
//                     0.05, JOINT); // Left collar
//    tree_.InsertChild(&node_[12], &node_[16]);
//    node_[17] = Node(3, VectorR3(0.0f, 0.3f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Left humerus
//    tree_.InsertChild(&node_[16], &node_[17]);
//    node_[18] = Node(1, VectorR3(0.0f, 0.6f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Left elbow
//    tree_.InsertChild(&node_[17], &node_[18]);
//    node_[19] = Node(3, VectorR3(0.0f, 0.55f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Left wrist
//    tree_.InsertChild(&node_[18], &node_[19]);
//    node_[20] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Left thumb
//    tree_.InsertChild(&node_[19], &node_[20]);
//    node_[21] = Node(3, VectorR3(0.0f, 0.0f, 0.2f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Left thumb end
//    tree_.InsertChild(&node_[20], &node_[21]);
//    node_[22] = Node(3, VectorR3(0.0f, 0.2f, 0.0f), kRotationAxes, kNoRotation,
//                     0.05, JOINT); // Left wrist end
//    tree_.InsertChild(&node_[19], &node_[22]);
//    node_[23] = Node(3, VectorR3(0.0f, 0.5f, 0.0f), kRotationAxes, z90,
//                     0.08, JOINT); // Right collar
//    tree_.InsertChild(&node_[12], &node_[23]);
//    node_[24] = Node(3, VectorR3(0.0f, 0.3f, 0.0f), kRotationAxes, kNoRotation,
//                     0.08, JOINT); // Right humerus
//    tree_.InsertChild(&node_[23], &node_[24]);
//    node_[25] = Node(1, VectorR3(0.0f, 0.6f, 0.0f), kRotationAxes, kNoRotation,
//                     0.08, JOINT); // Right elbow
//    tree_.InsertChild(&node_[24], &node_[25]);
//    node_[26] = Node(3, VectorR3(0.0f, 0.55f, 0.0f), kRotationAxes, kNoRotation,
//                     0.08, JOINT); // Right wrist
//    tree_.InsertChild(&node_[25], &node_[26]);
//    node_[27] = Node(3, VectorR3(0.0f, 0.0f, 0.0f), kRotationAxes, kNoRotation,
//                     0.08, JOINT); // Right thumb
//    tree_.InsertChild(&node_[26], &node_[27]);
//    node_[28] = Node(3, VectorR3(0.0f, 0.0f, 0.2f), kRotationAxes, kNoRotation,
//                     0.08, JOINT); // Right thumb end
//    tree_.InsertChild(&node_[27], &node_[28]);
//    node_[29] = Node(3, VectorR3(0.0f, 0.2f, 0.0f), kRotationAxes, kNoRotation,
//                     0.08, JOINT); // Right wrist end
//    tree_.InsertChild(&node_[26], &node_[29]);
//}
//
///*
// * Skeleton swaying in place
// */
//void RunTest7()
//{
//    BuildTree5();
//    tree_.MakeJointAndEffector(&node_[0]);
//    tree_.MakeEffector(&node_[3]);
//    tree_.MakeEffector(&node_[8]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(3);
//    UpdateTargets = UpdateTargets7;
//
//    RebuildJacobian();
//}
//
///*
// * 3 targets
// */
//void UpdateTargets7(double t)
//{
//    targets_[0].Set(0.0f + 0.4f * sin(12.0f * t),
//                    0.0f,
//                    0.0f + 0.4f * cos(12.0f * t));
//    targets_[1].Set(0.2f, -1.92f, 0.0f);
//    targets_[2].Set(-0.2f, -1.92f, 0.0f);
//}
//
///*
// * Skeleton walking with feet trajectories from mocap data
// */
//void RunTest8()
//{
//    BuildTree5();
//    tree_.MakeEffector(&node_[3]); // left ankle
//    tree_.MakeEffector(&node_[8]); // right ankle
//    tree_.MakeEffector(&node_[1]); // left femur
//    tree_.MakeEffector(&node_[6]); // right femur
//    tree_.MakeEffector(&node_[17]); // left humerus
//    tree_.MakeEffector(&node_[24]); // right humerus
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    ReadMarkers(trajectory_, "./data/test4.txt");
//    TransformMarkers(trajectory_, VectorR3(600.0, 600.0, -600.0),
//                     VectorR3(0.2, -1.92, 0.0));
//    //PrintMarkers(trajectory_, "test_processed.txt");
//    targets_.resize(0);
//    targets_.resize(6);
//    UpdateTargets = UpdateTargets8;
//
//    RebuildJacobian();
//}
//
///*
// * 2 targets whose trajectories are taken from mocap data
// */
//void UpdateTargets8(double t)
//{
//    VectorR3 oldLeftAnkle = targets_[1];
//    VectorR3 oldRightAnkle = targets_[0];
//
//    targets_[1] = trajectory_[step][0]; // left ankle
//    targets_[0] = trajectory_[step][1]; // right ankle
//
//    targets_[2] = node_[1].GetGlobalPosition(); // left femur
//    targets_[3] = node_[6].GetGlobalPosition(); // right femur
//    targets_[4] = node_[17].GetGlobalPosition(); // left humerus
//    targets_[5] = node_[24].GetGlobalPosition(); // right humerus
//
//    if (step > 0) {
//        targets_[2].x += targets_[1].x - oldLeftAnkle.x;
//        targets_[3].x += targets_[0].x - oldRightAnkle.x;
//        targets_[4].x += targets_[1].x - oldLeftAnkle.x;
//        targets_[5].x += targets_[0].x - oldRightAnkle.x;
//
//        targets_[2].z += targets_[1].z - oldLeftAnkle.z;
//        targets_[3].z += targets_[0].z - oldRightAnkle.z;
//        targets_[4].z += targets_[1].z - oldLeftAnkle.z;
//        targets_[5].z += targets_[0].z - oldRightAnkle.z;
//    }
//
//    if (step < trajectory_.size() - 2)
//        step += 2;
//}
//
///*
// * Walking backwards, foot trajectories taken from mocap data
// */
//void RunTest9()
//{
//    ReadMarkers(trajectory_, "./data/walk_backwards.txt");
//    TransformMarkers(trajectory_, VectorR3(800.0, 400.0, 800.0),
//                     VectorR3(0.2, -1.92, 1.0));
//    //PrintMarkers(trajectory_, "walk_backwards_processed.txt");
//
//    BuildTree5();
//    tree_.MakeEffector(&node_[3]);
//    tree_.MakeEffector(&node_[8]);
//    //tree_.MakeJointAndEffector(&node_[0]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(0);
//    targets_.resize(2);
//    UpdateTargets = UpdateTargets9;
//
//    RebuildJacobian();
//}
//
///*
// * 2 targets, trajectories taken from mocap data
// */
//void UpdateTargets9(double t)
//{
//    targets_[0].Set(trajectory_[step][0].x,
//                    trajectory_[step][0].y,
//                    trajectory_[step][0].z);
//    targets_[1].Set(trajectory_[step][1].x,
//                    trajectory_[step][1].y,
//                    trajectory_[step][1].z);
//    /*
//    if (step > 0) {
//        targets_[2].x += targets_[1].x - oldLeftAnkle.x;
//
//        targets_[2].z += targets_[1].z - oldLeftAnkle.z;
//    }
//    */
//    if (step < trajectory_.size() - 2)
//        step += 2;
//}
//
///*
// * Running, foot trajectories taken from mocap data
// */
//void RunTest10()
//{
//    ReadMarkers(trajectory_, "./data/running2.txt");
//    TransformMarkers(trajectory_, VectorR3(600.0, 600.0, 600.0),
//                     VectorR3(-0.2, -0.3, -1.92));
//    //PrintMarkers(trajectory_, "running_processed.txt");
//
//    BuildTree5();
//    tree_.MakeEffector(&node_[3]); // left ankle
//    tree_.MakeEffector(&node_[8]); // right ankle
//    tree_.MakeEffector(&node_[17]); // left humerus
//    tree_.MakeEffector(&node_[24]); // right humerus
//    tree_.MakeJointAndEffector(&node_[0]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(0);
//    targets_.resize(5);
//    UpdateTargets = UpdateTargets10;
//
//    RebuildJacobian();
//}
//
///*
// * 2 targets, trajectories taken from mocap data
// */
//void UpdateTargets10(double t)
//{
//    VectorR3 oldLeftAnkle = targets_[1];
//    VectorR3 oldRightAnkle = targets_[0];
//
//    targets_[1].x = trajectory_[step][0].x; // left ankle
//    targets_[1].y = trajectory_[step][0].z;
//    targets_[1].z = trajectory_[step][0].y;
//    targets_[0].x = trajectory_[step][1].x; // right ankle
//    targets_[0].y = trajectory_[step][1].z;
//    targets_[0].z = trajectory_[step][1].y;
//
//    targets_[2] = node_[17].GetGlobalPosition(); // left femur
//    targets_[3] = node_[24].GetGlobalPosition(); // right femur
//
//    if (step > 0) {
//        targets_[2].x += targets_[1].x - oldLeftAnkle.x;
//        targets_[3].x += targets_[0].x - oldRightAnkle.x;
//
//        targets_[2].z += targets_[1].z - oldLeftAnkle.z;
//        targets_[3].z += targets_[0].z - oldRightAnkle.z;
//
//        targets_[4].x += (targets_[1].x - oldLeftAnkle.x +
//                          targets_[0].x - oldRightAnkle.x) / 2.0;
//        targets_[4].z += (targets_[1].z - oldLeftAnkle.z +
//                          targets_[0].z - oldRightAnkle.z) / 2.0;
//        targets_[4].y = node_[0].GetGlobalPosition().y;
//    }
//
//    if (step < trajectory_.size() - 2)
//        step += 2;
//}
//
///*
// * Backward walking
// */
//void RunTest12()
//{
//    ReadBVHFile(trajectory_, "./data/walk_backwards.bvh");
//
//    BuildTree6();
//    AssignIndex();
//
//    node_[currentNode_].Select();
//
//    VectorR3 temp(trajectory_[0][0]);
//    for (int i = 0; i < trajectory_.size(); ++i)
//    {
//        trajectory_[i][0] -= temp;
//
//        for (int j = 0; j < trajectory_[i].size(); ++j) {
//            trajectory_[i][j] *= DegreesToRadians;
//        }
//    }
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(0);
//
//    UpdateTargets = UpdateTargets12;
//}
//
///*
// * Updates targets
// */
//void UpdateTargets12(double t)
//{
//    node_[0].SetPosition(trajectory_[step][0] * 3);
//    node_[0].SetRotation(trajectory_[step][1]);
//
//    for (int i = 1; i < 30; ++i) {
//        int j;
//        if ((j = node_[i].GetIndex()) != -1)
//            node_[i].SetRotation(trajectory_[step][j]);
//    }
//
//    if (step < trajectory_.size() - 1)
//        step += 1;
//}
//
///*
// * Jumping (mocap)
// */
//void RunTest11()
//{
//    ReadMarkers(trajectory_, "./data/jumping.txt");
//    TransformMarkers(trajectory_, VectorR3(600.0, 600.0, 600.0),
//                     VectorR3(-0.2, -1.92, -0.2));
//
//    BuildTree5();
//    tree_.MakeEffector(&node_[3]); // left ankle
//    tree_.MakeEffector(&node_[8]); // right ankle
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.clear();
//    targets_.resize(2);
//    UpdateTargets = UpdateTargets11;
//
//    RebuildJacobian();
//}
//
///*
// * Updates targets
// */
//void UpdateTargets11(double t)
//{
//    targets_[0].z = trajectory_[step][1].x;
//    targets_[0].y = trajectory_[step][1].y;
//    targets_[0].x = trajectory_[step][1].z;
//    targets_[1].z = trajectory_[step][0].x;
//    targets_[1].y = trajectory_[step][0].y;
//    targets_[1].x = trajectory_[step][0].z;
//
//    if (step < trajectory_.size() - 2)
//        step += 2;
//}
//
///*
// * Backward walking (linear constraints)
// */
//void RunTest13()
//{
//    trajectory_.clear();
//    jointTrajectory_.clear();
//
//    ReadMarkers(trajectory_, "./data/walking_backwards.txt");
//    ReadBVHFile(jointTrajectory_, "./data/walk_backwards.bvh");
//    ProcessEulerAngles(jointTrajectory_);
//
//    for (int i = 0; i < jointTrajectory_.size(); ++i) {
//        jointTrajectory_[i][0] *= 0.05;
//    }
//
//    m_ = jointTrajectory_[0].size();
//    k_ = 2;
//    n_ = trajectory_.size();
//
//    BuildTree6();
//    AssignIndex();
//
//    node_[currentNode_].Select();
//
//    VectorR3 temp(jointTrajectory_[0][0]);
//
//    for (int i = 0; i < jointTrajectory_.size(); ++i)
//    {
//        jointTrajectory_[i][0] -= temp;
//
//        for (int j = 1; j < jointTrajectory_[i].size(); ++j) {
//            jointTrajectory_[i][j] *= DegreesToRadians;
//        }
//    }
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(0);
//
//    UpdateTargets = UpdateTargets13;
//
//    BuildMatrixX();
//    BuildMatrixZ();
//    BuildMatrixB();
//}
//
///*
// * Forward running (simple linear learning)
// */
//void RunTest17()
//{
//    trajectory_.clear();
//    jointTrajectory_.clear();
//
//    ReadMarkers(trajectory_, "./data/running_2.txt");
//    ReadBVHFile(jointTrajectory_, "./data/running_2.bvh");
//
//    ProcessEulerAngles(jointTrajectory_);
//
//    for (int i = 0; i < jointTrajectory_.size(); ++i) {
//        jointTrajectory_[i][0] *= 0.05;
//    }
//
//    m_ = jointTrajectory_[0].size();
//    k_ = 2;
//    n_ = trajectory_.size();
//
//    BuildTree7();
//    AssignIndex();
//
//    node_[currentNode_].Select();
//
//    VectorR3 temp(jointTrajectory_[0][0]);
//
//    for (int i = 0; i < jointTrajectory_.size(); ++i)
//    {
//        jointTrajectory_[i][0] -= temp;
//
//        for (int j = 1; j < jointTrajectory_[i].size(); ++j) {
//            jointTrajectory_[i][j] *= DegreesToRadians;
//        }
//    }
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.resize(0);
//
//    UpdateTargets = UpdateTargets17;
//
//    BuildMatrixX();
//    BuildMatrixZ();
//    BuildMatrixB();
//}
//
///*
// * Solves linear constraints
// */
//void UpdateTargets17(double t)
//{
//    BuildVectorM();
//    BuildVectorN();
//
//    node_[0].SetPosition(N_(0), N_(1), N_(2));
//    node_[0].SetRotation(N_(3), N_(4), N_(5));
//
//    for (int i = 1; i < 30; ++i) {
//        int j;
//        if ((j = node_[i].GetIndex()) != -1)
//            node_[i].SetRotation(N_(3 * j), N_(3 * j + 1), N_(3 * j + 2));
//    }
//
//    if (step < trajectory_.size() - 1)
//        step += 1;
//}
//
///*
// * Inplace walking with 9 targets
// */
//void RunTest16()
//{
//    ReadMarkers(trajectory_, "./data/inplace_walking_even_more_markers.txt");
//    TransformMarkers(trajectory_, VectorR3(500.0, 500.0, 500.0),
//        VectorR3(0.0, 0.0, 0.0));
//
//    BuildTree8();
//    tree_.MakeJointAndEffector(&node_[15]); // head end
//    tree_.MakeJointAndEffector(&node_[21]); // left thumb end
//    tree_.MakeJointAndEffector(&node_[28]); // right thumb end
//    tree_.MakeJointAndEffector(&node_[1] ); // left femur
//    tree_.MakeJointAndEffector(&node_[6] ); // right femur
//    tree_.MakeJointAndEffector(&node_[2] ); // left tibia
//    tree_.MakeJointAndEffector(&node_[7] ); // right tibia
//    tree_.MakeJointAndEffector(&node_[3] ); // left foot
//    tree_.MakeJointAndEffector(&node_[8] ); // right foot
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.clear();
//    targets_.resize(9);
//    UpdateTargets = UpdateTargets16;
//
//    RebuildJacobian();
//}
//
///*
// * Updates targets
// */
//void UpdateTargets16(double t)
//{
//    targets_[0] = trajectory_[step][0];
//    targets_[1] = trajectory_[step][1];
//    targets_[2] = trajectory_[step][2];
//    targets_[3] = (trajectory_[step][3] + trajectory_[step][4]) / 2.0;
//    targets_[4] = (trajectory_[step][5] + trajectory_[step][6]) / 2.0;
//    targets_[5] = trajectory_[step][7];
//    targets_[6] = trajectory_[step][9];
//    targets_[7] = trajectory_[step][8];
//    targets_[8] = trajectory_[step][10];
//    /*targets_[3] = trajectory_[step][7];
//    targets_[4] = trajectory_[step][9];
//    targets_[5] = trajectory_[step][8];
//    targets_[6] = trajectory_[step][10];*/
//
//    if (step < trajectory_.size() - 1)
//        step += 1;
//}
//
///*
// * Inplace walking with 5 targets
// */
//void RunTest15()
//{
//    ReadMarkers(trajectory_, "./data/inplace_walking_more_markers.txt");
//    TransformMarkers(trajectory_, VectorR3(500.0, 500.0, 500.0),
//                     VectorR3(0.0, 0.0, 0.0));
//
//    BuildTree8();
//    tree_.MakeEffector(&node_[15]);
//    tree_.MakeEffector(&node_[21]);
//    tree_.MakeEffector(&node_[28]);
//    //tree_.MakeEffector(&node_[0]);
//    tree_.MakeJointAndEffector(&node_[3]);
//    tree_.MakeJointAndEffector(&node_[8]);
//
//    node_[currentNode_].Select();
//
//    tree_.Compute();
//    //tree_.Print();
//
//    targets_.clear();
//    targets_.resize(5);
//    UpdateTargets = UpdateTargets15;
//
//    RebuildJacobian();
//}
//
///*
// * Updates targets
// */
//void UpdateTargets15(double t)
//{
//    targets_[0] = trajectory_[step][0];
//    targets_[1] = trajectory_[step][1];
//    targets_[2] = trajectory_[step][2];
//    //targets_[3] = (trajectory_[step][3] + trajectory_[step][4]) / 2.0;
//    //targets_[3].y -= 0.1;
//    //targets_[4] = trajectory_[step][5];
//    //targets_[5] = trajectory_[step][6];
//    targets_[3] = trajectory_[step][5];
//    targets_[4] = trajectory_[step][6];
//
//    if (step < trajectory_.size() - 1)
//        step += 1;
//}
//
///*
// * Builds M
// */
//void BuildVectorM()
//{
//    M_.resize(3 * k_);
//    for (int i = 0; i < k_; ++i) {
//        M_(3 * i    ) = trajectory_[step][i].x;
//        M_(3 * i + 1) = trajectory_[step][i].y;
//        M_(3 * i + 2) = trajectory_[step][i].z;
//    }
//}
//
///*
// * Builds N
// */
//void BuildVectorN()
//{
//    N_ = B_ * M_;
//}
//
///*
// * Updates targets
// */
//void UpdateTargets13(double t)
//{
//    BuildVectorM();
//    BuildVectorN();
//
//    node_[0].SetPosition(N_(0), N_(1), N_(2));
//    node_[0].SetRotation(N_(3), N_(4), N_(5));
//
//    for (int i = 1; i < 30; ++i) {
//        int j;
//        if ((j = node_[i].GetIndex()) != -1)
//            node_[i].SetRotation(N_(3 * j), N_(3 * j + 1), N_(3 * j + 2));
//    }
//
//    if (step < trajectory_.size() - 2)
//        step += 2;
//}
//
///*
// * Builds X
// */
//void BuildMatrixX()
//{
//    X_.resize(3 * k_, n_);
//    for (int i = 0; i < k_; ++i) {
//        for (int j = 0; j < n_; ++j) {
//            X_(3 * i    , j) = trajectory_[j][i].x;
//            X_(3 * i + 1, j) = trajectory_[j][i].y;
//            X_(3 * i + 2, j) = trajectory_[j][i].z;
//        }
//    }
//}
//
///*
// * Builds Z
// */
//void BuildMatrixZ()
//{
//    Z_.resize(3 * m_, n_);
//    for (int i = 0; i < m_; ++i) {
//        for (int j = 0; j < n_; ++j) {
//            Z_(3 * i    , j) = jointTrajectory_[j][i].x;
//            Z_(3 * i + 1, j) = jointTrajectory_[j][i].y;
//            Z_(3 * i + 2, j) = jointTrajectory_[j][i].z;
//        }
//    }
//}
//
///*
// * Builds B
// */
//void BuildMatrixB()
//{
//    B_ = Z_ * X_.transpose() * (X_ * X_.transpose()).inverse();
//}
//
///*
// * Assigns one index to each node
// * This number is used to index into the bvh format
// */
//void AssignIndex()
//{
//    node_[0].SetIndex(0); // Root
//    node_[1].SetIndex(2); // Left femur
//    node_[2].SetIndex(3); // Left tibia
//    node_[3].SetIndex(4); // Left foot
//    node_[4].SetIndex(5); // Left toe
//    node_[5].SetIndex(-1); // Left toe end
//    node_[6].SetIndex(6); // Right femur
//    node_[7].SetIndex(7); // Right tibia
//    node_[8].SetIndex(8); // Right foot
//    node_[9].SetIndex(9); // Right toe
//    node_[10].SetIndex(-1); // Right toe end
//    node_[11].SetIndex(10); // Lower back
//    node_[12].SetIndex(11); // Thorax
//    node_[13].SetIndex(12); // Neck
//    node_[14].SetIndex(13); // Head
//    node_[15].SetIndex(-1); // Head end
//    node_[16].SetIndex(14); // Left collar
//    node_[17].SetIndex(15); // Left humerus
//    node_[18].SetIndex(16); // Left elbow
//    node_[19].SetIndex(17); // Left wrist
//    node_[20].SetIndex(18); // Left thumb
//    node_[21].SetIndex(-1); // Left thumb end
//    node_[22].SetIndex(19); // Left wrist end
//    node_[23].SetIndex(20); // Right collar
//    node_[24].SetIndex(21); // Right humerus
//    node_[25].SetIndex(22); // Right elbow
//    node_[26].SetIndex(23); // Right wrist
//    node_[27].SetIndex(24); // Right thumb
//    node_[28].SetIndex(-1); // Right thumb end
//    node_[29].SetIndex(25); // Right wrist end
//}
//
///*
// * Returns the sign of x
// */
//double Sign(double x)
//{
//    return (x > 0) - (x < 0);
//}
//
///*
// * Avoids discontinuities in angles from consecutive frames
// */
//void ProcessEulerAngles(vector<vector<VectorR3>> &trajectory)
//{
//    for (int i = 1; i < trajectory.size(); ++i) {
//        if (i == 0)
//            continue;
//        for (int j = 1; j < trajectory[i].size(); ++j) {
//            VectorR3 &currentRotation = trajectory[i][j];
//            VectorR3 &lastRotation = trajectory[i - 1][j];
//            if (fabs(currentRotation.x - lastRotation.x) > 90.0f)
//                currentRotation.x = -1.0 * Sign(currentRotation.x) *
//                                    (360.0 - fabs(currentRotation.x));
//            if (fabs(currentRotation.y - lastRotation.y) > 90.0f)
//                currentRotation.y = -1.0 * Sign(currentRotation.y) *
//                                    (360.0 - fabs(currentRotation.y));
//            if (fabs(currentRotation.z - lastRotation.z) > 90.0f)
//                currentRotation.z = -1.0 * Sign(currentRotation.z) *
//                                    (360.0 - fabs(currentRotation.z));
//        }
//    }
//}
//
//// trim from end
//static inline string &rtrim(string &s) {
//    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
//    return s;
//}
//
//// trim from start
//static inline string &ltrim(string &s) {
//    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
//    return s;
//}
//
//// trim from both ends
//static inline string &trim(string &s) {
//    return ltrim(rtrim(s));
//}
//
///*
// * Builds a skeleton from a bvh
// */
//void BuildSkeletonFromBVH(const char *fileName)
//{
//    ifstream file(fileName);
//    string line;
//
//    getline(file, line); // HIERARCHY
//    int id = -1;
//    int level = -1;
//    string root("ROOT");
//    string offset("OFFSET");
//    string channels("CHANNELS");
//    string joint("JOINT");
//    string endSite("End Site");
//
//    while (line.compare("MOTION") != 0) {
//        getline(file, line);
//        line = trim(line);
//        if (line.compare(0, offset.length(), offset) == 0) {
//            istringstream lineStream(line);
//            string temp;
//            double x, y, z;
//            lineStream >> temp >> x >> y >> z;
//            node_[id] = Node(3, VectorR3(x, y, z), kRotationAxes, kNoRotation, 0.008, JOINT);
//            if (id > 0)
//                tree_.InsertChild(&node_[id - 1], &node_[id]);
//        } else if (line.compare(0, channels.length(), channels) == 0) {
//            continue;
//        } else if (line.compare(0, joint.length(), joint) == 0) {
//            ++id;
//            continue;
//        } else if (line.compare(0, endSite.length(), endSite)) {
//            continue;
//        } else if (line.compare(0, root.length(), root)) {
//            ++id;
//            continue;
//        } else if (line[0] == '{') {
//            ++level;
//        } else if (line[0] == '}') {
//            --level;
//        }
//    }
//
//    file.close();
//}
//
///*
// * Reads data from a bvh file
// */
//void ReadBVHFile(vector<vector<VectorR3>> &trajectory, const char *fileName)
//{
//    ifstream file(fileName);
//    string line;
//
//    getline(file, line);
//    while (line.compare("MOTION") != 0)
//        getline(file, line);
//
//    getline(file, line); // Frames: xxx
//    getline(file, line); // Frame Time: xxx
//
//    while (getline(file, line)) {
//        istringstream lineStream(line);
//
//        vector<VectorR3> currentFrame;
//        double x, y, z;
//        while(lineStream >> x >> y >> z)
//            currentFrame.push_back(VectorR3(x, y, z));
//
//        trajectory.push_back(currentFrame);
//    }
//
//    file.close();
//}
//
///*
// * Reads the markers' trajectories from a text file
// */
//void ReadMarkers(vector<vector<VectorR3>> &trajectory, const char *fileName)
//{
//    ifstream file(fileName);
//    string line;
//
//    getline(file, line); // ignore the first empty line
//
//    getline(file, line); // this line stores the markers' names
//    istringstream lineStream(line);
//    string markerName;
//    string temp;
//    lineStream >> temp; // the "Frame" string
//    int markerNo = 0;
//    while (lineStream >> markerName >> temp) {
//        markerName = markerName.substr(markerName.find(':') + 1);
//        //cout << markerName << " ";
//        markers_.push_back(markerName);
//        makersNameToInt_[markerName] = markerNo++;
//    }
//
//    while (getline(file, line)) {
//        lineStream.clear();
//        lineStream.str(line);
//
//        int frameNo;
//        lineStream >> frameNo; // ignore the frame's number
//
//        vector<VectorR3> currentFrame;
//        double x, y, z;
//        while(lineStream >> x >> y >> z)
//            currentFrame.push_back(VectorR3(x, y, z));
//
//        trajectory.push_back(currentFrame);
//    }
//
//    file.close();
//}
//
///*
// * Transforms the markers' coordinates to fit with the coordinate frame
//   of the program
// */
//void TransformMarkers(vector<vector<VectorR3>> &trajectory, VectorR3 &scale,
//                      VectorR3 &translate)
//{
//    // Scale the coordinates of the markers to fit with the coordinate frame
//    // of the program
//    // The scale factor should be different for each set of data
//    for (vector<vector<VectorR3>>::iterator it = trajectory.begin();
//                                            it < trajectory.end();
//                                            ++it) {
//        for (vector<VectorR3>::iterator v = (*it).begin();
//                                        v < (*it).end();
//                                        ++v) {
//            (*v).x /= scale.x;
//            (*v).y /= scale.y;
//            (*v).z /= scale.z;
//        }
//    }
//
//    // Translate the left ankle marker to the left ankle of the skeleton
//    double x = trajectory[0][0].x - translate.x;
//    double y = trajectory[0][0].y - translate.y;
//    double z = trajectory[0][0].z - translate.z;
//    for (vector<vector<VectorR3>>::iterator it = trajectory.begin();
//                                            it < trajectory.end();
//                                            ++it) {
//        for(vector<VectorR3>::iterator v = (*it).begin();
//                                       v < (*it).end();
//                                       ++v) {
//            (*v).x -= x;
//            (*v).y -= y;
//            (*v).z -= z;
//        }
//    }
//}
//
///*
// * Prints the transformed markers' data to a text file for debugging
// */
//void PrintMarkers(vector<vector<VectorR3>> &trajectory, char *fileName)
//{
//    ofstream file(fileName);
//    if (file.is_open()) {
//        int frameNo = 1;
//        for (vector<vector<VectorR3>>::const_iterator it = trajectory.begin();
//                                                      it < trajectory.end();
//                                                      ++it) {
//            file << setw(5) << setfill (' ') << frameNo++ << "    ";
//            file << setprecision(3) << fixed;
//            for (vector<VectorR3>::const_iterator v = (*it).begin();
//                                                  v < (*it).end();
//                                                  ++v) {
//                file << setw(8) << (*v).x << " " <<
//                        setw(8) << (*v).y << " " <<
//                        setw(8) << (*v).z << " ; ";
//            }
//            file << endl;
//        }
//    }
//}
//
///*
// * Converts Euler angles to a quaternion
// */
//void ConvertEToQ(float xRot, float yRot, float zRot, float *w, float *x, float *y, float *z)
//{
//    *w = cos(xRot / 2.0f) * cos(yRot / 2.0f) * cos(zRot / 2.0f) +
//         sin(xRot / 2.0f) * sin(yRot / 2.0f) * sin(zRot / 2.0f);
//    *x = sin(xRot / 2.0f) * cos(yRot / 2.0f) * cos(zRot / 2.0f) -
//         cos(xRot / 2.0f) * sin(yRot / 2.0f) * sin(zRot / 2.0f);
//    *y = cos(xRot / 2.0f) * sin(yRot / 2.0f) * cos(zRot / 2.0f) +
//         sin(xRot / 2.0f) * cos(yRot / 2.0f) * sin(zRot / 2.0f);
//    *z = cos(xRot / 2.0f) * cos(yRot / 2.0f) * sin(zRot / 2.0f) -
//         sin(xRot / 2.0f) * sin(yRot / 2.0f) * cos(zRot / 2.0f);
//}
//
///*
// * Converts a quaternion to Euler angles
// */
//void ConvertQToE(float w, float x, float y, float z, float *xRot, float *yRot, float *zRot)
//{
//    float sp = 2.0f * (w * y - x * z);
//
//    *xRot = atan2(y * z + w * x, 0.5f - x * x - y * y);
//    *yRot = asin(sp);
//    *zRot = atan2(x * y + w * z, 0.5f - y * y - z * z);
//}
//
//#ifdef IKINEMA
///*
// * Inplace walking (IKinema, 5 targets)
// */
//void RunTest18()
//{
//    LoadSkeleton("./data/inplace_walking_skeleton.txt");
//    LoadLimit("./data/inplace_walking_skeleton_limit.txt");
//
//    CreateTasks();
//
//    trajectory_.clear();
//    ReadMarkers(trajectory_, "./data/inplace_walking_more_markers.txt");
//    cout << trajectory_.size() << endl;
//    TransformMarkers(trajectory_, VectorR3(500.0, 500.0, 500.0),
//                     VectorR3(0.0, 0.0, 0.0));
//
//    g_pIKinemaSolver->updateFK();
//
//    targets_.clear();
//    targets_.resize(0);
//    UpdateTargets = UpdateTargets18;
//}
//
///*
// * Updates targets
// */
//void UpdateTargets18(double t)
//{
//    headTask_->setTarget(trajectory_[step][0].x, trajectory_[step][0].y,
//                         trajectory_[step][0].z);
//    leftHandTask_->setTarget(trajectory_[step][1].x, trajectory_[step][1].y,
//                             trajectory_[step][1].z);
//    rightHandTask_->setTarget(trajectory_[step][2].x, trajectory_[step][2].y,
//                              trajectory_[step][2].z);
//    leftFootTask_->setTarget(trajectory_[step][5].x, trajectory_[step][5].y,
//                             trajectory_[step][5].z);
//    rightFootTask_->setTarget(trajectory_[step][6].x, trajectory_[step][6].y,
//                              trajectory_[step][6].z);
//
//    g_pIKinemaSolver->solve();
//    g_pIKinemaSolver->updateFK();
//
//    if (step < trajectory_.size() - 1)
//        step += 1;
//}
//
///*
// * Draws the IKinema skeleton
// */
//void DrawSkeleton(IKSolver &solver)
//{
//    for (int i = 0; i < solver.numSegments(); ++i) {
//        IKSegment *segment = solver.getSegmentByHandle(i);
//
//        const FIK::Real *pos = segment->getGlobalPos();
//        glPushMatrix();
//        glTranslatef(pos[0], pos[1], pos[2]);
//        glColor3fv(kBlue);
//        glutSolidSphere(0.05f, 12, 12);
//        if (segment->Parent()) {
//            IKSegment *parent = segment->Parent();
//            const Real *parentPos = parent->getGlobalPos();
//            float x = parentPos[0] - pos[0];
//            float y = parentPos[1] - pos[1];
//            float z = parentPos[2] - pos[2];
//            glTranslatef(x / 2.0f, y / 2.0f, z / 2.0f);
//            VectorR3 axis(x, y, z);
//            DrawCylinder(axis, sqrt(x * x + y * y + z * z));
//        }
//
//        glPopMatrix();
//    }
//}
//
///*
// * Load IKinema's skeleton
// */
//void LoadSkeleton(const char *strFileSkeleton)
//{
//    char *pErrorMessage = 0;
//    g_pIKinemaSolver = new FIK::IKSolver(0,&pErrorMessage);
//    if( !g_pIKinemaSolver )
//    {
//        std::cout << "Error creating IKinema solver instance!\n";
//        return;
//    }
//    if( pErrorMessage )
//    {
//        std::cout << "IKinema licensing error!\n";
//        std::cout << pErrorMessage << std::endl;
//        return;
//    }
//
//    std::ifstream skeletonFile( strFileSkeleton );
//    if( !skeletonFile.good() )
//    {
//        std::cout << "Error opening skeleton file!\n";
//        return;
//    }
//    std::vector< FIK::ImportBone > vecSkeletonData;
//
//    std::list< std::string > boneNames;
//
//    while( skeletonFile.good() )
//    {
//        FIK::ImportBone curBone;
//        std::string strBoneName, strBoneParentName;
//        skeletonFile >> strBoneName >> strBoneParentName;
//        skeletonFile >> curBone.rest_offset[0] >> curBone.rest_offset[1] >>
//                        curBone.rest_offset[2];
//        skeletonFile >> curBone.rest_orientation[1] >> curBone.rest_orientation[2] >>
//                        curBone.rest_orientation[3] >> curBone.rest_orientation[0];
//
//        std::list< std::string >::iterator itName = std::find(boneNames.begin(),
//                                                              boneNames.end(),
//                                                              strBoneName);
//        if( itName == boneNames.end() )
//            boneNames.push_back( strBoneName );
//
//        curBone.name = boneNames.back().c_str();
//        curBone.parentname = 0;
//        if( strBoneParentName != "0" )
//        {
//            std::list< std::string >::iterator itParent = std::find(boneNames.begin(),
//                                                                    boneNames.end(),
//                                                                    strBoneParentName);
//            if( itParent == boneNames.end() )
//            {
//                boneNames.push_back( strBoneParentName );
//                curBone.parentname = boneNames.back().c_str();
//            }else
//                curBone.parentname = (*itParent).c_str();
//        }
//
//        if( ! skeletonFile.good() )
//            break;
//
//        vecSkeletonData.push_back( curBone );
//    }
//    skeletonFile.close();
//
//    g_pIKinemaSolver->importBones( &vecSkeletonData[0], (unsigned int)vecSkeletonData.size() );
//    g_pIKinemaSolver->translateRoot(true);
//}
//
///*
// * Loads IKinema's limit file
// */
//void LoadLimit(const char *strFileLimits)
//{
//    unsigned int iBoneNum = (unsigned int)g_pIKinemaSolver->numSegments();
//
//    std::ifstream skeletonLimits( strFileLimits );
//    if( !skeletonLimits.good() )
//    {
//        std::cout << "Error opening limits file!\n";
//        return;
//    }
//    std::string labels, boneName;
//    for( unsigned int i = 0 ; i < iBoneNum ; i++ )
//    {
//        FIK::EulerLimits limits;
//        bool bEnforced;
//
//        skeletonLimits >> boneName;
//        skeletonLimits >> labels >> limits.rot[0][0] >> labels >> limits.rot[0][1];
//        skeletonLimits >> labels >> limits.rot[1][0] >> labels >> limits.rot[1][1];
//        skeletonLimits >> labels >> limits.rot[2][0] >> labels >> limits.rot[2][1];
//        skeletonLimits >> labels >> bEnforced;
//        skeletonLimits >> labels >> limits.bone_axis;
//
//        if( !skeletonLimits.good() )
//            break;
//
//        FIK::IKSegment *seg = g_pIKinemaSolver->getSegment( boneName.c_str() );
//        if( !seg )
//            continue;
//
//        seg->setEulerLimits( limits );
//        seg->enforceLimits( bEnforced );
//        seg->enableLimits( true );
//    }
//    skeletonLimits.close();
//}
//
///*
// * Creates tasks for IKinema's skeleton
// */
//void CreateTasks()
//{
//    leftHandTask_ = g_pIKinemaSolver->addPositionTask("LeftHandThumb");
//    rightHandTask_ = g_pIKinemaSolver->addPositionTask("RightHandThumb");
//    headTask_ = g_pIKinemaSolver->addPositionTask("Head");
//    leftFootTask_ = g_pIKinemaSolver->addPositionTask("LeftFoot");
//    rightFootTask_ = g_pIKinemaSolver->addPositionTask("RightFoot");
//    if (!leftHandTask_ || !rightHandTask_ || !headTask_ || !leftFootTask_ || !rightFootTask_)
//    {
//        std::cout << "Error creating Position task for segment\n";
//        return;
//    }
//}
//
//#endif
//
///*
// * Draws a cylinder along axis
// */
//void DrawCylinder(VectorR3 axis, float size)
//{
//    glPushMatrix();
//    axis.Normalize();
//    double angle = acos(axis.Dot(VectorR3::UnitZ));
//    axis = axis * VectorR3::UnitZ;
//    glRotatef(-angle * RadiansToDegrees, axis.x, axis.y, axis.z);
//    glTranslatef(0.0f, 0.0f, -size / 2.0f);
//    glutSolidCylinder(0.03f, size, 12, 12);
//    glPopMatrix();
//}