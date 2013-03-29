#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <string.h> 
#include <math.h>
#include <vector>
#include <GL/glut.h>
#include "BVHFormat.h"
#include "CartesianFormat.h"
#include "Qrotation.h"

using namespace std;

#ifndef PI
#define PI 3.1415926535897932384626433832795028841972
#endif

BVHFormat::BVHFormat(const char * filename, double rot)
{	
	currentFrame = 0;
	pointNumber = 0;
	FILE * fp = fopen (filename, "r");
	char* tempRead = new char[100];
	NODEHEADER* nodeHeader;
	NODEHEADER* parentHeader;
	fscanf(fp, "%s", tempRead);
	if (strcmp(tempRead, "HIERARCHY")==0)
	{
		fscanf(fp, "%s", tempRead);
		if (strcmp(tempRead, "ROOT")==0)
		{
			pointNumber++;
			fscanf(fp, "%s", tempRead);
			nodeHeader = &rootHeader;
			nodeHeader->numberOfChild = 0;
			nodeHeader->parent = NULL;
			nodeHeader->child = new NODEHEADER*[7];
			strcpy(nodeHeader->name, tempRead);
			fscanf(fp, "%s", tempRead);
			
			if (strcmp(tempRead, "{")==0)
			{
				fscanf(fp, "%s", tempRead);
				if (strcmp(tempRead, "OFFSET")==0)
				{
					fscanf(fp, "%lf", &(nodeHeader->offsetX));
					fscanf(fp, "%lf", &(nodeHeader->offsetY));
					fscanf(fp, "%lf", &(nodeHeader->offsetZ));
					for (int i=0; i<8; i++) fscanf(fp, "%s", tempRead);
					
					do
					{
						fscanf (fp, "%s", tempRead);
						if (strcmp(tempRead, "JOINT")==0)
						{
							pointNumber++;
							parentHeader = nodeHeader;
							parentHeader->child[parentHeader->numberOfChild] = new NODEHEADER;
							nodeHeader = parentHeader->child[parentHeader->numberOfChild];
							parentHeader->numberOfChild++;
							nodeHeader->child = new NODEHEADER*[7];
							nodeHeader->numberOfChild = 0;
							nodeHeader->parent = parentHeader;
							fscanf(fp, "%s", nodeHeader->name);
							fscanf(fp, "%s", tempRead);
							if (strcmp(tempRead, "{")==0)
							{
								fscanf(fp, "%s", tempRead);
								if (strcmp(tempRead, "OFFSET")==0)
								{
									fscanf(fp, "%lf", &(nodeHeader->offsetX));
									fscanf(fp, "%lf", &(nodeHeader->offsetY));
									fscanf(fp, "%lf", &(nodeHeader->offsetZ));
								}
								else
								{
									printf("Error BVH format.\n");
									exit(0);
								}
								for (int i=0; i<5; i++) fscanf(fp, "%s", tempRead);
							}
							else
							{
								printf("Error BVH format.\n");
								exit(0);
							}
						}
						else if (strcmp(tempRead, "End")==0)
						{
							pointNumber++;
							parentHeader = nodeHeader;
							parentHeader->child[parentHeader->numberOfChild] = new NODEHEADER;
							nodeHeader = parentHeader->child[parentHeader->numberOfChild];
							parentHeader->numberOfChild++;
							nodeHeader->numberOfChild = 0;
							nodeHeader->parent = parentHeader;
							fscanf(fp, "%s", tempRead);
							strcpy(nodeHeader->name, "endSite");
							fscanf(fp, "%s", tempRead);
							if (strcmp(tempRead, "{")==0)
							{
								fscanf(fp, "%s", tempRead);
								if (strcmp(tempRead, "OFFSET")==0)
								{
									fscanf(fp, "%lf", &(nodeHeader->offsetX));
									fscanf(fp, "%lf", &(nodeHeader->offsetY));
									fscanf(fp, "%lf", &(nodeHeader->offsetZ));
								}
								else
								{
									printf("Error BVH format.\n");
									exit(0);
								}
							}
						}
						else if (strcmp(tempRead, "}")==0)
						{
							nodeHeader = nodeHeader->parent;
						}
						else
						{
							printf("Error BVH format.\n");
							exit(0);
						}
					} while (nodeHeader!=NULL);
				}
				else
				{
					printf("Error BVH format.\n");
					exit(0);
				}
			}
			else
			{
				printf("Error BVH format.\n");
				exit(0);
			}
		}
		else
		{
			printf ("Error BVH format.\n");
			exit(0);
		}
	}
	else
	{
		printf("Error BVH format.\n");
		exit(0);
	}
	
	fscanf(fp, "%s", tempRead);
	if (strcmp(tempRead, "MOTION")==0)
	{
		fscanf(fp, "%s", tempRead);
		if (strcmp(tempRead, "Frames:")==0)
		{
			fscanf(fp, "%d", &frameNumber);
			fscanf(fp, "%s", tempRead);
			fscanf(fp, "%s", tempRead);
			fscanf(fp, "%lf", &frameTime);
			//printf ("%d %lf\n", frameNumber, frameTime);
			rootElement = new ROOTELEMENT[frameNumber];
			double XoffsetPos, ZoffsetPos;
			fscanf(fp, "%lf %lf %lf", &XoffsetPos, &(rootElement[0].Yposition), &ZoffsetPos);
			rootElement[0].Xposition = 0;
			rootElement[0].Zposition = 0;
			rootElement[0].nodeElement.parent = NULL;
			addChildElement(&(rootElement[0].nodeElement), &rootHeader, fp);
			for (int i=1; i<frameNumber; i++)
			{
				fscanf(fp, "%lf %lf %lf", &(rootElement[i].Xposition), &(rootElement[i].Yposition), &(rootElement[i].Zposition));
				rootElement[i].Xposition -= XoffsetPos;
				rootElement[i].Zposition -= ZoffsetPos;
				rootElement[i].nodeElement.parent = NULL;
				addChildElement(&(rootElement[i].nodeElement), &rootHeader, fp);
			}
			/*YrotationOffset = 180/PI*asin(rootElement[frameNumber-1].Zposition/sqrt(rootElement[frameNumber-1].Zposition*rootElement[frameNumber-1].Zposition+rootElement[frameNumber-1].Xposition*rootElement[frameNumber-1].Xposition));
			if (rootElement[frameNumber-1].Xposition<0)
			{
				if(YrotationOffset>0) YrotationOffset = 180 - YrotationOffset;
				else YrotationOffset = -180 - YrotationOffset;
			}*/
			//YrotationOffset = rot;
			YrotationOffset = 0;
			//printf("%lf\n", YrotationOffset);
		}
		else
		{
			printf("Error BVH format.\n");
			exit(0);
		}
	}
	else
	{
		printf("Error BVH format.\n");
		exit(0);
	}
	fclose(fp);
	lineNumber = pointNumber - 1;
	//printf ("%d\n", pointNumber);

	BVHtoCartesian();
}

void BVHFormat::addChildElement(NODEELEMENT* node, NODEHEADER* nodeHeader, FILE* fp)
{	
	if (strcmp(nodeHeader->name, "endSite")!=0)
	{
		fscanf(fp, "%lf %lf %lf", &(node->Xrotation),  &(node->Yrotation), &(node->Zrotation));
	}
	node->child = new NODEELEMENT*[nodeHeader->numberOfChild];
	for (int i=0; i<nodeHeader->numberOfChild; i++)
	{
		node->child[i] = new NODEELEMENT;
		node->child[i]->parent = node;
		addChildElement(node->child[i], nodeHeader->child[i], fp);
	}
}

int BVHFormat::countRootChild()
{
	return rootHeader.numberOfChild;
}

double BVHFormat::getFrameTime()
{
	return frameTime;
}

int BVHFormat::getFrameNumber()
{
	return frameNumber;
}

void BVHFormat::gotoNextFrame()
{
	currentFrame = (currentFrame+1)%frameNumber;
	ctFormat.gotoNextFrame();
}

void BVHFormat::gotoPreviousFrame()
{
	currentFrame = (currentFrame+frameNumber-1)%frameNumber;
	ctFormat.gotoPreviousFrame();
}

NODEHEADER* BVHFormat::getRootHeader()
{
	return &rootHeader;
}

ROOTELEMENT* BVHFormat::getRootElement()
{
	return rootElement;
}

void BVHFormat::setYrotationOffset(double rot)
{
	YrotationOffset = rot;
}

double BVHFormat::getYrotationOffset()
{
	return YrotationOffset;
}

LINESETCF* BVHFormat::getLineSet()
{
	return ctFormat.getLineSet();
}

void BVHFormat::BVHtoCartesian()
{
	FILE * fout = fopen("..\\test.txt", "w");
	for (int i=0; i<frameNumber; i++)
	{
		fprintf(fout, "%lf %lf %lf ", rootElement[i].Xposition, rootElement[i].Yposition, rootElement[i].Zposition);
		fprintf(fout, "%lf %lf %lf\n", rootElement[i].nodeElement.Xrotation, rootElement[i].nodeElement.Yrotation, rootElement[i].nodeElement.Zrotation);
	}
	fclose(fout);

	ctFormat.setFrameNumber(frameNumber);
	ctFormat.setFrameTime(frameTime);
	ctFormat.setPointNumber(pointNumber);
	ctFormat.setLineNumber(lineNumber);
	
	ct = 0;
	ctFormat.updateNodeHeader(ct, rootHeader.name);
	ct++;
	for (int i=0; i<rootHeader.numberOfChild; i++)
	{
		BVHtoCartesianUpdateNameAndLine(0, rootHeader.child[i]);
	}

	double Xtemp, Ytemp, Ztemp;
	double point[3];
	for (int i=0; i<frameNumber; i++)
	{
		ct = 0;
		Qrotation qr(YrotationOffset, 0.0, 1.0, 0.0);
		ctFormat.updateNodeNumber(i);
		Xtemp = rootElement[i].Xposition + rootHeader.offsetX;
		Ytemp = rootElement[i].Yposition + rootHeader.offsetY;
		Ztemp = rootElement[i].Zposition + rootHeader.offsetZ;
		qr.rotatePoint(Xtemp, Ytemp, Ztemp, point);
		Xtemp = point[0]; Ytemp = point[1]; Ztemp = point[2];
		ctFormat.updateNodeX(i, ct, Xtemp);
		ctFormat.updateNodeY(i, ct, Ytemp);
		ctFormat.updateNodeZ(i, ct, Ztemp);
		ct++;
		Qrotation qtemp = Qrotation(rootElement[i].nodeElement.Zrotation, 0.0, 0.0, 1.0);
		qtemp.rotate(rootElement[i].nodeElement.Yrotation, 0.0, 1.0, 0.0);
		qtemp.rotate(rootElement[i].nodeElement.Xrotation, 1.0, 0.0, 0.0);
		qtemp.rotate(qr.getAngle(), qr.getXaxis(), qr.getYaxis(), qr.getZaxis());
		qr = qtemp;
		for (int j=0; j<rootHeader.numberOfChild; j++)
		{
			BVHtoCartesianUpdateNode(i, Xtemp, Ytemp, Ztemp, qr, rootElement[i].nodeElement.child[j], rootHeader.child[j]);
		}
	}
}

void BVHFormat::BVHtoCartesianUpdateNode(int fn, double Xtemp, double Ytemp, double Ztemp, Qrotation qr, NODEELEMENT* node, NODEHEADER* nodeHeader)
{
	double tempOffset[3];
	qr.rotatePoint(nodeHeader->offsetX, nodeHeader->offsetY, nodeHeader->offsetZ, tempOffset);
	Xtemp += tempOffset[0];
	Ytemp += tempOffset[1];
	Ztemp += tempOffset[2];
	ctFormat.updateNodeX(fn, ct, Xtemp);
	ctFormat.updateNodeY(fn, ct, Ytemp);
	ctFormat.updateNodeZ(fn, ct, Ztemp);
	ct++;
	if (strcmp(nodeHeader->name, "endSite")!=0)
	{
		Qrotation qtemp = Qrotation(node->Zrotation, 0.0, 0.0, 1.0);
		qtemp.rotate(node->Yrotation, 0.0, 1.0, 0.0);
		qtemp.rotate(node->Xrotation, 1.0, 0.0, 0.0);
		qtemp.rotate(qr.getAngle(), qr.getXaxis(), qr.getYaxis(), qr.getZaxis());
		qr = qtemp;
	}
	for (int i=0; i<nodeHeader->numberOfChild; i++)
	{
		BVHtoCartesianUpdateNode(fn, Xtemp, Ytemp, Ztemp, qr, node->child[i], nodeHeader->child[i]);
	}
}

void BVHFormat::BVHtoCartesianUpdateNameAndLine(int prt, NODEHEADER* nodeHeader)
{
	ctFormat.updateNodeHeader(ct, nodeHeader->name);
	ctFormat.updateLine(ct-1, prt, ct);
	prt = ct;
	ct++;
	for (int i=0; i<nodeHeader->numberOfChild; i++)
	{
		BVHtoCartesianUpdateNameAndLine(prt, nodeHeader->child[i]);
	}
}

//Function on ctFormat
int BVHFormat::getNodeNumber()
{
	return ctFormat.getNodeNumber();
}

int BVHFormat::getLineNumber()
{
	return ctFormat.getLineNumber();
}

double BVHFormat::getNodeX(int fn, int ind)
{
	return ctFormat.getNodeX(fn, ind);
}

double BVHFormat::getNodeY(int fn, int ind)
{
	return ctFormat.getNodeY(fn, ind);
}

double BVHFormat::getNodeZ(int fn, int ind)
{
	return ctFormat.getNodeZ(fn, ind);
}

double BVHFormat::getNodeX(int ind)
{
	return ctFormat.getNodeX(ind);
}

double BVHFormat::getNodeY(int ind)
{
	return ctFormat.getNodeY(ind);
}

double BVHFormat::getNodeZ(int ind)
{
	return ctFormat.getNodeZ(ind);
}

double BVHFormat::getLineStartX(int fn, int ind)
{
	return ctFormat.getLineStartX(fn, ind);
}

double BVHFormat::getLineStartY(int fn, int ind)
{
	return ctFormat.getLineStartY(fn, ind);
}

double BVHFormat::getLineStartZ(int fn, int ind)
{
	return ctFormat.getLineStartZ(fn ,ind);
}

double BVHFormat::getLineEndX(int fn, int ind)
{
	return ctFormat.getLineEndX(fn, ind);
}

double BVHFormat::getLineEndY(int fn, int ind)
{
	return ctFormat.getLineEndY(fn, ind);
}

double BVHFormat::getLineEndZ(int fn, int ind)
{
	return ctFormat.getLineEndZ(fn, ind);
}

double BVHFormat::getLineStartX(int ind)
{
	return ctFormat.getLineStartX(ind);
}

double BVHFormat::getLineStartY(int ind)
{
	return ctFormat.getLineStartY(ind);
}

double BVHFormat::getLineStartZ(int ind)
{
	return ctFormat.getLineStartZ(ind);
}

double BVHFormat::getLineEndX(int ind)
{
	return ctFormat.getLineEndX(ind);
}

double BVHFormat::getLineEndY(int ind)
{
	return ctFormat.getLineEndY(ind);
}

double BVHFormat::getLineEndZ(int ind)
{
	return ctFormat.getLineEndZ(ind);
}