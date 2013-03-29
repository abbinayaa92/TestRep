#ifndef CARTESIANFORMAT_H
#define CARTESIANFORMAT_H

struct NODEHEADERCF
{
	char name[20];
};

struct NODESETCF
{
	double* Xposition;
	double* Yposition;
	double* Zposition;
};

struct LINESETCF
{
	int* start;
	int* end;
};

class CartesianFormat
{
	int frameNumber;
	double frameTime;
	int currentFrame;
	int pointNumber;
	int lineNumber;

	struct NODEHEADERCF* nodeHeaderCF;
	struct NODESETCF* nodeCF;
	struct LINESETCF lineCF;

	public:
		CartesianFormat();
		void setFrameNumber(int fn);
		void setFrameTime(double ft);
		void setPointNumber(int pn);
		void setLineNumber(int ln);
		void updateNodeHeader(int ind, const char* ch);
		void updateNodeNumber (int fn);
		void updateNodeX(int fn, int ind, double pos);
		void updateNodeY(int fn, int ind, double pos);
		void updateNodeZ(int fn, int ind, double pos);
		void gotoNextFrame();
		void gotoPreviousFrame();
		int getNodeNumber();
		int getLineNumber();
		LINESETCF* getLineSet();
		double getNodeX(int fn, int ind);
		double getNodeY(int fn, int ind);
		double getNodeZ(int fn, int ind);
		double getNodeX(int ind);
		double getNodeY(int ind);
		double getNodeZ(int ind);
		void updateLine(int ind, int st, int en);
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