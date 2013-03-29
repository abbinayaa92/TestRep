#pragma once
#include <vector>

class GLUITopLevelWindow;
class GLCamera;
class Vector3d;
class Point3d;

class Drawable {
public:
	virtual void draw() = 0;
};

class GLWindow
{
public:
	GLWindow(int x, int y, int w, int h);
	~GLWindow();
	void onInitGL();
	void onResize(int w, int h);

	void draw();
	void drawAxes();
	void drawShadows();
	
	virtual int onMouseEvent(int evt, int button, int x, int y);
	virtual int onKeyEvent(int key) { return 0; };

	void onStart();
	void onStop();

	void setCameraRotation(const Vector3d& newRot);
	void setCameraTarget(const Point3d& newTarget);
	GLCamera *getCamera() { return _camera; };
	void addDrawable(Drawable *d) { _drawables.push_back(d); };

protected:
	GLUITopLevelWindow *_gluiTopLevelWindow;
	GLCamera *_camera;
	bool _glInitialized;
	bool _drawAxes;
	bool _drawGround;
	bool _cameraControl;
	
	int _oldMouseX;
	int _oldMouseY;
	int _mouseButtonPressed;

	typedef std::vector<Drawable*> Drawables;
	Drawables _drawables;	
};
