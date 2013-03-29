//
//  TouchMotionAppDelegate.m
//  touchMotion
//
//  Created by Abbinayaa on 6/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "TouchMotionAppDelegate.h"
#import "MotionView.h"
#import "VVOSC/VVOSC.h"
#import "VVBasics/VVBasics.h"

@implementation TouchMotionAppDelegate
TouchMotionAppDelegate *_appDelegate;
@synthesize window = _window;
@synthesize view;
@synthesize pointrec;

@synthesize connState = _connState;

void AppDelegate_sendPacket(float x, float y,int phase,int type,float time, float radius,int mode, float imageAngle) {
   
    OSCMessage *msg ;
    if (mode==-1) 
    {
        msg = [[OSCMessage alloc] initWithAddress:@"/rotate"];
        [msg addFloat:x];
        
    }
//    else if(mode==2)
//    {
//         msg = [[OSCMessage alloc] initWithAddress:@"/foot"];
//        [msg addFloat:0];
//        [msg addFloat:0]; 
//        [msg addInt:0];// TODO: add double type
//        [msg addInt:0];
//        [msg addFloat:0];
//        [msg addFloat:0];
//        [msg addInt:mode];
//        [msg addFloat:0];
//    }
    else if(mode==6 || mode==5)
    {
        msg = [[OSCMessage alloc] initWithAddress:@"/play"];
        [msg addInt:mode];
    }
    else if(mode==7)
    {
        msg = [[OSCMessage alloc] initWithAddress:@"/automode"];
        [msg addFloat:x];
        [msg addFloat:y];
        [msg addFloat:time];
        [msg addFloat:radius];
    }
    else
    {
         msg = [[OSCMessage alloc] initWithAddress:@"/foot"];
        [msg addFloat:x];
        [msg addFloat:y];
        [msg addInt:phase];// TODO: add double type
        [msg addInt:type];
        [msg addFloat:time];
        [msg addFloat:radius];
        [msg addInt:mode];
        [msg addFloat:imageAngle];
    }
        
        OSCPacket *packet = [[OSCPacket alloc] initWithContent:msg];
        [_appDelegate sendThisPacket:packet];
}

void setupNetworkConnection(NSString *ipadd)
{
    // [self prepareConnectionWithIpAddress:@"192.168.1.101" port:1234];
    //ipadd
    [_appDelegate prepareConnectionWithIpAddress:ipadd port:1234];
   // [_appDelegate prepareConnectionWithIpAddress:@"172.23.102.200" port:1234];
    OSCMessage *msg = [OSCMessage createWithAddress:@"/reqid"];
    OSCPacket *packet = [OSCPacket createWithContent:msg];
    NSLog(@"sending first packet..");
    [_appDelegate->_outPort sendThisPacket:packet];
}

void ResetPoint(void)
{
    [_appDelegate resetpt];
}

- (void) resetpt
{
    pointrec.x=0.3;
    pointrec.y=24;
}

- (void)prepareConnectionWithIpAddress:(NSString *)ipAddress port:(int)port {
    if (_oscManager == nil) {
        _oscManager = [[OSCManager alloc] init];
        [_oscManager setDelegate:self];
    }
    
    // delete previous input/output connections
    [_oscManager deleteAllInputs];
    [_oscManager deleteAllOutputs];
    
    _outPort = [_oscManager createNewOutputToAddress:ipAddress atPort:port];
    _inPort = [_oscManager createNewInputForPort:port+1];
}

-(void)sendThisPacket:(OSCPacket*)packet {
    [_outPort sendThisPacket:packet];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    _appDelegate = self;
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    view =[[MotionView alloc] init];
    
    [[self window] setRootViewController:view];
    [self.window makeKeyAndVisible];
    pointrec.x=0.3;
    pointrec.y=24;
    _oscManager = nil;
    _connState = NOT_CONNECTED;
    //[self setupNetworkConnection];
//    NSDate* starttime=[NSDate date];
//    int count=0;
//    int mul=1;
//    for(int i=1;i<1000000;i++)
//    {
//        for(int j=1;j<100;j++)
//		{
//            count+=i+j;
//            mul*=i*j;
//            mul*=count;
//            count+=mul;
//		}
//    }
//    NSLog(@"time difference %f result %d , %d",[starttime timeIntervalSinceNow], count, mul);
    //setupNetworkConnection(@"192.168.247.101");
    
    return YES;
}



// called by delegate on message
- (void) receivedOSCMessage:(OSCMessage *)m	{
    NSLog(@"in received OSC message");
    if (_connState == NOT_CONNECTED) {
        if ([[m address] isEqualToString:@"/ackid"]) {
            NSLog(@"connection established..");
            _connState = CONNECTED;
            OSCValue *value=[ m value];
            int mode=[value intValue];
            view.winmode=mode;
            [view resetmode];
            NSLog(@"finished reset to mode %d",mode);

        }
    }
    else if (_connState == CONNECTED) {
         if ([[m address] isEqualToString:@"/step"])
         {
             int num=[m valueCount];
             NSLog(@"number= %d",num);
             NSMutableArray *values = [m valueArray];
            OSCValue *value1=[values objectAtIndex:0 ];
            OSCValue *value2=[values objectAtIndex:1];
             float x=[value1 doubleValue];
             float y=[value2 doubleValue];
             NSLog(@"x %f y: %f, prev x: %f, prevy: %f",x,y,pointrec.x,pointrec.y);
             updatestep(x-pointrec.x,y-pointrec.y);
             pointrec.x=x;
             pointrec.y=y;
             
         }
        if ([[m address] isEqualToString:@"/mode"])
        {
            OSCValue *value=[ m value];
            int mode=[value intValue];
            view.winmode=mode;
            [view resetmode];
            NSLog(@"finished reset to mode %d",mode);
            
        }
    }
}


- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
}

@end

