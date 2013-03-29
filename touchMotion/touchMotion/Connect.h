//
//  Connect.h
//  touchMotion
//
//  Created by Abbinayaa on 8/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UDPEcho.h"

enum FootType {
    FOOT_UNKNOWN = -1,
    FOOT_LEFT = 0,
    FOOT_RIGHT = 1,
};
struct Footstep{
    int stepId;
    int phase;
    enum FootType type;
    float x;
    float y;
    float radius;
    double timestamp;
    
    //FootStep() : stepId(-1), phase(-1), type(FOOT_UNKNOWN), x(0), y(0), radius(0), timestamp(0) {} 
};

@interface Connect : NSObject <UDPEchoDelegate>
{
    UDPEcho *       _echo;
    NSTimer *       _sendTimer;
    NSUInteger      _sendCount;
}
@property (nonatomic, retain, readwrite) UDPEcho *      echo;
@property (nonatomic, retain, readwrite) NSTimer *      sendTimer;
@property (nonatomic, assign, readwrite) NSUInteger     sendCount;


- (BOOL)runServerOnPort:(NSUInteger)port;
- (BOOL)runClientWithHost:(NSString *)host port:(NSUInteger)port;





@end
