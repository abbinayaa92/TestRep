//
//  TouchMotionAppDelegate.h
//  touchMotion
//
//  Created by Abbinayaa on 6/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "VVOSC/OSCPacket.h"
#import "VVOSC/OSCManager.h"
#import "VVOSC/OSCOutPort.h"
#import "VVOSC/OSCInPort.h"

@class MotionView;
//@class OSCManager, OSCInPort, OSCOutPort,OSCPacket;
@class NSData;


enum ConnectionState {
    NOT_CONNECTED,
    WAITING_FOR_CONNECTION,
    CONNECTED,
};

// interface for sending Foot packet
void AppDelegate_sendPacket(float x,float y,int phase,int type,float time, float radius,int mode,float imageAngle);
void setupNetworkConnection(NSString* ipadd);
void ResetPoint(void);

@interface TouchMotionAppDelegate : UIResponder <UIApplicationDelegate>
{
    MotionView *view;
    
    OSCManager *_oscManager;
    OSCInPort *_inPort;
    OSCOutPort *_outPort;
    CGPoint pointrec;
    enum ConnectionState _connState;
}

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic,strong) MotionView *view;
@property (readwrite) enum ConnectionState connState;
@property CGPoint pointrec;


- (void)prepareConnectionWithIpAddress:(NSString*)ipAddress port:(int)port;
- (void)sendThisPacket:(OSCPacket*)packet;
- (void) resetpt;

@end
