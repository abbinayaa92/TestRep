#define _CRT_SECURE_NO_DEPRECATE
#include "CartesianFormat.h"
#include "string.h"

CartesianFormat::CartesianFormat()
{
	currentFrame = 0;
}

void CartesianFormat::setFrameNumber(int fn)
{
	frameNumber = fn;
	nodeCF = new NODESETCF[fn];
}

void CartesianFormat::setFrameTime(double ft)
{
	frameTime = ft;
}

void CartesianFormat::setPointNumber(int pn)
{
	pointNumber = pn;
	nodeHeaderCF = new NODEHEADERCF[pn];
}

void CartesianFormat::setLineNumber(int ln)
{
	lineNumber = ln;
	lineCF.start = new int[ln];
	lineCF.end = new int[ln];
}

LINESETCF* CartesianFormat::getLineSet()
{
	return &lineCF;
}

void CartesianFormat::updateNodeHeader(int ind, const char* ch)
{
	strcpy(nodeHeaderCF[ind].name, ch);
}

void CartesianFormat::updateNodeNumber(int fn)
{
	nodeCF[fn].Xposition = new double[pointNumber];
	nodeCF[fn].Yposition = new double[pointNumber];
	nodeCF[fn].Zposition = new double[pointNumber];
}

void CartesianFormat::updateNodeX(int fn, int ind, double pos)
{
	nodeCF[fn].Xposition[ind] = pos;
}

void CartesianFormat::updateNodeY(int fn, int ind, double pos)
{
	nodeCF[fn].Yposition[ind] = pos;
}

void CartesianFormat::updateNodeZ(int fn, int ind, double pos)
{
	nodeCF[fn].Zposition[ind] = pos;
}

void CartesianFormat::gotoNextFrame()
{
	currentFrame = (currentFrame + 1)%frameNumber;
}

void CartesianFormat::gotoPreviousFrame()
{
	currentFrame = (currentFrame + frameNumber - 1)%frameNumber;
}

int CartesianFormat::getNodeNumber()
{
	return pointNumber;
}

int CartesianFormat::getLineNumber()
{
	return lineNumber;
}

double CartesianFormat::getNodeX(int fn, int ind)
{
	return nodeCF[fn].Xposition[ind];
}

double CartesianFormat::getNodeY(int fn, int ind)
{
	return nodeCF[fn].Yposition[ind];
}

double CartesianFormat::getNodeZ(int fn, int ind)
{
	return nodeCF[fn].Zposition[ind];
}

double CartesianFormat::getNodeX(int ind)
{
	return nodeCF[currentFrame].Xposition[ind];
}

double CartesianFormat::getNodeY(int ind)
{
	return nodeCF[currentFrame].Yposition[ind];
}

double CartesianFormat::getNodeZ(int ind)
{
	return nodeCF[currentFrame].Zposition[ind];
}

void CartesianFormat::updateLine(int ind, int st, int en)
{
	lineCF.start[ind] = st;
	lineCF.end[ind] = en;
}

double CartesianFormat::getLineStartX(int fn, int ind)
{
	return nodeCF[fn].Xposition[lineCF.start[ind]];
}

double CartesianFormat::getLineStartY(int fn, int ind)
{
	return nodeCF[fn].Yposition[lineCF.start[ind]];
}

double CartesianFormat::getLineStartZ(int fn, int ind)
{
	return nodeCF[fn].Zposition[lineCF.start[ind]];
}

double CartesianFormat::getLineEndX(int fn, int ind)
{
	return nodeCF[fn].Xposition[lineCF.end[ind]];
}

double CartesianFormat::getLineEndY(int fn, int ind)
{
	return nodeCF[fn].Yposition[lineCF.end[ind]];
}

double CartesianFormat::getLineEndZ(int fn, int ind)
{
	return nodeCF[fn].Zposition[lineCF.end[ind]];
}

double CartesianFormat::getLineStartX(int ind)
{
	return nodeCF[currentFrame].Xposition[lineCF.start[ind]];
}

double CartesianFormat::getLineStartY(int ind)
{
	return nodeCF[currentFrame].Yposition[lineCF.start[ind]];
}

double CartesianFormat::getLineStartZ(int ind)
{
	return nodeCF[currentFrame].Zposition[lineCF.start[ind]];
}

double CartesianFormat::getLineEndX(int ind)
{
	return nodeCF[currentFrame].Xposition[lineCF.end[ind]];
}

double CartesianFormat::getLineEndY(int ind)
{
	return nodeCF[currentFrame].Yposition[lineCF.end[ind]];
}

double CartesianFormat::getLineEndZ(int ind)
{
	return nodeCF[currentFrame].Zposition[lineCF.end[ind]];
}