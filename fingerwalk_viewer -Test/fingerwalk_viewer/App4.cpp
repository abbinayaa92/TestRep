
#include "animation/Animation.h"
#include "App4.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/IpEndpointName.h"
#include "ip/UdpSocket.h"
#include "GLWindow.h"
#include "animation/Animation.h"
#include "App4.h"
#include "ip/IpEndpointName.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"
#include <iostream>
#include <fstream>
#include <include/glut.h>
#include "Globals.h"
#include <GLUtils/GLUtils.h>
#include <string>
#include <sstream>
#include <GLUtils/GLCamera.h>
#include "ConcurrentQueue.h"
#include <math.h>

#define clamp(val, a, b) max(min(val, b), a)
#define USE_HISTORY 0
#define USE_ANIMATION 0
#define ADDRESS "127.0.0.1"

// scaling factor from touch space to animation space
int motion_type[35][25]={
		{0,0,0,0,0,0,26,0,0,0,0,0,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,22,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{23,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{23,0,22,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{23,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,22,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{23,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,22,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{23,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,22,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{23,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{23,0,22,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{23,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0},
		{0,0,22,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
		{0,0,0,0,0,0,26,0,0,0,0,18,0,0,0,13,0,0,0,0,11,0,0,0},
		{0,0,0,0,0,0,0,0,19,0,0,0,3,0,0,0,0,15,0,0,0,0,8,0}};

static const float UNITSCALE_Y = 8.0f * 20;
static const float UNITSCALE_X = 6.0f * 20;
App4 *App4::_instance = NULL;

App4::~App4() {
}

bool App4::init(bool online) {
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	win=glutCreateWindow(WINDOW_TITLE);

	// set up callback methods
	glutDisplayFunc(App4::displayCallback);
	glutIdleFunc(App4::idleCallback);
	glutReshapeFunc(App4::reshapeCallback);
	glutMouseFunc(App4::mouseCallback);
	glutMotionFunc(App4::motionCallback);
	//glutPassiveMotionFunc(onMouseMovedPassive);
	glutKeyboardFunc(App4::keyboardCallback);
	cout<<"Enter mode :\n1.Demo\n2.Game\n3.Automatic\n4.Exit"<<endl;
	cin>>winmode;
	if(winmode==4)
		exit(0);
	if(escflag==true)
	{
		UdpTransmitSocket transmitSocket(IpEndpointName(EndPoint->address, PORT_UDP_IN+1));
		char buffer[1024];
		osc::OutboundPacketStream p( buffer, 1024 );
		p<<osc::BeginBundleImmediate<<osc::BeginMessage("/mode")<<winmode<<osc::EndMessage<<osc::EndBundle;
		transmitSocket.Send(p.Data(), p.Size());
		cout<<"sent mode to "<<EndPoint->address<<endl;
	}


	_glWindow = new GLWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,winmode);
	_online = online;

	if (!_online) {
	}

	_glWindow->addDrawable(this);
	_timer.restart();
	_lastTime = _currTime = _timer.timeEllapsed();
	/*double check=_timer.timeEllapsed();
	int count=0;
	int mul=1;
	for(int i=1;i<1000000;i++)
	{
	for(int j=1;j<100;j++)
	{
	count+=i+j;
	mul*=i*j;
	mul*=count;
	count+=mul;
	}
	}
	double check1=_timer.timeEllapsed();
	cout<<"difference in time "<<check1-check<<" result"<<count<<","<<mul<<endl;*/

	flag=0;
	playmode=0;
	numberSteps[0] = numberSteps[1] =0;
	_footPrints.clear();
	_allPrints.clear();
	_newpoints.resize(0);
	_allpoints.resize(0);
	_footPrints.push_back(&_leftFootPrints);
	_footPrints.push_back(&_rightFootPrints);
	_allPrints.push_back(&_leftFootPrints1);
	_allPrints.push_back(&_rightFootPrints1);
	_footDrawable.resize(2);
	_currFoot.resize(2);
	_prevFoot.resize(2);
	step=0;
	_currFoot[0] = _currFoot[1] = NULL;
	_prevFoot[0] = _prevFoot[1] = NULL;
	type=0;
	dispType=0;
	typeBegan=0;
	typeEnd=0;
	last_angle=0;
	last_angle_cam=0;
	retbool=true;
	numid=0;
	walkflag=0;

	_firstStep = true;
	ignoreflag.resize(2);
	ignoreflag[0]=ignoreflag[1]=false;
	_startWalking = false;
	jumpflag=true;
	escflag=false;
	endflag=false;
	sendflag=false;
	autoflag=false;
	_sa.addGaitChangedListener(this);
	if(winmode==1 ||winmode==2)
	{
		_anim = new Animation(&_footDrawable[FOOT_LEFT], &_footDrawable[FOOT_RIGHT]);
		_glWindow->addDrawable(_anim);
	}
	else if(winmode==3)
	{

		_personstate = new PersonState();
		cout<<"in set maze"<<endl;
		setMaze();
		_glWindow->addDrawable(_personstate);
	}

	_currGaitType = _nextGaitType = _prevGaitType= kGaitWalking;
	_currStepLength = _nextStepLength = 0;
	_currStepDuration = _nextStepDuration = 2.5;

	_glWindow->getCamera()->translateTarget(Vector3d(0, 20, 0));
	_glWindow->getCamera()->modifyRotations(Vector3d(0, -3.141592, 0));

	_footDrawable[FOOT_LEFT].pos = _footDrawable[FOOT_LEFT].lastPos = _footDrawable[FOOT_LEFT].toPos =  Point3d(-0.5 * 20, 0, -UNITSCALE_Y);
	_footDrawable[FOOT_RIGHT].pos = _footDrawable[FOOT_RIGHT].lastPos = _footDrawable[FOOT_RIGHT].toPos = Point3d(0.5 * 20, 0, -UNITSCALE_Y);
	return true;
}

void App4::setMaze()
{
	int maze_array[35][25] ={
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

		
		int *tempMaze[35];
		//*motionMaze[35];
		for (int i=0; i<35; i++)
		{
			tempMaze[i] = maze_array[i];
			motionMaze[i]=motion_type[i];
		}

		int width = 25;
		int height = 35;
		double unitWidth = 200;
		double unitHeight = 200;
		double xStart = -2700;
		double zStart = 0;

		_maze = new Maze(tempMaze, width, height, unitWidth, unitHeight, xStart, zStart);
		////_maze->solveMaze(0, 0, 11, 0, 9, 3);
		/*_maze->solveMaze(0, 0, 11, 0, 11, 1);
		cout<<"solved"<<endl;
		_personstate->clearPath();
		for (int i=0; i<_maze->path.size(); i++)
		{
		_personstate->addPost(_maze->path[i].z, _maze->path[i].x, WALK);
		}
		_personstate->construct_motion();
		autoflag=true;*/


}

void App4::quit() {
#if LOG_DATA
#endif
}

void App4::gaitChanged(const LocomotionModel& m) {
	GaitType g = m.gaitType;
	switch (g) {
		case kGaitWalking:
			cout << "[App4] I'm walking.. with alpha = " << m.alpha << endl;
			_nextGaitType = g;
			break;
		case kGaitJumping:
			cout << "[App4] I'm Jumping.." << endl;
			_nextGaitType = g;
			break;
		case kGaitRunning:
			cout << "[App4] I'm running.. with alpha = " << m.alpha << endl;
			_nextGaitType = g;
			break;
		case kGaitTiptoeing:
			cout << "[App4] I'm tiptoeing.." << endl;
			_nextGaitType = g;
			break;
	}
}


void App4::onDisplay() {
	glPushMatrix();
	{
		//float dt = 0.02f;
		_currTime = _timer.timeEllapsed();
		double dt = _currTime - _lastTime;
		//cout<<"in display loop"<<endl;
		//	update(dt);
		// _currStepLength

		if(winmode==1 || winmode==2)
		{
			this->updateCamera(dt);
			if(dt>=0.008)
			{
				if(retbool==false)
				{
					//cout<<"updating"<<endl;
					bool retbooltemp;
					if(walkflag>0)
						retbooltemp=_anim->DoUpdateStep(kGaitWalking,0,0.1,winmode);
					else
					{
						if(_currGaitType==kGaitWalkBack || _currGaitType==kGaitTiptoeBack || _currGaitType==kGaitLimpBack || _currGaitType==kGaitMoonWalk )
							step-=_currStepLength;
						else
							step+=_currStepLength;
						retbooltemp= _anim->DoUpdateStep(_currGaitType,_currStepLength,_currStepDuration,winmode);

					}
					flag++;
					if(retbooltemp==true) numid++;
					while(_currGaitType==15 && numid<3 && retbooltemp==true)
					{
						retbool=false;
						retbooltemp=false;
					}
					if(retbooltemp==true)
					{//cout<<"number="<<flag<<endl;
						if(walkflag==0)
						{
							if(dispType ==0) dispType=1;
							else if(dispType==1) dispType=0;	
							cout<<"disptype new:"<<dispType<<endl;
							sendflag=true;
						}

						retbool=true;
					}
				}
				else if(!(_footPrints[dispType]->empty()))
				{

					//cout<<"disptype:"<<dispType<<endl;
					FootStepNew m=_footPrints[dispType]->front();

					/*if(flag > 100)
					{
					cout<<"changing"<<endl;
					_footPrints[dispType]->pop_front();
					m.down=false;
					m.endx=m.x;
					m.endy=m.y;
					m.endTime=m.startTime+0.01;
					_allPrints[dispType]->push_back(m);

					if(dispType ==0) dispType=1;
					else if(dispType==1) dispType=0;
					}*/
					walkflag=0;
					int othertype= (dispType+1)%2;
					double mtime=m.endTime-m.startTime;
					FootStepNew prev;
					FootStepNew prev2;
					if(!(_allPrints[othertype]->empty()))
					{
						prev=_allPrints[othertype]->back();
						FootStepNew prev2;
						double prev2time,prevtime;

						if(!(_allPrints[dispType]->empty()))
						{
							prev2=_allPrints[dispType]->back();
							prev2time=prev2.endTime-prev2.startTime;
							prevtime=prev.endTime-prev.startTime;
						}
						else
						{
							prev2.y=0;
							prev2.startTime=0;
							prevtime=0;
						}
						/*float t1=prev.endTime-m.startTime;
						float t2=m.endTime - prev.startTime;
						float t= t1/t2;
						cout<<"t value:"<<t<<endl;*/
						/*if(flag>200)
						_currGaitType= _currGaitType;
						else if(t>=0.8)
						_currGaitType= kGaitHopping;
						else if(prev.endTime < m.startTime)
						_currGaitType= kGaitRunning;
						else
						_currGaitType= kGaitWalking;*/

						float stepy = (prev.endy - prev.y);

						float t1= fabs(m.startTime-prev.startTime);
						float t2= fabs(prev.startTime-prev2.startTime);
						//double t2= ((m.startTime)^2);
						//double t3=((prev.endTime)^2);
						//float t=t1/(t2)^0.5;

						//if(flag>200 || (m.y - prev.y)>700 )
						//	_currGaitType= _currGaitType;
						if(m.mode==0)
						{
							//cout<<"prev end:"<<prev.endTime<<"   "<<"new start:"<<m.startTime<<"  "<<"step duration:"<<m.stepduration<<"t1: "<<t1<<endl;
							//m.y-prev.y)>4 || (m.startTime - prev.endTime)>1)
							if(m.steplength>1 && m.stepduration>2.5)	
							{
								_currGaitType= _currGaitType;
								m.steplength=prev.steplength;
								m.stepduration=prev.stepduration;
							}
							else if((t1<0.1 && prev2.endTime<prev.startTime) || (t2<0.1 && prev.endTime<m.startTime) )
							{
								if(m.radius_end-m.radius>30 && m.radius_end>100)
									_currGaitType=kGaitBridge;
								else if(stepy > 0.1)
									_currGaitType= kGaitDuck;
								else if(stepy> 0.05)
									_currGaitType= kGaitSit;
								else if(fabs(prev2.y-m.y)<= 0.05 && fabs(prev.y-m.y)<= 0.05)
									_currGaitType=kGaitSideSkip;
								else 
								{
									m.steplength= (fabs(m.y - prev2.y)* UNITSCALE_Y)/36.0;

									/*if(fabs(prev2.y-m.y)>0.3)
									_currGaitType=kGaitJumpLong;
									else*/
									_currGaitType=kGaitJumping;
								}
							}
							else if(prev.endTime < m.startTime)
							{
								/*if(fabs(prev.y-m.y)<= 0.02)
								_currGaitType=kGaitTap;
								else*/
								if(m.stepduration <= 0.6)
								{
									_currGaitType= kGaitRunning;
								}
								else
									_currGaitType= kGaitJog;
							}
							else
							{
								//cout<<"mtime "<<mtime<<" prevtime "<<prevtime<<endl;
								if(fabs(prev2.y-m.y)<= 0.05 && fabs(prev.y-m.y)<= 0.05)
									_currGaitType=kGaitSideWalk;
								else if((fabs(prev2.endy-m.endy)>= 0.1 && fabs(prev2.endy-m.endy)<= 0.3 && fabs(prev.endy-m.endy)<= 0.05) || (fabs(prev.endy-m.endy)>= 0.1 && fabs(prev.endy-prev2.endy)<= 0.05))
									_currGaitType=kGaitMarch;
								else if(m.y <=prev.y)
								{
									if(m.down==false && (m.endy - m.y) < -0.1)
										_currGaitType= kGaitSkate;
									else if(m.down==false && (m.endy - m.y) > 0.05)
										_currGaitType= kGaitSneak;
									else if(fabs(prev.x-m.x)<= 0.05)
									{	
										_currGaitType= kGaitCatWalk;
									}
									else if(fabs(mtime-prevtime)>0.5)
									{
										//cout<<"prev2:"<<prev2time<<"  prev:"<<prevtime<<endl;
										//(m.radius>70 && prev.radius<70) || (m.radius<70 && prev.radius>70)
										/*if(prev2.startTime!=0)
										{
										cout<<"prev2:"<<prev2.endTime-prev2.startTime<<endl;
										cout<<"prev"<<prev.endTime-prev.startTime<<endl;
										}*/
										_currGaitType= kGaitLimping;
									}
									else if(m.radius > 70)
										_currGaitType= kGaitWalking;
									else
										_currGaitType= kGaitTiptoeing;	
								}
								else
								{
									if(m.down==false && (m.endy - m.y)> 0.1)
										_currGaitType= kGaitMoonWalk;
									else if(prev2.startTime!=0 && fabs(prev2time-prevtime)>0.3)
									{
										//(m.radius>65 && prev.radius<65) || (m.radius<65 && prev.radius>65)
										_currGaitType= kGaitLimpBack;
									}
									else if(m.radius > 65)
										_currGaitType= kGaitWalkBack;
									else
										_currGaitType= kGaitTiptoeBack;
								}
							}
						}
						else if(m.mode==1)
						{
							if((m.y-prev.y)>4 || (m.startTime - prev.endTime)>1)
							{
								_currGaitType= _currGaitType;
								m.steplength=0;
								m.stepduration=1;
							}
							else if(prev.endTime < m.startTime)
								_currGaitType= kGaitHop;
							else if((t1<0.1 && prev2.endTime<prev.startTime) || (t2<0.1 && prev.endTime<m.startTime) )
								_currGaitType=kGaitJumping;

						}
						else if(m.mode==2)
						{
							//if(fabs(prev.x-m.x)<= 0.02)
							_currGaitType=kGaitSpin;
							//else if( stepy>0.1)
							//_currGaitType=kGaitSneak;
						}
						else if(m.mode==3)
							_currGaitType=kGaitKick;
						else if(m.mode==4)
							_currGaitType=kGaitSplit;


						//cout<<"m.starttime:"<<m.startTime<<" prev.endtime:"<<prev.endTime<<"gait type:"<<_currGaitType<<"foot type:"<<dispType<<endl;
					}
					if(m.down==true)
					{
						//_currStepLength=0;
						//_currStepDuration=2.5;
						//_anim->DoUpdateStep(_currGaitType,0, m.stepduration);
						//_anim->DoUpdateStep(kGaitWalking,0,_currStepDuration);
						if((m.steplength>1 && m.stepduration>=1)|| m.stepduration>=1)
						{
							if(winmode==2 && _currGaitType==kGaitJumping)
							{
								_currStepLength=max(1.8,prev.steplength);
								_currStepDuration=prev.stepduration;

							}
							else
							{
								_currStepLength=prev.steplength;
								_currStepDuration=prev.stepduration;
							}
							m.steplength=prev.steplength;
							m.stepduration=prev.stepduration;
						}
						else
						{
							_currStepLength=m.steplength;
							_currStepDuration=min(2.5,m.stepduration);
						}
						//_anim->DoUpdateStep(_currGaitType,_currStepLength,_currStepDuration);
					}	
					else
					{
						//_anim->DoUpdateStep(_currGaitType,2.5,2.5);
						numid=0;

						flag=0;

						//cout<<"footprints:"<<dispType<<"  "<<_footPrints[dispType]->size()<<"other:"<<othertype<<"  "<<_footPrints[othertype]->size()<<endl;
						//cout<<"mtime: "<<mtime<<endl;
						cout<<"retbool="<<retbool<<endl;

						float newangle=0;
						if(m.angle!=0)
						{
							GLCamera *cam = _glWindow->getCamera();
							newangle=(m.angle-last_angle)/60.0;
							if(newangle>=0)
							{
								cout<<"angle:"<<newangle<<endl;
								//_anim->rotate_character(0-newangle);
								//for(int i=0;i<newangle;i++)
								//{
								////cam->modifyRotations(Vector3d(0,i, 0));
								//	
								//}
							}
							else
							{
								cout<<"angle:"<<newangle<<endl;
								//_anim->rotate_character(0-newangle);
								//for(int i=0;i>newangle;i--)
								////cam->modifyRotations(Vector3d(0,i, 0));
								//_anim->rotate_character(i*(asin(1.0))/90);
							}

							last_angle=m.angle;
						}
						if(fabs(newangle)>=0.3) 
						{
							//m.endTime=m.startTime+(prev.endTime-prev.startTime);
							//if(winmode==2)
							_currGaitType=_prevGaitType;
						}
						if((m.steplength>1 && m.stepduration>=1)|| m.stepduration>=1)
						{
							_currStepLength=prev.steplength;
							_currStepDuration=prev.stepduration;
							m.steplength=prev.steplength;
							m.stepduration=prev.stepduration;
						}
						else
						{
							_currStepLength=m.steplength;
							_currStepDuration=min(2.5,m.stepduration);
						}

						_footPrints[dispType]->pop_front();
						_allPrints[dispType]->push_back(m);
						cout<<"footype"<<dispType<<"  update("<<_currGaitType<<","<<_currStepLength<<","<<_currStepDuration<<")"<<endl;
						//cout<<"time difference"<<(m.currstart-_timer.timeEllapsed())<<endl;

						if(_currStepDuration==0)
							_currStepDuration=0.1;
						if(fabs(newangle)>=0.3 && winmode==2)
						{}
						else if(_prevGaitType==kGaitJumping && _currGaitType==kGaitJumping)
						{
							if(jumpflag==true)
							{
								retbool= _anim->DoUpdateStep(_currGaitType,_currStepLength,_currStepDuration,winmode);
								if(_currGaitType==kGaitWalkBack || _currGaitType==kGaitTiptoeBack || _currGaitType==kGaitLimpBack || _currGaitType==kGaitMoonWalk )
									step-=_currStepLength;
								else
									step+=_currStepLength;
								jumpflag=false;
							}
							else if(jumpflag==false)
								jumpflag=true;
						}
						else
						{
							step+=_currStepLength;

							retbool= _anim->DoUpdateStep(_currGaitType,_currStepLength,_currStepDuration,winmode);
						}
						_prevGaitType = _currGaitType;

						//GLCamera *cam = _glWindow->getCamera();
						//cam->modifyRotations(Vector3d(0,-newangle, 0));

						if(retbool==true) 
						{
							if(dispType ==0) dispType=1;
							else if(dispType==1) dispType=0;
							cout<<"disptype new main:"<<dispType<<endl;
						}
					}	
				}
				else
				{
					if(playmode==0 && walkflag==0)
					{
						cout<<"in else mode"<<endl;
						retbool=_anim->DoUpdateStep(kGaitWalking,0,0.1,winmode);
						walkflag++;
					}
					else if(playmode==1)
					{
						walkflag=0;
						/*UdpTransmitSocket transmitSocket(IpEndpointName(EndPoint->address, PORT_UDP_IN+1));
						char buffer[1024];
						osc::OutboundPacketStream p( buffer, 1024 );
						p<<osc::BeginBundleImmediate<<osc::BeginMessage("/step")<<_currStepLength<<osc::EndMessage<<osc::EndBundle;
						transmitSocket.Send(p.Data(), p.Size());*/
						_anim->DoUpdateStep(_currGaitType,_currStepLength,_currStepDuration,winmode);
					}
				}

				_lastTime = _currTime;
				render();
			}
		}
		else if(winmode==3)
		{
			if(dt>=0.033)
			{
				if(!_newpoints.empty())
				{
					AutoPoint m=_newpoints.front();
					if(autoflag==true)
					{
						if(_personstate->getCurrentFrame() == _personstate->getTotalFrame()-1)
						{
							AutoPoint prev= _allpoints.back();
							cout<<"next:"<<prev.x2<<","<<prev.y2<<":"<<m.x2<<","<<m.y2<<endl;
							_maze->clearpath();
							for (int i=0; i<35; i++)
							{
								motionMaze[i]=motion_type[i];
							}
							_maze->solveMaze(0, 0, prev.x2, prev.y2, m.x2,m.y2,motionMaze);

							_personstate->clearPath();

							for (int i=0; i<_maze->path.size(); i++)
							{
								_personstate->addPost(_maze->path[i].z, _maze->path[i].x, WALK);
								cout<<"type:"<<_maze->motion_path[i]<<endl;
							}
							_personstate->construct_motion();
							_personstate->setCurrentFrame(0);

							_newpoints.pop_front();
							_allpoints.push_back(m);
						}
					}
					else
					{
						_maze->solveMaze(0, 0,11,0,m.x2 ,m.y2,motionMaze);
						cout<<"solved"<<endl;
						_personstate->clearPath();
						for (int i=0; i<_maze->path.size(); i++)
						{
							_personstate->addPost(_maze->path[i].z, _maze->path[i].x, WALK);
							cout<<"type:"<<_maze->motion_path[i]<<endl;
						}
						_personstate->construct_motion();
						autoflag=true;
						cout<<"first:"<<m.x2<<","<<m.y2<<endl;
						_newpoints.pop_front();
						_allpoints.push_back(m);
					}
				}
				if(autoflag==true)
				{
					_lastTime = _currTime;
					render();
					this->updateCamera(dt);
				}
			}
		}
	}
	glPopMatrix();
}

void App4::onIdle() {
	//ensureMainWindowSelected();
	glutPostRedisplay();
}

void App4::onReshape(int w, int h) {
	glViewport(0, 0, w, h);

	_glWindow->onResize(w, h);
}

void App4::onMouse(int button, int state, int x, int y) {
	int btn = MOUSE_LBUTTON;
	int evt = MOUSE_UP;

	if (button == GLUT_RIGHT_BUTTON) btn = MOUSE_RBUTTON;
	else if (button == GLUT_MIDDLE_BUTTON) btn = MOUSE_MBUTTON;
	else if (button == GLUT_WHEEL_DOWN) btn = MOUSE_WHEEL_DOWN;
	else if (button == GLUT_WHEEL_UP) btn = MOUSE_WHEEL_UP;

	if (state == GLUT_DOWN) evt = MOUSE_DOWN;

	_glWindow->onMouseEvent(evt, btn, x, y);
}

void App4::onMouseMoved(int x, int y) {
	_glWindow->onMouseEvent(MOUSE_DRAG, -1, x, y);
}

void App4::onKeyEvent(int key) {

	_glWindow->destroy();
	glutDestroyWindow(win);
	escflag=true;
	init(true);
}

void App4::updateCamera(double dt) {
	if(winmode==1 || winmode==2)
	{
		// move camera to follow target
		GLCamera *cam = _glWindow->getCamera();
		Vector3d target;
		_anim->GetRootPosition(&target.x, &target.y, &target.z);
		target.y = 0;
		float dz = (target.z - cam->getTarget().z);
		float dx = (target.x - cam->getTarget().x);
		//cout << dz << endl;

		static double dtAccum = 0;
		dtAccum += dt;

		if (dtAccum >= 0.008) {
			if (fabs(dz) > 0.1) {
				target.setZ(dz);
				target.setX(dx);
				cam->translateTarget(target);
			}
			dtAccum = 0;
		}
	}
	else if(winmode==3)
	{
		GLCamera *cam = _glWindow->getCamera();
		Vector3d target;
		target.x = _personstate->getStoredNodeZ(0);
		target.z = _personstate->getStoredNodeX(0);
		target.y = 0;
		float dz = (target.z - cam->getTarget().z);
		float dx = (target.x - cam->getTarget().x);
		//cout << dz << endl;

		static double dtAccum = 0;
		dtAccum += dt;

		if (dtAccum >= 0.008) {
			if (fabs(dz) > 0.1) {
				target.setZ(dz);
				target.setX(dx);
				cam->translateTarget(target);
			}
			dtAccum = 0;
		}
	}
}

//void App4::update(double dt) {
//	if (_startWalking) {
//		_elapsedTime += dt;
//	}
//
//	this->updateCamera(dt);
//
//	this->updateFoot(dt, FOOT_LEFT);
//	this->updateFoot(dt, FOOT_RIGHT);
//
//	//cout << _myGaitType << " " << _myStepLength << " " << _myStepDuration << endl;
//    static double dtAccum = 0;
//    dtAccum += dt;
//
//    if (dtAccum >= 0.008) {
//	    if (_anim->DoUpdateStep(_currGaitType, _currStepLength, _currStepDuration)) {
//		    _currGaitType = _nextGaitType;
//		    _currStepLength = _nextStepLength;
//            if (_currGaitType == kGaitHopping)
//                _currStepDuration = _nextStepDuration * 3.0;
//            else
//		    _currStepDuration = _nextStepDuration * 2.0;
//            //_currStepDuration = 1.0;
//	    }
//        dtAccum = 0;
//    }
//    /*if (_anim->DoUpdateStep(_myGaitType, 0.0, 1.0)) {
//    }*/
//
////	_fa->update(dt);
//
//	//float swingDuration = 0.7;
//	//float stepLength = 20;
//
//	//_fa->setFootSwingDuration(swingDuration);
//	//_fa->setFootSwingLength(stepLength);
//}
//
//void App4::updateFoot(double dt, int type) {
//	if (_currFoot[type] != NULL) {
//		if (_currFoot[type]->down) {
//			static const double footStayThreshold = 1.2;
//			if (_footStayTime < footStayThreshold) {
//				_footStayTime += dt;
//				if (_footStayTime > footStayThreshold) {
////					_fa->setFootSwingLength(0);
//					_stopWalking = true;
//				}
//			}
//		}
//		else {
//			//_prevFoot[type] = _currFoot[type];
//			_prevFoot[type] = _sa.addFootStep(type, *_currFoot[type]);
//			_currFoot[type] = NULL;
//
//			_footPrints[type]->pop_front();
//			_footStayTime = 0;
//			//cout << "pop = " << _prevFoot[type]->stepId << endl;
//		}
//	}
//	else {
//		if (!_footPrints[type]->empty()) {
//			FootStepModel &foot = _footPrints[type]->front();
//
//			if (!foot.down) {
//				cout << "Foot was up before picked from the queue." << endl;
//				//return;
//			}
//
//			if (!_startWalking && foot.stepId == _firstStepId) {
//				_startWalking = true;
//				_elapsedTime = 0;
//			}
//
//			_currFoot[type] = &foot;
//
//			if (_prevFoot[type] != NULL) {
//				if (_stopWalking) {
////					_fa->reset();
//					_stopWalking = false;
//				}
//
//				const FootContact &c1 = _prevFoot[type]->drag.back();
//				const FootContact &c2 = _currFoot[type]->drag.front();
//
//				float delta = fabs(c2.y - c1.y);
//
//				if (delta > 0.5) { // dont bother, the finger moves back from up to bottom
//				}
//				else {
//					delta = clamp(delta, 0, 0.2);
//					float swingDuration = _currFoot[type]->startTime - _prevFoot[type]->endTime;
//					float stepLength = delta * UNITSCALE_Y;
//
//                    //cout << "step length before = " << stepLength << endl;
//
//					static const float maxStepLength = 36.0;
//					static const float maxStepDuration = 1.0;
//
//					swingDuration = clamp(swingDuration, 0, maxStepDuration + 0.5);
//					stepLength = clamp(stepLength, 0, maxStepLength);
//
//					// normalize input
//					_nextStepLength = (stepLength / maxStepLength);
//					_nextStepDuration = swingDuration / maxStepDuration;
//
//					cout << "step length = " << _nextStepLength << ", duration = " << _nextStepDuration << endl;
//
//					//cout << "duration = " << swingDuration << ", length = " << stepLength << endl;
//					//_fa->setFootSwingDuration(swingDuration);
//					//_fa->setFootSwingLength(stepLength);
//
//					//const LocomotionModel& lm = _sa.getLocomotionModel();
//
//					//if (lm.gaitType == kGaitWalking) {
//					//	//cout << "walking with alpha = " << lm.alpha << " contact = " << lm.contactDuration << endl;
//					//	if (lm.alpha >= 0) {
//					//		((Walk*)_fa)->setContactDuration(lm.contactDuration);
//					//		((Walk*)_fa)->setOverlapAlpha(lm.alpha);
//					//	}
//					//}
//					//else if (lm.gaitType == kGaitRunning) {
//					//	cout << "running with alpha = " << lm.alpha << endl;
//					//	if (lm.alpha >= -0.55) {
//					//		((Walk*)_fa)->setContactDuration(lm.contactDuration);
//					//		((Walk*)_fa)->setOverlapAlpha(lm.alpha);
//					//	}
//					//}
//					//else if (lm.gaitType == kGaitHopping) {
//					//	((Hop*)_fa)->setContactDuration(lm.contactDuration);
//					//	((Hop*)_fa)->setAlpha(lm.alpha);
//					//}
//				}
//			}
//		}
//	}
//}
//
//void App4::updateAnimation(double dt) {
//}
//
//void App4::updateFootAnimation(double dt, int type) {
//}
//
//void App4::updateFootUsingHistory(double dt, int type) {
//}
//
void App4::render() {
	double x, y, z;
	if(winmode==1 ||winmode==2)
	{
		_anim->GetRootPosition(&x,&y,&z);
		//cout<<"pos:"<<x<<","<<y<<","<<z<<","<<endl;
		_glWindow->draw(_currGaitType,x,y,z);
	}
	else if(winmode==3)
	{
		x = _personstate->getStoredNodeZ(0);
		z = _personstate->getStoredNodeX(0);
		y = 0;
		_glWindow->draw(kGaitWalking,x,y,z);
	}

}

void App4::draw() {
	static const Color leftFootColor(1.0f, 0.2f, 0.2f, 1.0f);
	static const Color rightFootColor(0.2f, 1.0f, 0.2f, 1.0f);

	//this->drawFoot(_footDrawable[FOOT_LEFT].pos , _footDrawable[FOOT_LEFT].rot, leftFootColor, _footDrawable[FOOT_LEFT].radius * 0.02);
	//this->drawFoot(_footDrawable[FOOT_RIGHT].pos, _footDrawable[FOOT_RIGHT].rot, rightFootColor, _footDrawable[FOOT_RIGHT].radius * 0.02);
}
//
//void App4::drawFoot(Point3d center, Vector3d rot, Color color, float radius) {
//	static const float footWidth = 0.25f * 20;
//	static const float footLength = 0.5f * 20;
//	static const float footThickness = 0.1f * 20;
//
//	Point3d boxMin(-0.5f * footWidth + center.x, center.y                , -0.5f * footLength + center.z);
//	Point3d boxMax( 0.5f * footWidth + center.x, center.y + footThickness,  0.5f * footLength + center.z);
//
//	glColor3f(color.r, color.g, color.b);
//
//	glPushMatrix();
//	{
//		Point3d t = center;
//		if (rot.x > 0) {
//			t.z -= footLength * 0.5f;
//		}
//		else if (rot.x < 0) {
//			t.z += footLength * 0.5f;
//		}
//		glTranslatef(t.x, t.y, t.z);
//		glRotatef(-rot.x, 1, 0, 0);
//		glRotatef(rot.y, 0, 1, 0);
//		glRotatef(rot.z, 0, 0, 1);
//		glTranslatef(-t.x, -t.y, -t.z);
//		GLUtils::drawWireFrameBox(boxMin, boxMax);
//	}
//	glPopMatrix();
//
//	glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
//	GLUtils::drawDisk(radius, Vector3d(center.x, 0.001, center.z), Point3d(0, 1, 0));
//}

void App4::ProcessMessage(const ReceivedMessage& m, const IpEndpointName& remoteEndPoint) {
	try {
		//cout<<"pattern "<<m.AddressPattern()<<endl;
		if (strcmp(m.AddressPattern(), "/reqid") == 0) {
			char temp[16];
			remoteEndPoint.AddressAsString(temp);
			cout << "receiving id request from: " << temp << " on port: " << remoteEndPoint.port << endl;
			UdpTransmitSocket transmitSocket(IpEndpointName(remoteEndPoint.address, PORT_UDP_IN+1));
			EndPoint = new IpEndpointName(ADDRESS,PORT_UDP_IN);
			EndPoint->address=remoteEndPoint.address;
			cout<<"Address:"<<(EndPoint->address)<<endl;
			char buffer[1024];
			osc::OutboundPacketStream p( buffer, 1024 );

			p<<osc::BeginBundleImmediate<<osc::BeginMessage("/ackid")<<winmode<<osc::EndMessage<<osc::EndBundle;

			transmitSocket.Send(p.Data(), p.Size());
			cout<<"Sending ack"<<endl;
		}
		else if(winmode==1 || winmode==2)
		{
			if (strcmp(m.AddressPattern(), "/foot") == 0) {
				//cout<<"Inside Foot"<<endl;
				ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
				float x = (arg++)->AsFloat();
				float y = (arg++)->AsFloat();
				int phase= (arg++)->AsInt32();
				int type=(arg++)->AsInt32();
				//int stepId=(arg++)->AsInt32();
				float timestamp = (arg++)->AsFloat();
				float radius = (arg++)->AsFloat();
				int mode=(arg++)->AsInt32();
				float angle = (arg++)->AsFloat();
				int other= (type+1)%2;
				if(abs(_footPrints[type]->size() - _footPrints[other]->size()) > 1)
				{
					cout<<"error in code :"<<type<<" "<<_footPrints[type]->size()<<" "<<other<<" "<<_footPrints[other]->size()<<endl;
					throw "error in code";
				}
				else {}
				//cout<<"correct:"<<type<<" "<<_footPrints[type]->size()<<" "<<other<<" "<<_footPrints[other]->size()<<endl;

				if(phase==0)
				{
					cout<<"foot x:"<<x<<" y:"<<y<<" phase:"<<phase<<" type:"<<type<<" timestamp"<<setprecision(7)<<timestamp<<" mode:"<<mode<<endl;
					FootStepNew m;
					FootStepNew missed;
					if(typeBegan!=type)
					{
						//ignoreflag[typeBegan]=true;
						missed.x=x;
						missed.y=y;
						missed.down=true;
						missed.type=typeBegan;
						missed.startTime=timestamp-0.1;
						missed.radius=radius;
						missed.mode=mode;
						missed.angle=angle;

					}
					m.x =x;
					m.y= y;
					//m.stepId=stepId;
					m.down=true;
					m.type=type;
					m.startTime=timestamp;
					m.radius = radius;
					m.mode=mode;
					m.angle=angle;
					m.currstart=_timer.timeEllapsed();
					if(!_allPrints[type]->empty() || !_footPrints[type]->empty())
					{	
						FootStepNew prev;
						if(!_footPrints[type]->empty())
						{
							prev = _footPrints[type]->back() ;
							if(prev.down==true)
							{
								_footPrints[type]->pop_back();
								prev.down=false;
								prev.endx=prev.x+0.01;
								prev.endy=prev.y+0.01;
								prev.endTime=prev.startTime+0.001;
								_footPrints[type]->push_back(prev);
								cout<<"same began received again"<<endl;
								typeEnd=other;
							}
						}
						else
							prev = _allPrints[type]->back() ;


						m.steplength =(fabs(m.y - prev.y)* UNITSCALE_Y)/36.0;
						m.stepduration= m.startTime- prev.endTime;
						if(m.stepduration>2.5)
							m.stepduration=2.5;
						if(typeBegan!=type)
						{
							missed.steplength=(fabs(missed.y - prev.y)* UNITSCALE_Y)/36.0;
							missed.stepduration= missed.startTime- prev.endTime;
							_footPrints[typeBegan]->push_back(missed);
							numberSteps[typeBegan]++;
						}
					}
					else
					{
						m.steplength=0.1;
						m.stepduration=0.1;
					}
					if(m.mode==2 || m.mode==3)
					{
						if(!_footPrints[type]->empty())
						{
							FootStepNew old = _footPrints[type]->back() ;
							old.mode=m.mode;
							_footPrints[type]->pop_back();
							_footPrints[type]->push_back(old);
						}
						if(!_footPrints[other]->empty())
						{
							FootStepNew oldother = _footPrints[other]->back() ;
							oldother.mode=m.mode;
							_footPrints[other]->pop_back();
							_footPrints[other]->push_back(oldother);
						}
						//if(!_footPrints[type]->empty())
						//{
						//	FootStepNew old = _footPrints[type]->back() ;
						//	/*if(old.down ==true )
						//	{
						//	_footPrints[type]->pop_back();
						//	cout<<"popped"<<endl;
						//	numberSteps[type]--;
						//	}*/
						//}

					}
					else
					{
						/*if(!_footPrints[type]->empty())
						{
						FootStepNew old = _footPrints[type]->back() ;*/
						/*if(old.down ==true )
						{
						_footPrints[type]->pop_back();
						cout<<"popped"<<endl;
						numberSteps[type]--;
						}*/
						//}
						_footPrints[type]->push_back(m);
						numberSteps[type]++;

					}
					typeBegan=other;
					//cout<<"step added type"<<type<<" "<<_footPrints[type]->size() <<" "<<_allPrints[type]->size()<<endl;
					//_allPrints[type]->push_back(m);
					//cout<<"footprints :"<<m.type<<" :"<<_footPrints[type]->size()<<endl;
					//cout<<"allprints :"<<m.type<<" :"<<_allPrints[type]->size()<<endl;
				}
				else if(phase==3)
				{/*
				 cout<<"before:"<<x_pt<<" "<<y_pt<<endl;
				 x_pt+=x;
				 y_pt+=y;
				 cout<<"after:"<<x_pt<<" "<<y_pt<<endl;
				 drawpoint.push_back(std::make_pair(x_pt/2.0 ,y_pt/2.0));
				 x_pt=0;
				 y_pt=0;*/
					cout<<"foot x:"<<x<<" y:"<<y<<" phase:"<<phase<<" type:"<<type<<" angle:"<<angle<<" timestamp"<<setprecision(7)<<timestamp<<" mode"<<mode<<endl;
					FootStepNew m,prev;
					/*for(std::vector<FootStepNew *>::iterator it = _footPrints.begin(); it != _footPrints.end(); ++it)
					{
					if((*it)->down==true)
					{
					(*it)->endTime=timestamp;
					(*it)->down=false;
					break;
					}
					}
					*/
					//numberSteps[type] > _allPrints[type]->size()
					/*if(typeEnd==type)
					{*/
					/*if(ignoreflag[typeEnd]==true)
					{
					if(typeEnd==type)
					ignoreflag[typeEnd]=false;
					else if(typeEnd!=type)
					{
					endflag=true;
					cout<<"step popped"<<endl;
					_footPrints[type]->pop_back();
					ignoreflag[typeEnd]=false;
					}
					}*/

					if(typeEnd!=type)
					{
						if(!_footPrints[typeEnd]->empty())
						{
							prev=_footPrints[typeEnd]->back();
							if(prev.down==true)
							{
								prev.endTime=timestamp;
								prev.down=false;
								prev.angle=angle;
								prev.endx=x;
								prev.endy=y;
								prev.mode=mode;
								prev.radius_end = radius;
								prev.currend=_timer.timeEllapsed();
								_footPrints[typeEnd]->pop_back();
								_footPrints[typeEnd]->push_back(prev);
							}
							else
								throw "error in prev";
						}
						else
							throw "error in prev: footprint not found";
					}

					if(!_footPrints[type]->empty())
					{
						m=_footPrints[type]->back();
						//cout<<"m is down:"<<m.down<<endl;
						if(m.down==true)
						{
							m.endTime=timestamp;
							m.down=false;
							m.angle=angle;
							m.endx=x;
							m.endy=y;
							m.mode=mode;
							m.radius_end = radius;
							m.currend=_timer.timeEllapsed();
							_footPrints[type]->pop_back();
							_footPrints[type]->push_back(m);
							//_allPrints[type]->pop_back();

							//cout<<"step ended type"<<type<<" "<<_footPrints[type]->size() <<" "<<_allPrints[type]->size()<<endl;
							//_allPrints[type]->push_back(m);
						}
						else
						{
							cout<<"step ignored"<<numberSteps[type]<<" "<<_allPrints[type]->size()<<endl;
							m.endTime=timestamp;
							m.down=false;
							m.angle=angle;
							m.endx=x;
							m.endy=y;
							m.mode=mode;
							m.radius_end = radius;
							m.radius=radius;
							m.startTime=m.endTime-0.01;
							m.stepduration=0.1;
							m.steplength=0.1;
							m.type=type;
							m.x=x;
							m.y=y;
							_footPrints[type]->push_back(m);
							if(typeBegan==type)
							{
								cout<<"type beg error"<<endl;
								typeBegan=other;
							}
						}
					}
					else
					{
						cout<<"step ignored out"<<numberSteps[type]<<" "<<_allPrints[type]->size()<<endl;
						m.endTime=timestamp;
						m.down=false;
						m.angle=angle;
						m.endx=x;
						m.endy=y;
						m.mode=mode;
						m.radius_end = radius;
						m.radius=radius;
						m.startTime=m.endTime-0.01;
						m.stepduration=0.1;
						m.steplength=0.1;
						m.type=type;
						m.x=x;
						m.y=y;
						_footPrints[type]->push_back(m);
						if(typeBegan==type)
						{
							cout<<"type beg error"<<endl;
							typeBegan=other;
						}
					}

					typeEnd=other;
					if(winmode==2 && sendflag==true)
					{
						double rootposx, rootposy, rootposz;
						_anim->GetRootPosition(&rootposx,&rootposy,&rootposz);
						UdpTransmitSocket transmitSocket(IpEndpointName(remoteEndPoint.address, PORT_UDP_IN+1));
						char buffer[1024];
						osc::OutboundPacketStream p( buffer, 1024 );
						p<<osc::BeginBundleImmediate<<osc::BeginMessage("/step")<<rootposx<<rootposz<<osc::EndMessage<<osc::EndBundle;
						transmitSocket.Send(p.Data(), p.Size());
						sendflag=false;
					}

				}
			}
			else if (strcmp(m.AddressPattern(), "/play") == 0) {
				ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
				int pmode=(arg++)->AsInt32();
				if(pmode==6)
					playmode=0;
				else if(pmode==5)
					playmode=1;

				if(winmode==2)
				{
					double rootposx, rootposy, rootposz;
					_anim->GetRootPosition(&rootposx,&rootposy,&rootposz);
					UdpTransmitSocket transmitSocket(IpEndpointName(remoteEndPoint.address, PORT_UDP_IN+1));
					char buffer[1024];
					osc::OutboundPacketStream p( buffer, 1024 );
					p<<osc::BeginBundleImmediate<<osc::BeginMessage("/step")<<rootposx<<rootposz<<osc::EndMessage<<osc::EndBundle;
					transmitSocket.Send(p.Data(), p.Size());
					sendflag=false;
				}

			}
			else if (strcmp(m.AddressPattern(), "/rotate") == 0) {

				ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
				float angle = (arg++)->AsFloat();
				cout<<"Inside Rotate : "<<angle<<endl;
				GLCamera *cam = _glWindow->getCamera();
				float newangle=(angle-last_angle_cam)/60.0;
				//cout<<"Angle of rotation"<<newangle<<endl;
				if(winmode==2)
					cam->modifyRotations(Vector3d(0,-newangle, 0));
				_anim->rotate_character(0-newangle);
				last_angle_cam=angle;

			}
			else if (strcmp(m.AddressPattern(), "/spin") == 0) {
				cout<<"in spin"<<endl;
				FootStepNew old = _footPrints[type]->back();
				old.mode=2;
				_footPrints[type]->pop_back();
				_footPrints[type]->push_back(old);

			}
		}
		else if(winmode==3)
		{
			if (strcmp(m.AddressPattern(), "/automode") == 0) {
				//cout<<"Inside Foot"<<endl;
				ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
				float x1 = (arg++)->AsFloat();
				float y1 = (arg++)->AsFloat();
				float x2 = (arg++)->AsFloat();
				float y2 = (arg++)->AsFloat();
				AutoPoint mes;
				mes.x1=x1;
				mes.y1=y1;
				mes.x2=x2;
				mes.y2=y2;
				_newpoints.push_back(mes);
			}
		}
	}
	catch (osc::Exception& e) {
		// any parsing errors such as unexpected argument types, or
		// missing arguments get thrown as exceptions.
		cout << "error while parsing message: "
			<< m.AddressPattern() << ": " << e.what() << endl;
	}
}

