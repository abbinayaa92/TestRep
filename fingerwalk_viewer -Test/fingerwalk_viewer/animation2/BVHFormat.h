#ifndef BVHFORMAT_H
#define BVHFORMAT_H

#include "stdio.h"
#include "CartesianFormat.h"
#include "Qrotation.h"

struct NODEHEADER // Start of structure representing a single bone in a skeleton 
{ 
	char name[20];
	NODEHEADER** child;
	NODEHEADER* parent;
	int numberOfChild;
	double offsetX;
	double offsetY;
	double offsetZ;
};

struct NODEELEMENT
{
	double Xrotation;
	double Yrotation;
	double Zrotation;
	NODEELEMENT** child;
	NODEELEMENT* parent;
};

struct ROOTELEMENT
{
	NODEELEMENT nodeElement;
	double Xposition;
	double Yposition;
	double Zposition;
};

class BVHFormat
{
	struct NODEHEADER rootHeader;
	struct ROOTELEMENT* rootElement;
	int frameNumber;
	double frameTime;
	int pointNumber;
	int lineNumber;
	double YrotationOffset;
	
	//converted type to cartesian
	CartesianFormat ctFormat;
	
	//temporary variable
	int currentFrame; //use to track the frame of the raw data
	int ct;
	
	private:
		void addChildElement(NODEELEMENT* node, NODEHEADER* nodeHeader, FILE* fp);
		void BVHtoCartesianUpdateNode(int fn, double Xtemp, double Ytemp, double Ztemp, Qrotation qr, NODEELEMENT* node, NODEHEADER* nodeHeader);
		void BVHtoCartesianUpdateNameAndLine(int prt, NODEHEADER* nodeHeader);

	public:
		BVHFormat() {frameNumber = 0;};
		BVHFormat(const char* filename, double rot);
		int countRootChild();
		double getFrameTime();
		int getFrameNumber();
		void gotoNextFrame();
		void gotoPreviousFrame();
		NODEHEADER* getRootHeader();
		ROOTELEMENT* getRootElement();
		void setYrotationOffset(double rot);
		double getYrotationOffset();
		LINESETCF* getLineSet();

		//The following methods work on the Cartesian format type
		void BVHtoCartesian();
		int getNodeNumber();
		int getLineNumber();
		double getNodeX(int fn, int ind);
		double getNodeY(int fn, int ind);
		double getNodeZ(int fn, int ind);
		double getNodeX(int ind);
		double getNodeY(int ind);
		double getNodeZ(int ind);
		double getLineStartX(int fn, int ind);
		double getLineStartY(int fn, int ind);
		double getLineStartZ(int fn, int ind);
		double getLineEndX(int fn, int ind);
		double getLineEndY(int fn, int ind);
		double getLineEndZ(int fn, int ind);
		double getLineStartX(int ind);
		double getLineStartY(int ind);
		double getLineStartZ(int ind);
		double getLineEndX(int ind);
		double getLineEndY(int ind);
		double getLineEndZ(int ind);
};

#endif