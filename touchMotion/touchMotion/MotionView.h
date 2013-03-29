//
//  MotionView.h
//  touchMotion
//
//  Created by Abbinayaa on 6/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

void updatestep(float x,float y);
@interface MotionView : UIViewController
{
    UIView *myView;
    UIView *mapView;
    UIImageView *map;
    UILabel *xCoord;
    UILabel *yCoord;
    UIImageView *target;
    CGPoint targetPoint;
    CGPoint prevPoint;
    CGPoint startPoint;
    CGPoint startpt3;
    CGPoint endpt3;
    CGPoint translate;
    CGFloat imageAngle;
    CGPoint prevtouch;
    UISegmentedControl *segmode;
    UISwitch *playmode;
    UIButton *reset;
    int typeBegan;
    int typeEnded;
    int total;
    int mode;
    int flag0;
    int flag1;
    int winmode;
    float radius_end;
    UIImageView *latest;
    CGRect mapFrame;
}
@property (retain, nonatomic) IBOutlet UIView *myView;
@property (retain, nonatomic) IBOutlet UIView *mapView;
@property (retain, nonatomic) IBOutlet UILabel *xCoord;
@property (retain, nonatomic) IBOutlet UILabel *yCoord;
@property (retain, nonatomic) UIImageView *target;
@property (retain, nonatomic) UIImageView *map;
@property (retain, nonatomic) IBOutlet UISegmentedControl *segmode;
@property (retain, nonatomic) IBOutlet UISwitch *playmode;
@property (retain, nonatomic) IBOutlet UIButton *reset;
@property CGPoint startPoint;
@property CGPoint targetPoint;
@property CGPoint startPt3;
@property CGPoint endpt3;
@property CGPoint translate;
@property CGPoint prevPoint;
@property CGPoint prevtouch;
@property CGFloat imageAngle;
@property CGRect mapFrame;
@property NSInteger typeBegan;
@property NSInteger typeEnded;
@property NSInteger total;
@property NSInteger mode;
@property CGFloat radius_end;
@property NSInteger flag,flag0,flag1,winmode;
@property CGPoint touchPoint1,touchPoint2;
@property (retain, nonatomic) IBOutlet UIImageView *latest;


- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) update: (float) x with: (float) y;
-(void) resetmode;
-(UIImage*)scaleToSize:(CGSize)size;
-(IBAction) segmentedControlIndexChanged;
- (IBAction)toggleSwitch:(id)sender;
- (IBAction)Resetclick;

@end
