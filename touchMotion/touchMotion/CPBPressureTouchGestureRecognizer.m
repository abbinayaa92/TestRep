//
//  CPBPressureTouchGestureRecognizer.m
//  touchMotion
//
//  Created by Abbinayaa on 10/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

//  CPBPressureTouchGestureRecognizer.m

#import <UIKit/UIGestureRecognizerSubclass.h>
#import "CPBPressureTouchGestureRecognizer.h"
#import "CPBAcceleromterDelegate.h"

#define kUpdateFrequency            60.0f
#define KNumberOfPressureSamples    3

@interface CPBPressureTouchGestureRecognizer (private)
- (void)setup;
@end

@implementation CPBPressureTouchGestureRecognizer
@synthesize pressure, minimumPressureRequired, maximumPressureRequired;

- (id)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self != nil) {
        [self setup];
    }
    return self;
}

- (id)init {
    self = [super init];
    if (self != nil) {
        [self setup];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)setup {
    minimumPressureRequired = CPBPressureNone;
    maximumPressureRequired = CPBPressureInfinite;
    pressure = CPBPressureNone;
    
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:0.001f / kUpdateFrequency];
    [[UIAccelerometer sharedAccelerometer] setDelegate:[CPBAcceleromterDelegate sharedCPBAcceleromterDelegate]];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(accelerateNotification:) name:CPB_ACCELERATION_EVENT object:nil];
}

#pragma -
#pragma UIAccelerometerDelegate methods

- (void)accelerateNotification:(NSNotification *)notification {
    
    UIAcceleration *acceleration = [[CPBAcceleromterDelegate sharedCPBAcceleromterDelegate] acceleration];
    
    int sz = (sizeof pressureValues) / (sizeof pressureValues[0]);
    
    // set current pressure value
    pressureValues[currentPressureValueIndex%sz] = acceleration.z;
    
    if (setNextPressureValue > 0) {
        
        // calculate average pressure
        float total = 0.0f;
        for (int loop=0; loop<sz; loop++) total += pressureValues[loop]; 
        float average = total / sz;
        
        // start with most recent past pressure sample
        if (setNextPressureValue == KNumberOfPressureSamples) {
            float mostRecent = pressureValues[(currentPressureValueIndex-1)%sz];
            pressure = fabsf(average - mostRecent);
        }
        
        // caluculate pressure as difference between average and current acceleration
        float diff = fabsf(average - acceleration.z);
        if (pressure < diff) pressure = diff;
        setNextPressureValue--;
        
        if (setNextPressureValue == 0) {
            if (pressure >= minimumPressureRequired && pressure <= maximumPressureRequired)
                self.state = UIGestureRecognizerStateRecognized;
            else
                self.state = UIGestureRecognizerStateFailed;
        }
    }
    
    currentPressureValueIndex++;
}

#pragma -
#pragma UIGestureRecognizer subclass methods

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    setNextPressureValue = KNumberOfPressureSamples;
    self.state = UIGestureRecognizerStatePossible;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event{
    //setNextPressureValue = KNumberOfPressureSamples;
    NSLog(@"in touc ended laa");
    self.state = UIGestureRecognizerStatePossible;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    self.state = UIGestureRecognizerStateFailed;
}

- (void)reset {
    pressure = CPBPressureNone;
    setNextPressureValue = 0;
    currentPressureValueIndex = 0;
}

@end
