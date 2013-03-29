#include "GLWindow.h"
#include <GLUtils/GLUtils.h>
#include <GLUtils/GLUITopLevelWindow.h>
#include <GLUtils/GLCamera.h>
#include <include/glut.h>
#include "Globals.h"

#include <iostream>
using namespace std;

GLWindow::GLWindow(int x, int y, int w, int h,int mode) :
	_glInitialized(false),
	_drawAxes(true),
	_drawGround(true),
	_cameraControl(true),
	_oldMouseX(-1),
	_oldMouseY(-1)
{
	_gluiTopLevelWindow = new GLUITopLevelWindow(w, h);

	_camera = new GLCamera();
	_camera->setTarget(Point3d(0, 1, 0));
	//_camera->setTarget(Point3d(0, 0, 0));

	//_camera->translateTarget(Vector3d(-2, 0, 5));
    //_camera->translateCamDistance(-10.0f);
	if(mode==1)
	{
	Vector3d camPos(-112.0f, 64.0f, 100.0f);
	_camera->translateCamDistance(-camPos.length());
	_camera->modifyRotations(Vector3d(-0.18, 3.141592/2.0, 0));
	}
	else if(mode==2)
	{
	Vector3d camPos(-150.0f, 100.0f, 0.0f);
	//Vector3d camPos(100.0f, 1000.0f, 100.0f);
	//cout << "cam pos length" << camPos.length() << endl;
	_camera->translateCamDistance(-camPos.length());
	Vector3d target(0,50,0);
	_camera->translateTarget(target);
	}
	else if(mode==3)
	{
		Vector3d camPos(-112.0f, 64.0f, 100.0f);
	_camera->translateCamDistance(-camPos.length());
	Vector3d target(0,50,0);
	_camera->translateTarget(target);
	//_camera->modifyRotations(Vector3d(-0.18, 3.141592/2, 0));
	}

	if(mode==1) _drawMaze=false;
	else if(mode==2 || mode==3) _drawMaze=true;
	GLUtils::init();
	

	//_camera->modifyRotations(Vector3d(-0.18, -3.141592/2.0, 0));
	//_camera->modifyRotations(Vector3d(-0.18, 3.141592/2, 0));
}

GLWindow::~GLWindow() {
	_drawables.clear();
	
}

void GLWindow::destroy() 
{
	_drawables.clear();
	
}

void GLWindow::onInitGL() {
	// initialize OpenGL for use in the window.
	glClearColor(0, 0, 0, 1);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_BLEND);

	// turn on anti aliasing
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glClearColor(0, 0, 0, 1);

	GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
	GLfloat diffuse0[] = {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat specular[] = {0, 0, 0, 0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);

	glEnable(GL_LIGHTING);

	GLfloat light0_position[] = {50.0f, 10.0f, 200.0f, 0.0f};
	GLfloat light1_position[] = {200.0f, 10.0f, -200.0f, 0.0f};
	GLfloat light2_position[] = {-200.0f, 10.0f, -200.0f, 0.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
}

void GLWindow::drawAxes() {
	glPushMatrix();
	{
		glLoadIdentity();
		glTranslated(-3, -2.0, -6.0);

		glRotated(-180 / PI * _camera->getRotations().x, 1, 0, 0);
		glRotated(-180 / PI * _camera->getRotations().y, 0, 1, 0);
		glRotated(-180 / PI * _camera->getRotations().z, 0, 0, 1);

		glDisable(GL_DEPTH_TEST);
		{
			GLUtils::drawAxes(0.5);
		}
		glEnable(GL_DEPTH_TEST);
	}
	glPopMatrix();
}

void GLWindow::onResize(int w, int h) {
	_gluiTopLevelWindow->setSize(w, h);
}

void GLWindow::draw(GaitType _currGaitType, double x, double y, double z) {
	if (!_glInitialized) {
		this->onInitGL();
		_glInitialized = true;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// setup the 3D projection matrix
	GLUISize size = _gluiTopLevelWindow->getSize();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0, size.width/float(size.height), 0.1, 150.0);
	gluPerspective(45.0, size.width/float(size.height), 0.1, 1000.0);

	// apply the camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	_camera->applyCameraTransformations();

	if (_drawGround) {
		//GLUtils::drawGround(50, 5, 98);
		GLUtils::drawGround(1000000, 40, 98,_drawMaze,x,y,z);
	}

	if (_drawAxes) {
		this->drawAxes();
	}

	//GLUtils::drawWireFrameBox(Point3d(0, 0, 0), Point3d(0.25, 0.1, 0.5));
	//GLUtils::drawWireFrameBox(Point3d(1, 0, 0), Point3d(1.25, 0.1, 0.5));

	Drawables::iterator i;
	for (i = _drawables.begin(); i != _drawables.end(); ++i) {
		(*i)->draw();
	}

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-1.0f);

	glColor3d(1.0,1.0,1.0);
	glRasterPos2f(0.3f, 0.4f);

	//GLUtils::gprintf("FPS: %7.2lf (processing: %7.2lf %%)\n",  0, 0);
	string types[28]={"Walk","Walk Back","Run","Jump", "Tiptoe","Tiptoe Back","","","Limp","Limp Back"," ","Jog","","Cat Walk","Moon Walk",
	"Hop","Tap Dance","Spin","Skate","Duck","Kick","March","Side Walk","Sneak","Split","Sit & Stand","Side Jump","Bridge"};

	GLUtils::gprintf("Current Gait Type: %s",types[_currGaitType].c_str());

	glPopMatrix();

	// draw GLUI
	//_gluiTopLevelWindow->refresh();

	glutSwapBuffers();
}

int GLWindow::onMouseEvent(int evt, int button, int x, int y) {
	Vector3d v;
	TransformationMatrix camToWorld;

	if (evt == MOUSE_DOWN || evt == MOUSE_UP) {
		_mouseButtonPressed = button;
	}

	// otherwise it probably is a mouse event
	if (evt == MOUSE_DOWN || evt == MOUSE_UP || evt == MOUSE_DRAG) {
		//bool processed = false;

		if (evt == MOUSE_DRAG) {
			switch (_mouseButtonPressed) {
				case MOUSE_RBUTTON:
				{
					//const float s = 200.0;
					const float s = 20.0;

					// the translation part depends on the camera orientation, so figure it out properly.
					v.x = (_oldMouseX - x) / s;
					v.y = -(_oldMouseY - y) / s;
					v.z = 0;

					// the target is specified in world coordinates, but it is more intuitive if
					// v above is expressed in camera coordinates, so we need to switch it up.
					camToWorld.setToInverseCoordFrameTransformationOf(_camera->getWorldToCam());
					v = camToWorld * v;

					_camera->translateTarget(v);
					break;
				}
				case MOUSE_LBUTTON:
				{
					const float a = 100.0;

					double dx = (_oldMouseX - x) / a;
					double dy = (_oldMouseY - y) / a;

					_camera->modifyRotations(Vector3d(dy, dx, 0));

					break;
				}
				case MOUSE_MBUTTON:
				{
					//const float t = 200.0;
					const float t = 20.0;

					_camera->translateCamDistance((_oldMouseY - y) / t);
					break;
				}
			}
		}

		_oldMouseX = x;
		_oldMouseY = y;

		return 1;
	}

	return 0;
}