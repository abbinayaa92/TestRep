//
//  CPBPressureTouchGestureRecognizer.h
//  touchMotion
//
//  Created by Abbinayaa on 10/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

//  CPBPressureTouchGestureRecognizer.h
//  PressureSensitiveButton

#import <UIKit/UIKit.h>

#define CPBPressureNone         0.0f
#define CPBPressureLight        0.3f
#define CPBPressureMedium       0.4f
#define CPBPressureHard         0.6f
#define CPBPressureInfinite     2.0f

@interface CPBPressureTouchGestureRecognizer : UIGestureRecognizer {
@public
    float pressure;
    float minimumPressureRequired;
    float maximumPressureRequired;
    
@private
    float pressureValues[50];
    uint currentPressureValueIndex;
    uint setNextPressureValue;
}

@property (readonly, assign) float pressure;
@property (readwrite, assign) float minimumPressureRequired;
@property (readwrite, assign) float maximumPressureRequired;

@end