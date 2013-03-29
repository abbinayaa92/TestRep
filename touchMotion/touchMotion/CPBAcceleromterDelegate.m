//
//  CPBAcceleromterDelegate.m
//  touchMotion
//
//  Created by Abbinayaa on 10/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

//
//  CPBAcceleromterDelegate.m
//  iPan
//
//  Created by Anthony Picciano on 1/25/12.
//  Copyright (c) 2012 Crispin Porter + Bogusky. All rights reserved.
//

#import "CPBAcceleromterDelegate.h"
#include "SynthesizeSingleton.h"

@implementation CPBAcceleromterDelegate
@synthesize acceleration;

SYNTHESIZE_SINGLETON_FOR_CLASS(CPBAcceleromterDelegate)

-(void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)newAcceleration
{
    self.acceleration = newAcceleration;
    [[NSNotificationCenter defaultCenter] postNotificationName:CPB_ACCELERATION_EVENT object:self];
}

@end


