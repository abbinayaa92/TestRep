//
//  KTOneFingerRotationGestureRecognizer.m
//
//  Created by Kirby Turner on 4/22/11.
//  Copyright 2011 White Peak Software Inc. All rights reserved.
//
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#import "KTOneFingerRotationGestureRecognizer.h"
#import <UIKit/UIGestureRecognizerSubclass.h>


@implementation KTOneFingerRotationGestureRecognizer

@synthesize rotation = rotation_;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
   // Fail when more than 1 finger detected.
   if ([[event touchesForGestureRecognizer:self] count] > 1) {
      [self setState:UIGestureRecognizerStateFailed];
   }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
   if ([self state] == UIGestureRecognizerStatePossible) {
      [self setState:UIGestureRecognizerStateBegan];
   } else {
      [self setState:UIGestureRecognizerStateChanged];
   }

   // We can look at any touch object since we know we 
   // have only 1. If there were more than 1 then 
   // touchesBegan:withEvent: would have failed the recognizer.
   UITouch *touch = [touches anyObject];

   // To rotate with one finger, we simulate a second finger.
   // The second figure is on the opposite side of the virtual
   // circle that represents the rotation gesture.

   UIView *view = [self view];
    UIView *subtarg;
    int max=0;
    for(UIView *subview in view.subviews)
    {  
        if(subview.tag>max)
        {
            max= subview.tag;
            subtarg=subview;
        }
        
    }
   CGPoint center = CGPointMake(CGRectGetMidX([subtarg bounds]), CGRectGetMidY([subtarg bounds]));
   CGPoint currentTouchPoint = [touch locationInView:subtarg];
   CGPoint previousTouchPoint = [touch previousLocationInView:subtarg];
   
   CGFloat angleInRadians = atan2f(currentTouchPoint.y - center.y, currentTouchPoint.x - center.x) - atan2f(previousTouchPoint.y - center.y, previousTouchPoint.x - center.x);
   
   [self setRotation:angleInRadians];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
   // Perform final check to make sure a tap was not misinterpreted.
   if ([self state] == UIGestureRecognizerStateChanged) {
      [self setState:UIGestureRecognizerStateEnded];
   } else {
      [self setState:UIGestureRecognizerStateFailed];
   }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
   [self setState:UIGestureRecognizerStateFailed];
}


//- (void) touchesBegan:(NSSet *) touches withEvent:(UIEvent *) event
//{
//    self.points = [NSMutableArray array];
//    CGPoint pt = [[touches anyObject] locationInView:self];
//    [self.points addObject:[NSValue valueWithCGPoint:pt]];
//}
//
//// Add each point to the array
//- (void) touchesMoved:(NSSet *) touches withEvent:(UIEvent *) event
//{
//    CGPoint pt = [[touches anyObject] locationInView:self];
//    [self.points addObject:[NSValue valueWithCGPoint:pt]];
//    [self setNeedsDisplay];
//}
//
//// At the end of touches, determine whether a circle was drawn
//- (void) touchesEnded:(NSSet *) touches withEvent:(UIEvent *) event
//{
//    if (!self.points) return;
//    if (self.points.count < 3) return;
//    
//    // Test 1: The start and end points must be between 60 pixels of each other
//    CGRect tcircle;
//    if (distance(POINT(0), POINT(self.points.count - 1)) < 60.0f)
//        tcircle = [self centeredRectangle];
//    
//    // Test 2: Count the distance traveled in degrees. Must fall within 45 degrees of 2 PI
//    CGPoint center = CGPointMake(CGRectGetMidX(tcircle), CGRectGetMidY(tcircle));
//    float distance = ABS(acos(dotproduct(centerPoint(POINT(0), center), centerPoint(POINT(1), center))));
//    for (int i = 1; i < (self.points.count - 1); i++)
//        distance += ABS(acos(dotproduct(centerPoint(POINT(i), center), centerPoint(POINT(i+1), center))));
//    if ((ABS(distance - 2 * M_PI) < (M_PI / 4.0f))) circle = tcircle;
//    
//    [self setNeedsDisplay];
//}

@end
