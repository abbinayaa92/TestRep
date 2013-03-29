//
//  CPBAcceleromterDelegate.h
//  touchMotion
//
//  Created by Abbinayaa on 10/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

//
//  CPBAcceleromterDelegate.h
//  iPan
//
//  Created by Anthony Picciano on 1/25/12.
//  Copyright (c) 2012 Crispin Porter + Bogusky. All rights reserved.
//

#import <Foundation/Foundation.h>
#define CPB_ACCELERATION_EVENT @"accellerationEvent"

@interface CPBAcceleromterDelegate : NSObject <UIAccelerometerDelegate>

@property (nonatomic, retain) UIAcceleration *acceleration;

+ (CPBAcceleromterDelegate *)sharedCPBAcceleromterDelegate;

-(void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;

@end

