#include ".\glutils.h"
#include <MathLib/Quaternion.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <iosfwd>
#include <fstream>
#include <Utils/Image.h>
#include <Utils/BMPIO.h>
#include <vector>
#include <Include/glut.h>

#include "GLTexture.h"

//extern unsigned int fontBaseList;
using namespace std;

 ofstream outputFile;
GLTexture* groundTexture = NULL;
GLTexture* mazeTexture = NULL;
GLTexture* portalTexture = NULL;
GLTexture* iceTexture = NULL;
GLTexture* fireTexture = NULL;
GLTexture* JfireTexture = NULL;
GLTexture* rampTexture = NULL;
GLTexture* waterTexture = NULL;
GLTexture* pebbleTexture = NULL;
GLTexture* boardTexture = NULL;
GLTexture* boardTextureJump = NULL;
GLTexture* boardTextureSneak = NULL;
GLTexture* boardTextureSidewalk = NULL;
GLTexture* boardTextureMoonwalk = NULL;
GLTexture* boardTextureSidejump = NULL;
GLTexture* boardTextureDuck = NULL;
GLTexture* boardTextureSkate = NULL;
GLTexture* boardTextureCatwalk = NULL;
GLTexture* boardTextureHop = NULL;
GLTexture* boardTextureRun = NULL;
GLTexture* boardTextureJog = NULL;
GLTexture* boardTextureLimp = NULL;
GLTexture* boardTextureTiptoe = NULL;

GLUtils::GLUtils(void)
{
	
}

GLUtils::~GLUtils(void)
{
}

void GLUtils::init()
{
groundTexture = NULL;
mazeTexture = NULL;
portalTexture = NULL;
iceTexture = NULL;
fireTexture = NULL;
JfireTexture = NULL;
 rampTexture = NULL;
 waterTexture = NULL;
 pebbleTexture = NULL;
 boardTexture = NULL;
 boardTextureJump = NULL;
 boardTextureSneak = NULL;
 boardTextureSidewalk = NULL;
 boardTextureMoonwalk = NULL;
 boardTextureSidejump = NULL;
 boardTextureDuck = NULL;
 boardTextureSkate = NULL;
 boardTextureCatwalk = NULL;
 boardTextureHop = NULL;
 boardTextureRun = NULL;
 boardTextureJog = NULL;
 boardTextureLimp = NULL;
 boardTextureTiptoe = NULL;
}
void GLUtils::gprintf(const char *fmt, ...){
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	int len = (int) strlen(text);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
}



/**
	This method draws a wireframe cube that is defined by the two 3d points
*/
void GLUtils::drawWireFrameBox(Point3d min, Point3d max){
	//now draw the cube that is defined by these two points...
	glBegin(GL_LINES);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);

		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);

		glVertex3d(max.x, max.y, min.z);
		glVertex3d(min.x, max.y, min.z);

		glVertex3d(min.x, max.y, min.z);
		glVertex3d(min.x, min.y, min.z);



		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, max.y, min.z);

		glVertex3d(min.x, max.y, min.z);
		glVertex3d(min.x, max.y, max.z);

		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, min.y, max.z);

		glVertex3d(min.x, min.y, max.z);
		glVertex3d(min.x, min.y, min.z);

		glVertex3d(min.x, min.y, max.z);
		glVertex3d(max.x, min.y, max.z);

		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);

		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);

		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, min.y, max.z);

		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);

		glVertex3d(max.x, max.y, min.z);
		glVertex3d(max.x, max.y, max.z);

		glVertex3d(max.x, max.y, max.z);
		glVertex3d(max.x, min.y, max.z);

		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, min.y, min.z);

	glEnd();
}

/**
	This method draws a box cube that is defined by the two 3d points
*/
void GLUtils::drawBox(Point3d min, Point3d max){
	//now draw the cube that is defined by these two points...
	glBegin(GL_QUADS);
		glNormal3d(0, 0, -1);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(min.x, max.y, min.z);

		glNormal3d(0, 0, 1);
		glVertex3d(min.x, min.y, max.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);

		glNormal3d(0, -1, 0);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(min.x, min.y, max.z);

		glNormal3d(0, 1, 0);
		glVertex3d(min.x, max.y, min.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);

		glNormal3d(-1, 0, 0);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, min.y, max.z);
		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, max.y, min.z);

		glNormal3d(1, 0, 0);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(max.x, max.y, min.z);

	glEnd();
/*
	// sides
	glBegin (GL_TRIANGLE_STRIP);
	glNormal3d (-1,0,0);
	glVertex3d (min.x,min.y,min.z);
	glVertex3d (min.x,min.y,max.z);
	glVertex3d (min.x,max.y,min.z);
	glVertex3d (min.x,max.y,max.z);
	glNormal3d (0,1,0);
	glVertex3d (max.x,max.y,min.z);
	glVertex3d (max.x,max.y,max.z);
	glNormal3d (1,0,0);
	glVertex3d (max.x,min.y,min.z);
	glVertex3d (max.x,min.y,max.z);
	glNormal3d (0,-1,0);
	glVertex3d (min.x,min.y,min.z);
	glVertex3d (min.x,min.y,max.z);
	glEnd();

	// top face
	glBegin (GL_TRIANGLE_FAN);
	glNormal3d (0,0,1);
	glVertex3d (min.x,min.y,max.z);
	glVertex3d (max.x,min.y,max.z);
	glVertex3d (max.x,max.y,max.z);
	glVertex3d (min.x,max.y,max.z);
	glEnd();

	// bottom face
	glBegin (GL_TRIANGLE_FAN);
	glNormal3d (0,0,-1);
	glVertex3d (min.x,min.y,min.z);
	glVertex3d (min.x,max.y,min.z);
	glVertex3d (max.x,max.y,min.z);
	glVertex3d (max.x,min.y,min.z);
	glEnd();
*/
}


/**
	This method draws a sphere of radius r, centered at the origin. It uses nrPoints for the approximation.
*/
void GLUtils::drawSphere(Point3d origin, double r, int nrPoints){
	int j;
	double i, angle = PI/nrPoints;
	//this is the normal vector
	Vector3d n, v;

	glPushMatrix();
	glTranslated(origin.x, origin.y, origin.z);

	Point3d p, q;
	
	glBegin(GL_QUAD_STRIP);
		p.x = r*cos(-PI/2);
		p.y = r*sin(-PI/2);
		p.z = 0;
		for (i=-PI/2+angle;i<=PI/2;i+=angle)
		{
			q.x = r*cos(i);
			q.y = r*sin(i);
			q.z = 0;
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(p).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(p.x,p.y,p.z);
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(q).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(q.x,q.y,q.z);

			for (j=0;j<=2*nrPoints;j++){
				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(q.x * cos(j * angle), q.y, q.x * sin(j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(v.x, v.y, v.z);

				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(p.x * cos(j * angle), p.y, p.x * sin(j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(v.x, v.y, v.z);
			}
			p = q;
		}

	glEnd();
	glPopMatrix();
}



/**
	This method draws a system of coordinate axes of length n
*/
void GLUtils::drawAxes(double n){
	glBegin(GL_LINES);								//draw the axis...
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3d(0,0,0);							//X axis
		glVertex3d(n,0,0);

		glColor3f(0.0f,1.0f,0.0f);
		glVertex3d(0,0,0);							//Y axis
		glVertex3d(0,n,0);

		glColor3f(0.0f,0.0f,1.0f);
		glVertex3d(0,0,0);							//Z axis
		glVertex3d(0,0,n);

	glEnd();
}

/**
	This method draws a sphere of radius r, centered at the origin. It uses nrPoints for the approximation.
*/
void GLUtils::drawCapsule(double r, Vector3d dir, Point3d org, int nrPoints){
	Vector3d axis = Vector3d(0,1,0);
	//we first need a rotation that gets dir to be aligned with the y-axis...
	Vector3d rotAxis = dir.unit().crossProductWith(axis);
	double rotAngle = asin(rotAxis.length());
	if (dir.dotProductWith(axis) < 0){
		org += dir;
		dir *= -1;
	}
	rotAxis.toUnit();

	Quaternion rot = Quaternion::getRotationQuaternion(-rotAngle, rotAxis);
	glPushMatrix();
	glTranslated(org.x, org.y, org.z);
	double rotData[16];
	TransformationMatrix rotM;
	rot.getRotationMatrix(&rotM);
	rotM.getOGLValues(rotData);
	glMultMatrixd(rotData);

	org.x = 0;
	org.y = 0;
	org.z = 0;


	//we'll start out by getting a vector that is perpendicular to the given vector.
	Vector3d n;


	//try to get a vector that is perpendicular to r.
	n = Vector3d(1,0,0);
	dir = axis * dir.length();

	(n.toUnit()) *= r;
	glBegin(GL_TRIANGLE_STRIP);

	//now, we we'll procede by rotating the vector n around v, and create the cylinder that way.
	for (int i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(2*i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
		Point3d p2 = org + dir + p;
		glVertex3d(p2.x, p2.y, p2.z);
	}
	glEnd();

	//now we'll draw a half sphere...
	Point3d p, q;
	double angle = PI/nrPoints;
		p.x = r*cos(-PI/2);
		p.y = r*sin(-PI/2);
		p.z = 0;
		for (int i=nrPoints/2;i>=0;i--){
			glBegin(GL_QUAD_STRIP);
			q.x = r*cos(-i*angle);
			q.y = r*sin(-i*angle);
			q.z = 0;
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(p).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x+p.x,org.y + p.y, org.z + p.z);
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(q).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x + q.x,org.y + q.y,org.z + q.z);

			for (int j=0;j<=nrPoints;j++){
				//make sure we compute the normal as well as the node coordinates
				Vector3d v = Vector3d(q.x * cos(2*j * angle), q.y, q.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);

				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(p.x * cos(2*j * angle), p.y, p.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);
			}
			p = q;
			glEnd();
		}	

	org += dir;

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//and now draw the other half - horrible, quick research code...
	
		p.x = r*cos(PI/2);
		p.y = r*sin(PI/2);
		p.z = 0;
		for (int i=nrPoints/2;i>=0;i--){
			glBegin(GL_QUAD_STRIP);
			q.x = r*cos(i*angle);
			q.y = r*sin(i*angle);
			q.z = 0;
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(p).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x+p.x,org.y + p.y, org.z + p.z);
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(q).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x + q.x,org.y + q.y,org.z + q.z);

			for (int j=0;j<=nrPoints;j++){
				//make sure we compute the normal as well as the node coordinates
				Vector3d v = Vector3d(q.x * cos(2*j * angle), q.y, q.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);

				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(p.x * cos(2*j * angle), p.y, p.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);
			}
			p = q;
			glEnd();
		}
	

	glPopMatrix();

}


/**
	This method draws a disc of radius r, centered on point org with normal norm
*/
void GLUtils::drawDisk(double r, Point3d org, Vector3d norm, int nrPoints) {
	Vector3d n = norm;
	n.toUnit();
	Vector3d x = Vector3d(0,1,0).crossProductWith(n);
	Vector3d y = Vector3d(1,0,0).crossProductWith(n);
	if( x.length() < 0.01 )
		x = Vector3d(0,0,1).crossProductWith(n);
	else if( y.length() < 0.01 )
		y = Vector3d(0,0,1).crossProductWith(n);
	x.toUnit();
	y.toUnit();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d( n.x, n.y, n.z );
	glVertex3d( org.x, org.y, org.z );
	for( int i=0; i<=nrPoints; ++i ) {
		double theta = (i%nrPoints) / (double)nrPoints * 2.0 * 3.14159265;
		Point3d p = org + x * cos(theta) * r + y * sin(theta) * r;
		glVertex3d( p.x, p.y, p.z );
	}
	glEnd();
}


/**
	This method draws a cylinder of thinkness r, along the vector dir.
*/
void GLUtils::drawCylinder(double r, Vector3d v, Point3d org, int nrPoints, int h){
	//we'll start out by getting a vector that is perpendicular to the given vector.
	Vector3d n;
	Vector3d axis = v;
	axis.toUnit();
	int i;
	//try to get a vector that is not colinear to v.
	if (v.x != 0 || v.y != 0)
		n = Vector3d(v.x, v.y, v.z + 1);
	else if (v.y != 0 || v.z != 0)
		n = Vector3d(v.x, v.y+1, v.z);
	else
		n = Vector3d(v.x+1, v.y, v.z);

	n = n.crossProductWith(v);

	if (IS_ZERO(v.length()) || IS_ZERO(n.length()))
		return;
	(n.toUnit()) *= r;

	glBegin(GL_TRIANGLE_STRIP);

	//now, we we'll procede by rotating the vector n around v, and create the cylinder that way.
	for(int j=0;j<h;j++)
	{
	for (i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x+j, p1.y, p1.z);
		Point3d p2 = org + v + p;
		glVertex3d(p2.x+j, p2.y, p2.z);
	}
	}
	
	glEnd();

}


/**
	This method draws a cone of radius r, along the vector dir, with the center of its base at org.
*/
void GLUtils::drawCone(double r, Vector3d v, Point3d org, int nrPoints){
	//we'll start out by getting a vector that is perpendicular to the given vector.
	Vector3d n;
	Vector3d axis = v;
	axis.toUnit();
	int i;
	//try to get a vector that is not colinear to v.
	if (v.x != 0 || v.y != 0)
		n = Vector3d(v.x, v.y, v.z + 1);
	else if (v.y != 0 || v.z != 0)
		n = Vector3d(v.x, v.y+1, v.z);
	else
		n = Vector3d(v.x+1, v.y, v.z);

	n = n.crossProductWith(v);

	if (IS_ZERO(v.length()) || IS_ZERO(n.length()))
		return;
	(n.toUnit()) *= r;
	glBegin(GL_TRIANGLE_FAN);

	Point3d p2 = org + v;
	glNormal3d(axis.x, axis.y, axis.z);
	glVertex3d(p2.x, p2.y, p2.z);


	//now, we we'll procede by rotating the vector n around v, and creating the cone that way.
	for (i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(2*i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
	}
	glEnd();


	//now we need to draw the bottom of the cone.
	glBegin(GL_POLYGON);

	//now, we we'll procede by rotating the vector n around v, and creating the cone that way.
	for (i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(2*i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
	}
	glEnd();
}

/**
	This method is used to draw an arrow, in the direction pointed by the vector dir, originating at the point org.
	The thickness of the cylinder used, as well as the length of the arrow head are estimated based on the length of the direction vector.
*/
void GLUtils::drawArrow(Vector3d dir, Point3d org){
	drawCylinder(dir.length()/20, dir*0.8, org);
	drawCone(dir.length()/10, dir/4, org+dir*0.75);
}


/**
	This method is used to draw an arrow, in the direction pointed by the vector dir, originating at the point org.
	The thickness of the cylinder used, as well as the length of the arrow head are estimated based on the length of the direction vector.
*/
void GLUtils::drawArrow(Vector3d dir, Point3d org, double scale){
	drawCylinder(scale, dir*0.8, org);
	drawCone(2*scale, dir/(dir.length())*scale*5, org+dir*0.75);
}


/**
	This method will take a screenshot of the current scene and it will save it to a file with the given name
*/
void GLUtils::saveScreenShot(char* fileName, int x, int y, int width, int height){
	if (fileName == NULL)
		return;
	std::ofstream out(fileName, std::ofstream::binary);
	if(!out)
		return;

	glReadBuffer(GL_BACK);

	Image *img = new Image(3, width, height, NULL);

	glReadPixels(x , y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img->getDataPointer());

	BMPIO b(fileName);
	b.writeToFile(img);

	delete img;
}


/**
	This method draws a checker-board pattern (centered at the origin, parallel to the XZ plane) based on the current parameters:
	int n - the number of squares (we'll assume square checkerboards for now!)
	double w - the size of each square int the checkerboard
	double h - the height at which the checkerboard is to be drawn
*/
void GLUtils::drawCheckerboard(int n, double w, double h){
#define BRIGHT 0.5f
#define DARK 0.2f
	double start = n/2.0 * w;
	for (int i=0;i<n;i++){
		for (int j=0;j<n;j++){
			if ((i+j)%2 == 1)
				glColor3d(BRIGHT, BRIGHT, BRIGHT);
			else
				glColor3d(DARK, DARK, DARK);

			glBegin(GL_QUADS);
				glVertex3d(-start+i*w,h,-start+j*w);
				glVertex3d(-start+i*w+w,h,-start+j*w);
				glVertex3d(-start+i*w+w,h,-start+j*w+w);
				glVertex3d(-start+i*w,h,-start+j*w+w);
			glEnd();
		}
	}
}


/**
	This method draws a grid pattern (centered at the origin, parallel to the XZ plane) based on the current parameters:
	int n - the number of squares (we'll assume square grid for now!)
	double w - the size of each square in the grid
	double h - the height at which the grid is to be drawn
*/
void GLUtils::drawGrid(int n, double w, double h){
	glColor3d(0.5f, 0.5f, 0.5f);
	double start = n/2.0 * w;
	glLineWidth(0.5);
	for (int i=1;i<n;i++){
		glBegin(GL_LINES);
			glVertex3d(-start+i*w,h,-start);
			glVertex3d(-start+i*w,h,start);

			glVertex3d(-start,h,-start+i*w);
			glVertex3d(start, h,-start+i*w);
		glEnd();
	}
	glLineWidth(1);

//draw a thicker line in the middle now, 
	glColor3d(0.7f, 0.7f, 0.7f);
	glLineWidth(2);
	glBegin(GL_LINES);
		glVertex3d(0,h,-start);
		glVertex3d(0,h,start);
		glVertex3d(-start,h,0);
		glVertex3d(start, h,0);
	glEnd();
	glLineWidth(1);

}


/**
	Prints the openGL errors
*/
int GLUtils::printOglError(char *file, int line){
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR){
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void applyColor(double x, double z, double spotRadius) {

	glColor3d(1, 1, 1);
	//double fadeOff = 20;
	double fadeOff = 200;

	double dist2 = x*x + z*z;
	if( dist2 > spotRadius*spotRadius ) {
		double diff = sqrt(dist2) - spotRadius;
		double color = (fadeOff-diff)/fadeOff;
		glColor3d( color, color, color );
	}
}


void GLUtils::drawsquare1(double x, double y, double z) {  

glPushMatrix();

glEnable(GL_TEXTURE_2D);
mazeTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x, y, z); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x+200, y, z); // The bottom right corner  
glEnd();  

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+200, y, z); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z+5); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x+200, y, z+5); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+200, y, z+5); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z+5); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x, y+50, z+5); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x, y, z+5); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x, y, z+5); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x, y+50, z+5); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x, y+50, z); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x, y, z); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x, y+50, z+5); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x+200, y+50, z+5); // The bottom right corner  
glEnd();
glDisable(GL_TEXTURE_2D);
glPopMatrix();
//outputFile<<"{"<<x<<","<<y+50<<","<<z+5<<","<<x+200<<","<<y+50<<","<<z+5<<"},"<<endl;
}  

void GLUtils::drawsquare2(double x, double y, double z) {  

glPushMatrix();

glEnable(GL_TEXTURE_2D);
mazeTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+200, y, z); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z+200); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+200, y, z+200); // The bottom right corner  
glEnd();

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+200, y, z+200); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+195, y+50, z+200); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+195, y, z+200); // The bottom right corner  
glEnd();

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+195, y, z+200); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x+195, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+195, y+50, z); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+195, y, z); // The bottom right corner  
glEnd();

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+195, y, z); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x+195, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+200, y, z); // The bottom right corner  
glEnd();

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+195, y+50, z); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z+200); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+195, y+50, z+200); // The bottom right corner  
glEnd();
glDisable(GL_TEXTURE_2D);
glPopMatrix();
//outputFile<<"{"<<x+195<<","<<y+50<<","<<z<<","<<x+195<<","<<y+50<<","<<z+200<<"},"<<endl;
} 

void GLUtils::drawsquare3(double x, double y, double z) {  

glPushMatrix();
 
glEnable(GL_TEXTURE_2D);
mazeTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0,0);
glVertex3f(x+200, y, z+200); // The bottom left corner  
glTexCoord2d(0,2);
glVertex3f(x+200, y+50, z+200); // The top left corner 
glTexCoord2d(2,2);
glVertex3f(x, y+50, z+200); // The top right corner  
glTexCoord2d(2,0);
glVertex3f(x, y, z+200); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+200, y, z+200); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z+195); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x+200, y, z+195); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+200, y, z+195); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z+195); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x, y+50, z+195); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x, y, z+195); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x, y, z+195); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x, y+50, z+195); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x, y+50, z+200); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x, y, z+200); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x, y+50, z+195); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+50, z+200); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x+200, y+50, z+195); // The bottom right corner  
glEnd();
glDisable(GL_TEXTURE_2D);
glPopMatrix();

//outputFile<<"{"<<x<<","<<y+50<<","<<z+195<<","<<x+200<<","<<y+50<<","<<z+195<<"},"<<endl;
} 

void GLUtils::drawsquare4(double x, double y, double z) {  

glPushMatrix();  
glEnable(GL_TEXTURE_2D);
mazeTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0,0); 
glVertex3f(x, y, z+200); // The bottom left corner  
glTexCoord2d(0,2);
glVertex3f(x, y+50, z+200); // The top left corner  
glTexCoord2d(2,2);
glVertex3f(x, y+50, z); // The top right corner  
glTexCoord2d(2,0);
glVertex3f(x, y, z); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x, y, z+200); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+5, y+50, z+200); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+5, y, z+200); // The bottom right corner  
glEnd();

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+5, y, z+200); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x+5, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+5, y+50, z); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+5, y, z); // The bottom right corner  
glEnd();

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+5, y, z); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x+5, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x, y+50, z); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x, y, z); // The bottom right corner  
glEnd();

glBegin(GL_POLYGON); // Start drawing a point primitive  
glTexCoord2d(0, 0);
glVertex3f(x+5, y+50, z); // The bottom left corner 
glTexCoord2d(0, 2);
glVertex3f(x, y+50, z); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x, y+50, z+200); // The top right corner 
glTexCoord2d(2,0);
glVertex3f(x+5, y+50, z+200); // The bottom right corner  
glEnd();
glDisable(GL_TEXTURE_2D);
glPopMatrix();

//outputFile<<"{"<<x+5<<","<<y+50<<","<<z<<","<<x+5<<","<<y+50<<","<<z+200<<"},"<<endl;
} 

void drawportal1(double x, double y, double z, int motion)
{
glPushMatrix();
glEnable(GL_TEXTURE_2D);
portalTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+5, y+50, z); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+5, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+100, y+100, z+200); // The top right corner 
glTexCoord2d(2, 0);
glVertex3f(x+100, y+100, z); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+195, y+50, z); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+195, y+50, z+200); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+100, y+100, z+200); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x+100, y+100, z); // The bottom right corner  
glEnd(); 
glDisable(GL_TEXTURE_2D);

glLineWidth(5.0f);
glEnable(GL_TEXTURE_2D);
if(motion == 23)
	boardTextureSneak->activate();
else if(motion == 22)
	boardTextureSidewalk->activate();
else if(motion == 14)
	boardTextureMoonwalk->activate();
else if(motion == 26)
	boardTextureSidejump->activate();
else if(motion == 19)
	boardTextureDuck->activate();
else if(motion == 18)
	boardTextureSkate->activate();
else if(motion == 3)
	boardTextureJump->activate();
else if(motion == 8)
	boardTextureLimp->activate();
else if(motion == 13)
	boardTextureCatwalk->activate();
else if(motion == 15)
	boardTextureHop->activate();
else if(motion == 2)
	boardTextureRun->activate();
else if(motion == 11)
	boardTextureJog->activate();
else if(motion == 4)
	boardTextureTiptoe->activate();
else
	boardTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 1);//
glVertex3f(x+112, y+72, z); // The bottom left corner  
glTexCoord2d(1, 1);//
glVertex3f(x+88, y+72, z); // The top left corner  
glTexCoord2d(1, 0);//
glVertex3f(x+88, y+62, z); // The top right corner  
glTexCoord2d(0, 0);//
glVertex3f(x+112, y+62, z); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(1, 1);
glVertex3f(x+112, y+72, z+200); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+88, y+72, z+200); // The top left corner  
glTexCoord2d(0, 0);
glVertex3f(x+88, y+62, z+200); // The top right corner  
glTexCoord2d(1, 0);
glVertex3f(x+112, y+62, z+200); // The bottom right corner  
glEnd(); 
glDisable(GL_TEXTURE_2D);

glColor3f(0,0,0);
glBegin(GL_LINES);
glVertex3f(x+120, y+90, z-0.01);
glVertex3f(x+110, y+70, z-0.01);
glEnd();
glBegin(GL_LINES);
glVertex3f(x+80, y+90, z-0.01);
glVertex3f(x+90, y+70, z-0.01);
glEnd();
glColor3f(1,1,1);
glPopMatrix();



}

void drawportal2(double x, double y, double z, int motion)
{

glPushMatrix();

glEnable(GL_TEXTURE_2D);
portalTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x, y+50, z+5); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z+5); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+100, z+100); // The top right corner 
glTexCoord2d(2, 0);
glVertex3f(x, y+100, z+100); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x, y+50, z+195); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+200, y+50, z+195); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+200, y+100, z+100); // The top right corner  
glTexCoord2d(2, 0);
glVertex3f(x, y+100, z+100); // The bottom right corner  
glEnd(); 
glDisable(GL_TEXTURE_2D);

glLineWidth(5.0f);
glEnable(GL_TEXTURE_2D);
if(motion == 23)
	boardTextureSneak->activate();
else if(motion == 22)
	boardTextureSidewalk->activate();
else if(motion == 14)
	boardTextureMoonwalk->activate();
else if(motion == 26)
	boardTextureSidejump->activate();
else if(motion == 19)
	boardTextureDuck->activate();
else if(motion == 18)
	boardTextureSkate->activate();
else if(motion == 3)
	boardTextureJump->activate();
else if(motion == 8)
	boardTextureLimp->activate();
else if(motion == 13)
	boardTextureCatwalk->activate();
else if(motion == 15)
	boardTextureHop->activate();
else if(motion == 2)
	boardTextureRun->activate();
else if(motion == 11)
	boardTextureJog->activate();
else if(motion == 4)
	boardTextureTiptoe->activate();
else
	boardTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(1, 1);
glVertex3f(x, y+72, z+112); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x, y+72, z+88); // The top left corner  
glTexCoord2d(0, 0);
glVertex3f(x, y+62, z+88); // The top right corner  
glTexCoord2d(1, 0);
glVertex3f(x, y+62, z+112); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 1);
glVertex3f(x+200, y+72, z+112); // The bottom left corner  
glTexCoord2d(1, 1);
glVertex3f(x+200, y+72, z+88); // The top left corner  
glTexCoord2d(1, 0);
glVertex3f(x+200, y+62, z+88); // The top right corner  
glTexCoord2d(0, 0);
glVertex3f(x+200, y+62, z+112); // The bottom right corner  
glEnd(); 
glDisable(GL_TEXTURE_2D);

glColor3f(0,0,0);
glBegin(GL_LINES);
glVertex3f(x-0.01, y+90, z+120);
glVertex3f(x-0.01, y+70, z+110);
glEnd();
glBegin(GL_LINES);
glVertex3f(x-0.01, y+90, z+80);
glVertex3f(x-0.01, y+70, z+90);
glEnd();
glColor3f(1,1,1);
glPopMatrix();

}

void drawportal3(double x, double y,double z,int motion)
{

glPushMatrix();
 
glEnable(GL_TEXTURE_2D);
portalTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+5, y+100, z+5); // The bottom left corner  
glTexCoord2d(0, 2);
glVertex3f(x+195, y+100, z+5); // The top left corner  
glTexCoord2d(2, 2);
glVertex3f(x+195, y+100, z+195); // The top right corner 
glTexCoord2d(2, 0);
glVertex3f(x+5, y+100, z+195); // The bottom right corner  
glEnd(); 

// 2 triangles
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+100, y+100, z+5); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+195, y+50, z+5); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+195, y+100, z+5); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+100, y+100, z+5); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+100, y+100, z+5); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+5, y+50, z+5); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+5, y+100, z+5); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+100, y+100, z+5); // The bottom right corner  
glEnd(); 

//2 triangles
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+100, y+100, z+195); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+195, y+50, z+195); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+195, y+100, z+195); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+100, y+100, z+195); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+100, y+100, z+195); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+5, y+50, z+195); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+5, y+100, z+195); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+100, y+100, z+195); // The bottom right corner  
glEnd(); 

//z// 2 triangles
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+5, y+100, z+100); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+5, y+50, z+195); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+5, y+100, z+195); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+5, y+100, z+100); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+5, y+100, z+100); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+5, y+50, z+5); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+5, y+100, z+5); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+5, y+100, z+100); // The bottom right corner  
glEnd(); 

//2 triangles
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+195, y+100, z+100); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+195, y+50, z+195); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+195, y+100, z+195); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+195, y+100, z+100); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 0);
glVertex3f(x+195, y+100, z+100); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+195, y+50, z+5); // The top left corner  
glTexCoord2d(1, 1);
glVertex3f(x+195, y+100, z+5); // The top right corner 
glTexCoord2d(1, 0);
glVertex3f(x+195, y+100, z+100); // The bottom right corner  
glEnd(); 
glDisable(GL_TEXTURE_2D);


glEnable(GL_TEXTURE_2D);
if(motion == 23)
	boardTextureSneak->activate();
else if(motion == 22)
	boardTextureSidewalk->activate();
else if(motion == 14)
	boardTextureMoonwalk->activate();
else if(motion == 26)
	boardTextureSidejump->activate();
else if(motion == 19)
	boardTextureDuck->activate();
else if(motion == 18)
	boardTextureSkate->activate();
else if(motion == 3)
	boardTextureJump->activate();
else if(motion == 8)
	boardTextureLimp->activate();
else if(motion == 13)
	boardTextureCatwalk->activate();
else if(motion == 15)
	boardTextureHop->activate();
else if(motion == 2)
	boardTextureRun->activate();
else if(motion == 11)
	boardTextureJog->activate();
else if(motion == 4)
	boardTextureTiptoe->activate();
else
	boardTexture->activate();
glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(1, 1);
glVertex3f(x, y+72, z+112); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x, y+72, z+88); // The top left corner  
glTexCoord2d(0, 0);
glVertex3f(x, y+62, z+88); // The top right corner  
glTexCoord2d(1, 0);
glVertex3f(x, y+62, z+112); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 1);
glVertex3f(x+200, y+72, z+112); // The bottom left corner  
glTexCoord2d(1, 1);
glVertex3f(x+200, y+72, z+88); // The top left corner  
glTexCoord2d(1, 0);
glVertex3f(x+200, y+62, z+88); // The top right corner  
glTexCoord2d(0, 0);
glVertex3f(x+200, y+62, z+112); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(0, 1);//
glVertex3f(x+112, y+72, z); // The bottom left corner  
glTexCoord2d(1, 1);//
glVertex3f(x+88, y+72, z); // The top left corner  
glTexCoord2d(1, 0);//
glVertex3f(x+88, y+62, z); // The top right corner  
glTexCoord2d(0, 0);//
glVertex3f(x+112, y+62, z); // The bottom right corner  
glEnd(); 

glBegin(GL_POLYGON); // Start drawing a point primitive 
glTexCoord2d(1, 1);
glVertex3f(x+112, y+72, z+200); // The bottom left corner  
glTexCoord2d(0, 1);
glVertex3f(x+88, y+72, z+200); // The top left corner  
glTexCoord2d(0, 0);
glVertex3f(x+88, y+62, z+200); // The top right corner  
glTexCoord2d(1, 0);
glVertex3f(x+112, y+62, z+200); // The bottom right corner  
glEnd(); 
glDisable(GL_TEXTURE_2D);

glLineWidth(5.0f);
glColor3f(0,0,0);
glBegin(GL_LINES);
glVertex3f(x+120, y+90, z-0.01);
glVertex3f(x+110, y+70, z-0.01);
glEnd();
glBegin(GL_LINES);
glVertex3f(x+80, y+90, z-0.01);
glVertex3f(x+90, y+70, z-0.01);
glEnd();

glColor3f(0,0,0);
glBegin(GL_LINES);
glVertex3f(x-0.01, y+90, z+120);
glVertex3f(x-0.01, y+70, z+110);
glEnd();
glBegin(GL_LINES);
glVertex3f(x-0.01, y+90, z+80);
glVertex3f(x-0.01, y+70, z+90);
glEnd();
glColor3f(1,1,1);
glPopMatrix();
}

void drawcube(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	GLUtils::drawsquare2(x,y,z);
	GLUtils::drawsquare3(x,y,z);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawcube1(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare2(x,y,z);
	GLUtils::drawsquare3(x,y,z);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawcube2(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	GLUtils::drawsquare3(x,y,z);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawcube3(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	GLUtils::drawsquare2(x,y,z);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawcube4(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	GLUtils::drawsquare2(x,y,z);
	GLUtils::drawsquare3(x,y,z);
	glPopMatrix();
}

void drawcube5(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawcube6(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	GLUtils::drawsquare2(x,y,z);
	glPopMatrix();
}

void drawcube7(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare2(x,y,z);
	GLUtils::drawsquare3(x,y,z);
	glPopMatrix();
}

void drawcube8(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare3(x,y,z);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawcube9(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	GLUtils::drawsquare3(x,y,z);
	//drawportal2(x,y,z);
	glPopMatrix();
}

void drawcube10(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	
	GLUtils::drawsquare2(x,y,z);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawcube11(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare3(x,y,z);
	glPopMatrix();
}

void drawcube12(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare2(x,y,z);
	glPopMatrix();
}

void drawcube13(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare1(x,y,z);
	glPopMatrix();
}

void drawcube14(double x,double y,double z)
{
	glPushMatrix();
	//glColor3f(0, 0, 0);
	GLUtils::drawsquare4(x,y,z);
	glPopMatrix();
}

void drawice(double x, double y, double z)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	iceTexture->activate();
	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 0);
	glVertex3f(x, y, z); // The bottom left corner  
	glTexCoord2d(0, 1);
	glVertex3f(x+200, y, z); // The top left corner  
	glTexCoord2d(1, 1);
	glVertex3f(x+200, y, z+200); // The top right corner  
	glTexCoord2d(1, 0);
	glVertex3f(x, y, z+200); // The bottom right corner  
	glEnd();  
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}

void drawcatwalk(double x, double y, double z)
{
	glPushMatrix();
	//glEnable(GL_TEXTURE_2D);
	//waterTexture->activate();
	glColor3f(0,0,0);
	glBegin(GL_POLYGON); // Start drawing a point primitive  
	//glTexCoord2d(0, 0);
	glVertex3f(x, y, z); // The bottom left corner  
	//glTexCoord2d(0, 1);
	glVertex3f(x+200, y, z); // The top left corner  
	//glTexCoord2d(1, 1);
	glVertex3f(x+200, y, z+200); // The top right corner  
	//glTexCoord2d(1, 0);
	glVertex3f(x, y, z+200); // The bottom right corner  
	glEnd(); 
	glColor3f(1,1,1);
	//glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	rampTexture->activate();
	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 0);
	glVertex3f(x+80, y+0.1, z); // The bottom left corner  
	glTexCoord2d(0, 1);
	glVertex3f(x+120, y+0.1, z); // The top left corner  
	glTexCoord2d(1, 1);
	glVertex3f(x+120, y+0.1, z+200); // The top right corner  
	glTexCoord2d(1, 0);
	glVertex3f(x+80, y+0.1, z+200); // The bottom right corner  
	glEnd();  

	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 0);
	glVertex3f(x, y+0.1, z+80); // The bottom left corner  
	glTexCoord2d(0, 1);
	glVertex3f(x, y+0.1, z+120); // The top left corner  
	glTexCoord2d(1, 1);
	glVertex3f(x+200, y+0.1, z+120); // The top right corner  
	glTexCoord2d(1, 0);
	glVertex3f(x+200, y+0.1, z+80); // The bottom right corner  
	glEnd();  
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}

void drawside(double x, double y, double z)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	waterTexture->activate();
	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 0);
	glVertex3f(x, y, z); // The bottom left corner  
	glTexCoord2d(0, 1);
	glVertex3f(x+200, y, z); // The top left corner  
	glTexCoord2d(1, 1);
	glVertex3f(x+200, y, z+200); // The top right corner  
	glTexCoord2d(1, 0);
	glVertex3f(x, y, z+200); // The bottom right corner  
	glEnd(); 
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	pebbleTexture->activate();
	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 0);
	glVertex3f(x+90, y+0.1, z); // The bottom left corner  
	glTexCoord2d(0, 0.5);
	glVertex3f(x+110, y+0.1, z); // The top left corner  
	glTexCoord2d(2, 0.5);
	glVertex3f(x+110, y+0.1, z+200); // The top right corner  
	glTexCoord2d(2, 0);
	glVertex3f(x+90, y+0.1, z+200); // The bottom right corner  
	glEnd();  

	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 0);
	glVertex3f(x, y+0.1, z+90); // The bottom left corner  
	glTexCoord2d(0, 0.5);
	glVertex3f(x, y+0.1, z+110); // The top left corner  
	glTexCoord2d(2, 0.5);
	glVertex3f(x+200, y+0.1, z+110); // The top right corner  
	glTexCoord2d(2, 0);
	glVertex3f(x+200, y+0.1, z+90); // The bottom right corner  
	glEnd();  
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}


void drawjump(double x, double y, double z)
{
	glPushMatrix();
	glColor3f(0,0,0);
	//glBegin(GL_POLYGON); // Start drawing a point primitive  
	//glVertex3f(x, y, z); // The bottom left corner  
	//glVertex3f(x+200, y, z); // The top left corner  
	//glVertex3f(x+200, y, z+200); // The top right corner  
	//glVertex3f(x, y, z+200); // The bottom right corner  
	//glEnd(); 
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	
	fireTexture->activate();
	
	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 1);
	glVertex3f(x,y+0.1, z+30); // The bottom left corner  
	glTexCoord2d(2,1);
	glVertex3f(x+200, y+0.1, z+30); // The top left corner  
	glTexCoord2d(2, 0);
	glVertex3f(x+200, y+0.1, z+50); // The top right corner  
	glTexCoord2d(0, 0);
	glVertex3f(x, y+0.1, z+50); // The bottom right corner  
	glEnd(); 

	glBegin(GL_POLYGON); // Start drawing a point primitive  
	glTexCoord2d(0, 1);
	glVertex3f(x,y+0.1, z+130); // The bottom left corner  
	glTexCoord2d(2,1);
	glVertex3f(x+200, y+0.1, z+130); // The top left corner  
	glTexCoord2d(2, 0);
	glVertex3f(x+200, y+0.1, z+150); // The top right corner  
	glTexCoord2d(0, 0);
	glVertex3f(x, y+0.1, z+150); // The bottom right corner  
	glEnd();

	//for(int i=20;i<200;i+=80)
	//{
	//glBegin(GL_POLYGON); // Start drawing a point primitive  
	//glTexCoord2d(0, 1);
	//glVertex3f(x+70,y+0.1, z+i); // The bottom left corner  
	//glTexCoord2d(1,1);
	//glVertex3f(x+130, y+0.1, z+i); // The top left corner  
	//glTexCoord2d(1, 0);
	//glVertex3f(x+130, y+0.1, z+i+5); // The top right corner  
	//glTexCoord2d(0, 0);
	//glVertex3f(x+70, y+0.1, z+i+5); // The bottom right corner  
	//glEnd(); 
	//}

	//for(int i=50;i<160;i+=100)
	//{
	//glBegin(GL_POLYGON); // Start drawing a point primitive  
	//glTexCoord2d(0, 1);
	//glVertex3f(x+20, y+0.1, z+i); // The bottom left corner  
	//glTexCoord2d(1,1);
	//glVertex3f(x+70, y+0.1, z+i); // The top left corner  
	//glTexCoord2d(1, 0);
	//glVertex3f(x+70, y+0.1, z+i+5); // The top right corner  
	//glTexCoord2d(0, 0);
	//glVertex3f(x+20, y+0.1, z+i+5); // The bottom right corner  
	//glEnd(); 

	//glBegin(GL_POLYGON); // Start drawing a point primitive  
	//glTexCoord2d(0, 1);
	//glVertex3f(x+130, y+0.1, z+i); // The bottom left corner  
	//glTexCoord2d(1,1);
	//glVertex3f(x+180, y+0.1, z+i); // The top left corner  
	//glTexCoord2d(1, 0);
	//glVertex3f(x+180, y+0.1, z+i+5); // The top right corner  
	//glTexCoord2d(0, 0);
	//glVertex3f(x+130, y+0.1, z+i+5); // The bottom right corner  
	//glEnd(); 
	//}

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	
}

void GLUtils::drawGround(double size, double spotRadius, int nb, bool drawmaze, double currx, double curry, double currz){

	glDisable(GL_LIGHTING);
	glColor3d(1, 1, 1);
	double x, z, x1, z1;
	outputFile.open("testdata.txt");
	if (groundTexture == NULL )
		groundTexture = new GLTexture("../data/textures/grid.bmp");

	glEnable(GL_TEXTURE_2D);
	groundTexture->activate();
	glBegin(GL_QUADS);
	
	double smallSize = 2 * size / (float)nb;
	for (int i=0; i<nb; ++i) {
		x1 = -size + i * smallSize;
		for (int j=0; j<nb; ++j) {
			z1 = -size + j * smallSize;
			x = x1; z = z1;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/20, z/20);
			glVertex3d(x, 0,z);
			z = z1+smallSize;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/20, z/20);
			glVertex3d(x, 0,z);
			x = x1+smallSize;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/20, z/20);
			glVertex3d(x, 0,z);
			z = z1;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/20, z/20);
			glVertex3d(x, 0,z);
		}
	}
	glEnd();
    glDisable(GL_TEXTURE_2D);

	if(drawmaze)
	{
	if (mazeTexture == NULL )
		mazeTexture = new GLTexture("../data/textures/brick3.bmp");
	
	//walls_ind.clear();
	int maze_array[35][25]={
	{3,6,9,13,13,9,2,6,9,9,5,10,6,5,6,9,5,6,2,6,9,9,9,9,5},
	{10,10,6,8,12,5,6,11,9,2,10,10,10,12,8,4,8,12,9,14,3,6,9,13,8},
	{10,10,10,6,14,7,8,4,9,9,8,7,8,10,6,9,5,7,5,10,7,8,4,14,3},
	{10,10,10,1,7,5,6,5,6,5,6,9,9,8,12,2,7,9,8,10,6,9,5,10,10},
	{10,10,10,6,5,7,8,7,8,7,8,6,9,2,10,6,5,6,2,7,8,3,10,12,8},
	{7,14,10,10,10,4,9,13,9,13,5,10,6,9,8,10,10,10,6,9,9,14,10,10,3},
	{4,14,7,8,7,9,5,10,3,10,7,8,12,9,5,10,7,8,12,9,5,7,8,7,8},
	{6,8,6,9,9,5,7,11,8,7,9,9,8,4,8,10,6,13,8,4,11,9,13,9,2},
	{7,9,8,6,5,12,9,5,6,9,9,5,6,9,5,7,8,7,9,5,6,9,8,6,5},
	{3,6,9,8,7,8,6,8,10,6,9,8,10,3,10,6,5,4,13,8,7,5,4,8,10},
	{10,7,9,9,5,4,8,4,14,10,3,6,8,7,8,10,7,5,10,6,5,7,9,13,14},
	{10,6,9,9,11,9,9,5,10,7,8,7,9,9,9,14,6,8,1,10,10,4,5,1,10},
	{12,8,6,9,9,5,3,7,11,9,9,9,9,13,5,1,7,9,9,14,7,5,7,13,8},
	{7,13,8,6,5,7,14,6,2,6,9,5,3,10,10,6,9,9,5,10,6,11,2,7,5},
	{4,8,6,8,10,4,11,14,6,8,6,8,7,8,10,7,5,3,10,10,7,9,5,6,8},
	{6,5,7,5,7,9,9,8,10,6,8,6,13,5,10,3,10,10,10,10,6,9,8,7,2},
	{1,10,6,8,4,9,5,6,8,12,2,12,0,14,10,10,10,7,8,1,10,4,9,9,5},
	{6,8,7,9,9,9,11,8,3,7,5,12,0,14,10,10,7,9,9,5,7,9,9,13,8},
	{7,9,9,5,6,5,6,9,11,9,14,12,0,14,7,8,6,9,13,8,6,5,7,14,3},
	{6,9,9,11,8,10,1,6,5,6,14,12,11,8,4,5,7,2,7,5,10,7,5,7,14},
	{10,6,13,9,5,7,9,8,10,10,7,8,6,9,9,11,9,9,9,8,1,6,14,6,14},
	{10,1,7,2,7,9,9,9,8,10,6,5,10,6,9,9,9,9,5,6,9,8,7,8,10},
	{10,6,5,6,9,9,9,13,9,8,10,7,8,10,6,9,9,13,8,12,9,2,6,5,10},
	{12,8,10,10,6,9,5,1,6,9,8,4,5,10,7,5,3,7,5,7,9,9,8,10,1},
	{10,4,8,10,10,3,7,5,7,5,6,9,8,10,3,10,7,5,7,9,9,9,5,7,5},
	{10,6,5,10,7,8,3,7,13,8,7,9,9,8,10,7,5,7,9,9,9,5,12,9,8},
	{10,10,12,8,6,9,14,6,8,6,5,4,13,9,11,5,10,6,5,6,5,7,14,4,5},
	{7,8,7,5,10,4,8,7,5,10,7,5,10,6,2,10,10,10,10,10,12,2,7,9,8},
	{6,9,9,14,7,9,9,13,8,7,5,7,11,8,6,14,10,10,10,10,10,6,13,9,5},
	{10,6,2,7,9,9,5,12,5,3,10,3,6,9,8,1,12,14,7,8,10,10,7,5,1},
	{10,7,9,9,9,5,10,10,10,7,14,10,7,9,9,5,10,6,8,5,10,7,2,7,5},
	{10,3,6,9,5,10,7,8,10,3,10,7,9,9,5,7,14,7,5,10,7,9,13,9,8},
	{7,14,12,5,7,14,6,9,8,7,14,6,13,9,8,3,7,9,8,7,5,6,8,6,2},
	{3,10,10,10,3,10,10,4,13,5,7,8,1,6,13,11,9,9,9,9,8,7,5,7,5},
	{7,8,1,7,8,7,11,9,8,7,9,9,9,8,7,9,9,9,9,9,9,2,7,9,8}};

	int motion_array[35][25];
	if (portalTexture == NULL )
		portalTexture = new GLTexture("../data/textures/roof6.bmp");
	if (iceTexture == NULL )
		iceTexture = new GLTexture("../data/textures/Ice0002.bmp");
	if (fireTexture == NULL )
		fireTexture = new GLTexture("../data/textures/Fire_2.bmp");
	if (JfireTexture == NULL )
		JfireTexture = new GLTexture("../data/textures/JUMP.bmp");
	if(rampTexture == NULL)
		rampTexture = new GLTexture("../data/textures/wood2.bmp");
	if (waterTexture == NULL )
		waterTexture = new GLTexture("../data/textures/water0.bmp");
	if (pebbleTexture == NULL )
		pebbleTexture = new GLTexture("../data/textures/pebble.bmp");
	if (boardTexture == NULL )
		boardTexture = new GLTexture("../data/textures/wood6.bmp");
	if (boardTextureJump == NULL )
		boardTextureJump = new GLTexture("../data/textures/wood6_jump.bmp");
	if(boardTextureSneak == NULL)
		boardTextureSneak = new GLTexture("../data/textures/wood6_sneak.bmp");
	if(boardTextureSidewalk == NULL)
		boardTextureSidewalk = new GLTexture("../data/textures/wood6_sidewalk.bmp");
	if(boardTextureMoonwalk == NULL)
		boardTextureMoonwalk = new GLTexture("../data/textures/wood6_moonwalk.bmp");
	if(boardTextureSidejump == NULL)
		boardTextureSidejump = new GLTexture("../data/textures/wood6_sidejump.bmp");
	if(boardTextureSkate == NULL)
		boardTextureSkate = new GLTexture("../data/textures/wood6_skate.bmp");
	if(boardTextureDuck == NULL)
		boardTextureDuck = new GLTexture("../data/textures/wood6_duck.bmp");
	if(boardTextureCatwalk == NULL)
		boardTextureCatwalk = new GLTexture("../data/textures/wood6_catwalk.bmp");
	if(boardTextureHop == NULL)
		boardTextureHop = new GLTexture("../data/textures/wood6_hop.bmp");
	if(boardTextureRun == NULL)
		boardTextureRun = new GLTexture("../data/textures/wood6_run.bmp");
	if(boardTextureLimp == NULL)
		boardTextureLimp = new GLTexture("../data/textures/wood6_limp.bmp");
	if(boardTextureJog == NULL)
		boardTextureJog = new GLTexture("../data/textures/wood6_jog.bmp");
	if(boardTextureTiptoe == NULL)
		boardTextureTiptoe = new GLTexture("../data/textures/wood6_tiptoe.bmp");

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	mazeTexture->activate();
	glColor3d(1,1,1);
	//drawcube(-50,0,0);
	double x_maze=-2700;
	double y_maze=0;
	double z_maze=0;
	for(int i=0;i<35;i++)
	{
		for(int j=24;j>=0;j--)
		{
			if(fabs(currx-x_maze)<750 && fabs(currz-z_maze)<750)
			{
			switch(maze_array[i][j])
			{
			case 0: break;
			case 1: drawcube1(x_maze,y_maze,z_maze);
					break;
			case 2: drawcube2(x_maze,y_maze,z_maze);
					break;
			case 3: drawcube3(x_maze,y_maze,z_maze);
					break;
			case 4: drawcube4(x_maze,y_maze,z_maze);
					break;
			case 5: drawcube5(x_maze,y_maze,z_maze);
					break;
			case 6: drawcube6(x_maze,y_maze,z_maze);
					break;
			case 7: drawcube7(x_maze,y_maze,z_maze);
					break;
			case 8: drawcube8(x_maze,y_maze,z_maze);
					break;
			case 9: drawcube9(x_maze,y_maze,z_maze);
					break;
			case 10: drawcube10(x_maze,y_maze,z_maze);
					break;
			case 11: drawcube11(x_maze,y_maze,z_maze);
					break;
			case 12: drawcube12(x_maze,y_maze,z_maze);
					break;
			case 13: drawcube13(x_maze,y_maze,z_maze);
					break;
			case 14: drawcube14(x_maze,y_maze,z_maze);
					break;
			}
			}
			x_maze=x_maze+200.5;
		}
		x_maze=-2700;
		z_maze=z_maze+200.5;
	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	x_maze=-2700;
	y_maze=0;
	z_maze=0;

	for(int j=24;j>=0;j--)
	{
		for(int i=0;i<35;i++)
		{
			if(fabs(currx-x_maze)<750 && fabs(currz-z_maze)<750)
			{
			if(j==0 &&((i-3)%4==0))
				motion_array[i][j]=23;
			else if(j==2 && ((i-2)%5==0))
				motion_array[i][j]=22;
			else if(j==4 && ((i-2)%3==0))
				motion_array[i][j]=14;
			else if(j==6 && ((i)%3==0))
				motion_array[i][j]=26;
			else if(j==8 && ((i-1)%3==0))
				motion_array[i][j]=19;
			else if(j==11 && ((i-3)%3==0) && (i-3)>=0)
				motion_array[i][j]=18;
			else if(j==12 && ((i-1)%3==0))
				motion_array[i][j]=3;
			else if(j==15 && ((i)%3==0))
				motion_array[i][j]=13;
			else if(j==17 && ((i-1)%3==0))
				motion_array[i][j]=15;
			else if(j==19 && ((i-2)%3==0))
				motion_array[i][j]=2;
			else if(j==20 && ((i)%3==0))
				motion_array[i][j]=11;
			else if(j==22 && ((i-1)%3==0))
				motion_array[i][j]=8;
			else if(j==24 && ((i-2)%3==0))
				motion_array[i][j]=4;
			else
				motion_array[i][j]=-1;

			if((maze_array[i][j]==1 || maze_array[i][j]==3 || maze_array[i][j]==10) && motion_array[i][j]!=-1)
			{
				outputFile<<"{"<<x_maze<<","<<x_maze+200<<","<<z_maze<<","<<z_maze+200<<","<<motion_array[i][j]<<"},"<<endl;
				drawportal1(x_maze,y_maze,z_maze,motion_array[i][j]);
				if(motion_array[i][j]==18)
					drawice(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==13)
					drawcatwalk(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==26 || motion_array[i][j]==22 )
					drawside(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==3)
					drawjump(x_maze,y_maze+0.1,z_maze);
			}
			else if((maze_array[i][j]==2 || maze_array[i][j]==4 || maze_array[i][j]==9) && motion_array[i][j]!=-1)
			{
				outputFile<<"{"<<x_maze<<","<<x_maze+200<<","<<z_maze<<","<<z_maze+200<<","<<motion_array[i][j]<<"},"<<endl;
				drawportal2(x_maze,y_maze,z_maze,motion_array[i][j]);
				if(motion_array[i][j]==18)
					drawice(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==13)
					drawcatwalk(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==26 || motion_array[i][j]==22 )
					drawside(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==3)
					drawjump(x_maze,y_maze+0.1,z_maze);
			}
			else if(motion_array[i][j]!=-1)
			{
				outputFile<<"{"<<x_maze<<","<<x_maze+200<<","<<z_maze<<","<<z_maze+200<<","<<motion_array[i][j]<<"},"<<endl;
				drawportal3(x_maze,y_maze,z_maze,motion_array[i][j]);
				if(motion_array[i][j]==18)
					drawice(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==13)
					drawcatwalk(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==26 || motion_array[i][j]==22 )
					drawside(x_maze,y_maze+0.1,z_maze);
				else if(motion_array[i][j]==3)
					drawjump(x_maze,y_maze+0.1,z_maze);
			}
			}

			z_maze+=200.5;
		}
		z_maze=0;
		x_maze+=200.5;
	}

	glPopMatrix();
	}

}


