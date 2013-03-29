//
//  MotionView.m
//  touchMotion
//
//  Created by Abbinayaa on 6/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "MotionView.h"
#import "TouchMotionAppDelegate.h"
#import "OneFingerRotationGestureRecognizer.h"
#import "KTOneFingerRotationGestureRecognizer.h"
#import <QuartzCore/QuartzCore.h>
#import "PopupViewController.h"
#import "UIViewController+MJPopupViewController.h"

@implementation MotionView
MotionView *mview;
@synthesize myView;
@synthesize xCoord;
@synthesize yCoord;
@synthesize segmode;
@synthesize startPoint;
@synthesize typeBegan;
@synthesize typeEnded;
@synthesize total;
@synthesize mode;
@synthesize radius_end;
@synthesize latest;
@synthesize imageAngle;
@synthesize flag,flag0,flag1,winmode;
@synthesize touchPoint1,touchPoint2;
@synthesize playmode;
@synthesize reset;
@synthesize targetPoint;
@synthesize target;
@synthesize prevPoint;
@synthesize translate;
@synthesize prevtouch;
@synthesize map;
@synthesize mapView;
@synthesize mapFrame;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    typeEnded=0;
    typeBegan=0;
    total=0;
    mode=0;
    imageAngle=0;
    flag=0;
    flag0=1;
    flag1=1;
    prevPoint.x=0;
    prevPoint.y=0;
    translate.x=0;
    translate.y=0;
    prevtouch.x=0;
    prevtouch.y=0;
    winmode=1;
    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)cancelButtonClicked:(PopupViewController *)aSecondDetailViewController
{
    [self dismissPopupViewControllerWithanimationType:MJPopupViewAnimationFade];
}


#pragma mark - CircularGestureRecognizerDelegate protocol

- (void) rotation: (CGFloat) angle
{
    // calculate rotation angle
    imageAngle += angle;
    if (imageAngle > 360)
        imageAngle -= 360;
    else if (imageAngle < -360)
        imageAngle += 360;
    
    // rotate image and update text field
    for(UIView *subview in myView.subviews)
    {
        if(subview.tag==total)
            subview.transform =CGAffineTransformMakeRotation((imageAngle) *  M_PI / 180);
        //subview.transform =CGAffineTransformRotate(subview.transform,(imageAngle) *  M_PI / 180);
    }
    AppDelegate_sendPacket(imageAngle,0,0,0,0,0,-1,0);
    if(winmode==1 || winmode==2)
        myView.transform = CGAffineTransformMakeRotation((0-imageAngle) *  M_PI / 180);
    if(winmode==2)
        target.transform = CGAffineTransformMakeRotation((imageAngle) *  M_PI / 180);
    
    // NSLog(@"angle rotation %f",imageAngle);
    
}

- (void) finalAngle: (CGFloat) angle
{
    flag=1;
    NSLog(@"angle rotation %f",imageAngle);
    //  target.transform = CGAffineTransformRotate(target.transform, (angle) *  M_PI / 180);
//    if(winmode==2)
//    {
//        //    UIWindow *mainWindow = [[UIApplication sharedApplication] keyWindow];
//        //    CGPoint pointInWindowCoords = [mainWindow convertPoint:target.center fromWindow:nil];
//        //    CGPoint pointInViewCoords = [myView convertPoint:pointInWindowCoords fromView:mainWindow];
//        //    CGPoint Anchor=CGPointMake(pointInViewCoords.x/myView.bounds.size.width, pointInViewCoords.y/myView.bounds.size.height);
//        //    [self setAnchorPoint:Anchor forView:myView];
//        //    myView.transform = CGAffineTransformRotate(myView.transform, (0-angle) *  M_PI / 180);
//        for(UIView *subview in myView.subviews)
//        {
//            if(subview.tag==total)
//                subview.transform =CGAffineTransformMakeRotation((0) *  M_PI / 180);
//            //subview.transform =CGAffineTransformRotate(subview.transform,(imageAngle) *  M_PI / 180);
//        }
//    }
    
    
}
- (void) update: (float) x with:(float) y
{
//    if(prevPoint.x==0 && prevPoint.y==0)
//    {
//        prevPoint.x+=x;
//        prevPoint.y+=y;
//        
//    }
//    else{
//        NSLog(@"x %f y %f",x,y);

//        targetPoint.x+=translate.x;
//        targetPoint.y+=translate.y;
//        
//        myView.transform = CGAffineTransformTranslate(myView.transform, translate.x, -translate.y);
//        
//        UIWindow *mainWindow = [[UIApplication sharedApplication] keyWindow];
//        CGPoint pointInWindowCoords = [mainWindow convertPoint:target.center fromWindow:nil];
//        CGPoint pointInViewCoords = [myView convertPoint:pointInWindowCoords fromView:mainWindow];
//        CGPoint Anchor=CGPointMake(pointInViewCoords.x/myView.bounds.size.width, pointInViewCoords.y/myView.bounds.size.height);
//        [self setAnchorPoint:Anchor forView:myView];
//        prevPoint.x+=x;
//        prevPoint.y+=y;
//    }
    
for(UIView *subview in mapView.subviews)
      {
          if(subview.tag==-1)
          {
          NSLog(@"inside loop");
        [subview performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:NO];
          }
      }
    
    translate.x= mapFrame.origin.x-(x*0.0914);
    translate.y=mapFrame.origin.y-(y*0.0905);
    NSLog(@"translate x:%f y:%f",translate.x,translate.y);
    
    CGRect myImageRect = CGRectMake(0,0,200,200);
    map = [[UIImageView alloc] initWithFrame:myImageRect];
    map.tag=-1;
    UIImage* img = [UIImage imageNamed:@"maze10_old.jpg"];
    mapFrame = CGRectMake(translate.x, translate.y, 54.84,54.33);
    CGImageRef imageRef = CGImageCreateWithImageInRect([img CGImage], mapFrame);
    [map setImage:[UIImage imageWithCGImage:imageRef]];
    CGImageRelease(imageRef);
    //[mapView addSubview:map];
    //[mapView sendSubviewToBack:map];
    [mapView performSelectorOnMainThread:@selector(addSubview:) withObject:map waitUntilDone:YES];
    
}



void updatestep(float x,float y)
{
    [mview update:x with:y];
}

- (void)spin:(UISwipeGestureRecognizer *)gestureRecognizer {
    NSLog(@"in spin");
    mode=2;
    if(gestureRecognizer.state==UIGestureRecognizerStateEnded)
    {
        //AppDelegate_sendPacket(0,0,0,0,0,0,2,0);
        
    }
}

- (void)twoFingerPinch:(UIPinchGestureRecognizer *)recognizer
{
    // NSLog(@"Pinch scale: %f", recognizer.scale);
    int count;
    count=[recognizer numberOfTouches];
    if(count==2)
    {
        touchPoint1 = [recognizer locationOfTouch:0 inView:self.view];
        touchPoint2 = [recognizer locationOfTouch:1 inView:self.view];
    }
    if(recognizer.state==UIGestureRecognizerStateEnded)
    {
        //        if(recognizer.scale<0.6)
        //            AppDelegate_sendPacket(0,0,0,0,0,0,2);
        //        else
        if(recognizer.scale>1.2 && fabs(touchPoint1.x-touchPoint2.x)<=100)
        {
            NSLog(@"pinch point %f , %f",touchPoint1.x,touchPoint2.x);
            //AppDelegate_sendPacket(0,0,0,0,0,0,4,0);
            mode=4;
        }
        else if(recognizer.scale>1.2)
        {
            //AppDelegate_sendPacket(0,0,0,0,0,0,3,0);
            mode=3;
        }
    }
}

#pragma mark - Helper methods


- (void) setupGestureRecognizer
{
    
    UISwipeGestureRecognizer *recognizer;
    recognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(spin:)];  // <-- Note the colon after the method name
    [recognizer setDirection:UISwipeGestureRecognizerDirectionLeft];
    recognizer.cancelsTouchesInView=NO;
    recognizer.delaysTouchesBegan=NO;
    recognizer.delaysTouchesEnded=NO;
    recognizer.delegate=self;
    [self.view addGestureRecognizer:recognizer];
    
    UIPinchGestureRecognizer *twoFingerPinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingerPinch:)];
    twoFingerPinch.cancelsTouchesInView=NO;
    twoFingerPinch.delaysTouchesBegan=NO;
    twoFingerPinch.delaysTouchesEnded=NO;
    twoFingerPinch.delegate=self;
    [self.view addGestureRecognizer:twoFingerPinch];
    
    // calculate center and radius of the control
    CGPoint midPoint = CGPointMake(myView.frame.origin.x + myView.frame.size.width / 2,
                                   myView.frame.origin.y + myView.frame.size.height / 2);
    CGFloat outRadius = myView.frame.size.width / 2;
    
    // outRadius / 3 is arbitrary, just choose something >> 0 to avoid strange
    // effects when touching the control near of it's center
    OneFingerRotationGestureRecognizer *gestureRecognizer = [[OneFingerRotationGestureRecognizer alloc] initWithMidPoint: midPoint
                                                                                                             innerRadius: outRadius / 3
                                                                                                             outerRadius: outRadius
                                                                                                                  target: self];
    gestureRecognizer.cancelsTouchesInView=NO;
    //[gestureRecognizer requireGestureRecognizerToFail:recognizer];
    gestureRecognizer.delaysTouchesBegan=NO;
    gestureRecognizer.delaysTouchesEnded=NO;
    gestureRecognizer.delegate=self;
    [self.view addGestureRecognizer: gestureRecognizer];
    
    
    
}

-(void) drawBackground
{
    // CGRect myImageRect = CGRectMake(-6000, -12000, 22000, 38000);
    //CGRect myImageRect = CGRectMake(9000,20500,5012.5, -7517.5);
    
    //[myView sendSubviewToBack:target];
    if(winmode==3)
    {
        UIImage *constimage=[UIImage imageNamed:@"blue_arrow_3.png"];
        CGSize imageSize = CGSizeMake(constimage.size.width, constimage.size.height);
        UIGraphicsBeginImageContext( imageSize );
        [constimage drawInRect:CGRectMake(0,0,imageSize.width,imageSize.height)];
        UIImage* myImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        target= [[UIImageView alloc] initWithImage:myImage];
        target.tag=-5;
        targetPoint= CGPointMake(2883.4, 9915.8);
        // targetPoint= CGPointMake(384, 490);
        // ... and center it at the touch location
        [target setCenter:CGPointMake(383.4, 488.3)];
       // [self.view addSubview:target];
         [self.view performSelectorOnMainThread:@selector(addSubview:) withObject:target waitUntilDone:NO];
        UIWindow *mainWindow = [[UIApplication sharedApplication] keyWindow];
        NSLog(@"target centre: %f %f",target.center.x,target.center.y);
        CGPoint pointInWindowCoords = [mainWindow convertPoint:target.center fromWindow:nil];
        CGPoint pointInViewCoords = [myView convertPoint:pointInWindowCoords fromView:mainWindow];
         NSLog(@"target point in view x:%f y:%f",pointInViewCoords.x,pointInViewCoords.y);
        CGPoint Anchor=CGPointMake(pointInViewCoords.x/myView.bounds.size.width, pointInViewCoords.y/myView.bounds.size.height);
        [self setAnchorPoint:Anchor forView:myView];
        prevtouch.x=pointInViewCoords.x;
        prevtouch.y=pointInViewCoords.y;
    }
    else if(winmode==2)
    {
        UIImage *constimage=[UIImage imageNamed:@"blue_arrow.png"];
        CGSize imageSize = CGSizeMake(constimage.size.width, constimage.size.height);
        UIGraphicsBeginImageContext( imageSize );
        [constimage drawInRect:CGRectMake(0,0,imageSize.width,imageSize.height)];
        UIImage* myImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        target= [[UIImageView alloc] initWithImage:myImage];
        target.tag=-5;
        targetPoint= CGPointMake(100, 100);
        // targetPoint= CGPointMake(384, 490);
        // ... and center it at the touch location
        [target setCenter:CGPointMake(100, 880)];
       // [self.view addSubview:target];
         [self.view performSelectorOnMainThread:@selector(addSubview:) withObject:target waitUntilDone:NO];
        [self setAnchorPoint:targetPoint forView:mapView];

    }
    
    
    //  [image setImage:[UIImage imageNamed:@"blue_grid_2.png"]];
    if(winmode==1)
    {
        CGRect myImageRect = CGRectMake(0,0,self.view.frame.size.width+16000,self.view.frame.size.height+16000);
        UIImageView *image = [[UIImageView alloc] initWithFrame:myImageRect];
        image.tag=-1;
        [image setImage:[UIImage imageNamed:@"blue_grid_2.png"]];
       // [myView addSubview:image];
        //[myView sendSubviewToBack:image];
         [myView performSelectorOnMainThread:@selector(addSubview:) withObject:image waitUntilDone:NO];
         [myView performSelectorOnMainThread:@selector(sendSubviewToBack:) withObject:image waitUntilDone:NO];
    }
    else if(winmode==2 )
    {

        CGRect myImageRect = CGRectMake(0,0,200,200);
        map = [[UIImageView alloc] initWithFrame:myImageRect];
        map.tag=-1;
        UIImage* img = [UIImage imageNamed:@"maze10_old.jpg"];
        mapFrame = CGRectMake(194.3, 610.28, 54.84,54.33);
        CGImageRef imageRef = CGImageCreateWithImageInRect([img CGImage], mapFrame);
        [map setImage:[UIImage imageWithCGImage:imageRef]];
        CGImageRelease(imageRef);
       // [mapView addSubview:map];
       // [mapView sendSubviewToBack:map];
        [mapView performSelectorOnMainThread:@selector(addSubview:) withObject:map waitUntilDone:NO];
        [mapView performSelectorOnMainThread:@selector(sendSubviewToBack:) withObject:map waitUntilDone:NO];
        
        CGRect myImageRect1 = CGRectMake(0,0,self.view.frame.size.width+16000,self.view.frame.size.height+16000);
        UIImageView *image1 = [[UIImageView alloc] initWithFrame:myImageRect1];
        image1.tag=-1;
        [image1 setImage:[UIImage imageNamed:@"blue_grid_2.png"]];
        //[myView addSubview:image1];
       // [myView sendSubviewToBack:image1];
        [myView performSelectorOnMainThread:@selector(addSubview:) withObject:image1 waitUntilDone:NO];
        [myView performSelectorOnMainThread:@selector(sendSubviewToBack:) withObject:image1 waitUntilDone:NO];
    }
    else if(winmode==3)
    {
        CGRect myImageRect = CGRectMake(0,0,self.view.frame.size.width+1732,self.view.frame.size.height+2520);
        UIImageView *image = [[UIImageView alloc] initWithFrame:myImageRect];
        image.tag=-1;
        [image setImage:[UIImage imageNamed:@"maze10.jpg"]];
        NSLog(@"add image");
      //  [myView addSubview:image];
      //  [myView sendSubviewToBack:image];
        [myView performSelectorOnMainThread:@selector(addSubview:) withObject:image waitUntilDone:NO];
        [myView performSelectorOnMainThread:@selector(sendSubviewToBack:) withObject:image waitUntilDone:NO];
    }
  
    
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    mview=self;
    
    
    PopupViewController *pop = [[PopupViewController alloc] initWithNibName:@"PopupViewController" bundle:nil];
    pop.delegate = self;
    [self presentPopupViewController:pop animationType:MJPopupViewAnimationFade];
    
    [self.view setMultipleTouchEnabled:YES];
    // [self addRotationGestureToView:[self view]];
    self.view.userInteractionEnabled = YES;
    
    [self setupGestureRecognizer];
    winmode=2;
    //  CGRect newrect=CGRectMake(-10000, -20000, 24000, 34000);
    // CGRect newrect= CGRectMake(-1000,500, 5012.5, -7517.5);
    NSLog(@"width %f height %f",self.view.bounds.size.width,self.view.bounds.size.height);
    CGRect newrect;
    if(winmode==2)
    {
        NSLog(@"mode:%d",winmode);
        //newrect = CGRectMake(-2500,-9427.5,self.view.frame.size.width+5500,self.view.frame.size.height+9000);
        newrect=CGRectMake(-8000, -8000, self.view.frame.size.width+16000, self.view.frame.size.width+16000);
        CGRect maprect = CGRectMake(0, 780, 200, 200);
        mapView = [[UIView alloc] initWithFrame:maprect];
        mapView.tag=-10;
        [[self view] addSubview:mapView];
    }
    else if(winmode==3)
    {
        NSLog(@"mode:%d",winmode);
        newrect = CGRectMake(-766,-2950,self.view.frame.size.width+1732,self.view.frame.size.height+2520);
    }
    else if(winmode==1)
    {
        NSLog(@"mode:%d",winmode);
        newrect=CGRectMake(-8000, -8000, self.view.frame.size.width+16000, self.view.frame.size.width+16000);
    }
    myView = [[UIView alloc] initWithFrame:newrect];
    myView.tag=-100;
    [[self view] addSubview:myView];
    
    [self drawBackground];
    [myView setMultipleTouchEnabled:YES];
    myView.userInteractionEnabled = YES;
    [self.view sendSubviewToBack:myView];
    
}


- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


-(IBAction) segmentedControlIndexChanged{
    switch (self.segmode.selectedSegmentIndex) {
        case 0:
            mode=0;
            NSLog(@"index is %d",mode);
            break;
        case 1:
            mode=1;
            NSLog(@"index is %d",mode);
            break;
        default:
            break;
    }
}

- (IBAction)toggleSwitch:(id)sender {
    if(playmode.on)
        AppDelegate_sendPacket(0,0,0,0,0,0,5,0);
    else
        AppDelegate_sendPacket(0,0,0,0,0,0,6,0);
}

-(void) resetmode
{
    
    for(UIView *subview in self.view.subviews)
    {
        if(subview.tag==-100 || subview.tag==-5 || subview.tag==-10)
            [subview performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:NO];
    }
    CGRect newrect;
    if(winmode==2)
    {
        NSLog(@"mode:%d",winmode);
        //newrect = CGRectMake(-2500,-9427.5,self.view.frame.size.width+5500,self.view.frame.size.height+9000);
         newrect=CGRectMake(-8000, -8000, self.view.frame.size.width+16000, self.view.frame.size.width+16000);
        CGRect maprect = CGRectMake(0, 780, 200, 200);
        mapView = [[UIView alloc] initWithFrame:maprect];
        mapView.tag=-10;
        //[[self view] addSubview:mapView];
        [self.view performSelectorOnMainThread:@selector(addSubview:) withObject:mapView waitUntilDone:NO];
    }
    else if(winmode==3)
    {
         NSLog(@"mode:%d",winmode);
        newrect = CGRectMake(-766,-2950,self.view.frame.size.width+1732,self.view.frame.size.height+2520);
    }
    else if(winmode==1)
    {
        NSLog(@"mode:%d",winmode);
        newrect=CGRectMake(-8000, -8000, self.view.frame.size.width+16000, self.view.frame.size.width+16000);
    }
    
    myView = [[UIView alloc] initWithFrame:newrect];
    myView.tag=-100;
    //[[self view] addSubview:myView];
    [self.view performSelectorOnMainThread:@selector(addSubview:) withObject:myView waitUntilDone:YES];
    
    [self drawBackground];
    [myView setMultipleTouchEnabled:YES];
    myView.userInteractionEnabled = YES;
   // [self.view sendSubviewToBack:myView];
    [self.view performSelectorOnMainThread:@selector(sendSubviewToBack:) withObject:myView waitUntilDone:NO];
    
    [self.segmode setSelectedSegmentIndex:0];
    [playmode setOn:false];
    typeEnded=0;
    typeBegan=0;
    total=0;
    mode=0;
    imageAngle=0;
    flag=0;
    flag0=1;
    flag1=1;
    prevPoint.x=0;
    prevPoint.y=0;
    ResetPoint();
    
}
- (IBAction)Resetclick{
    
    for(UIView *subview in self.view.subviews)
    {
        if(subview.tag==-100 || subview.tag==-5)
            [subview removeFromSuperview];
    }
    CGRect newrect;
    if(winmode==2)
    {
        NSLog(@"mode:%d",winmode);
        //newrect = CGRectMake(-2500,-9427.5,self.view.frame.size.width+5500,self.view.frame.size.height+9000);
        newrect=CGRectMake(-8000, -8000, self.view.frame.size.width+16000, self.view.frame.size.width+16000);
        CGRect maprect = CGRectMake(0, 780, 200, 200);
        mapView = [[UIView alloc] initWithFrame:maprect];
        mapView.tag=-10;
        [[self view] addSubview:mapView];
    }
    else if(winmode==3)
    {
        NSLog(@"mode:%d",winmode);
        newrect = CGRectMake(-766,-2950,self.view.frame.size.width+1732,self.view.frame.size.height+2520);
    }
    else if(winmode==1)
    {
        NSLog(@"mode:%d",winmode);
        newrect=CGRectMake(-8000, -8000, self.view.frame.size.width+16000, self.view.frame.size.width+16000);
    }
    
    myView = [[UIView alloc] initWithFrame:newrect];
    myView.tag=-100;
    [[self view] addSubview:myView];
    
    [self drawBackground];
    [myView setMultipleTouchEnabled:YES];
    myView.userInteractionEnabled = YES;
    [self.view sendSubviewToBack:myView];
    
    [self.segmode setSelectedSegmentIndex:0];
    [playmode setOn:false];
    typeEnded=0;
    typeBegan=0;
    total=0;
    mode=0;
    imageAngle=0;
    flag=0;
    flag0=1;
    flag1=1;
    prevPoint.x=0;
    prevPoint.y=0; 
    ResetPoint();
    
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    return YES;
}

-(void)setAnchorPoint:(CGPoint)anchorPoint forView:(UIView *)view
{
    CGPoint newPoint = CGPointMake(view.bounds.size.width * anchorPoint.x, view.bounds.size.height * anchorPoint.y);
    CGPoint oldPoint = CGPointMake(view.bounds.size.width * view.layer.anchorPoint.x, view.bounds.size.height * view.layer.anchorPoint.y);
    
    newPoint = CGPointApplyAffineTransform(newPoint, view.transform);
    oldPoint = CGPointApplyAffineTransform(oldPoint, view.transform);
    
    CGPoint position = view.layer.position;
    
    position.x -= oldPoint.x;
    position.x += newPoint.x;
    
    position.y -= oldPoint.y;
    position.y += newPoint.y;
    
    view.layer.position = position;
    view.layer.anchorPoint = anchorPoint;
}


- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    
    NSSet *allTouches = [event allTouches];
    int count = [allTouches count];
    for(int i=0;i<[allTouches count];i++)
    {
        UITouch *theTouch = [[allTouches allObjects] objectAtIndex:i];
        if([theTouch phase]== 0)
        {
            if(winmode==1 ||winmode==2)
            {
                total++;
                startPoint = [theTouch locationInView:self.view];
                CGPoint startpointrot=[theTouch locationInView:myView];
                // struct Footstep foot;
                float time=[theTouch timestamp];
                xCoord.text = [NSString stringWithFormat:@"x = %f", startPoint.x];
                yCoord.text = [NSString stringWithFormat:@"y = %f", startPoint.y];
                float x = startPoint.x/self.view.bounds.size.width;
                float y = startPoint.y/self.view.bounds.size.height;
                int phase=[theTouch phase];
                float radius = [[theTouch valueForKey:@"pathMajorRadius"] floatValue];
                //int id= [theTouch self];
                //foot.stepId= [theTouch self];
                
                
                CGPoint newPoint= CGPointMake(startpointrot.x/myView.bounds.size.width, startpointrot.y/myView.bounds.size.height);
                    [self setAnchorPoint:newPoint forView:myView];
                
                CGSize imageSize;
                
                
                UIImage *constimage=[UIImage imageNamed:@"footprint.png"];
                imageSize = CGSizeMake(constimage.size.width * radius*5/40, constimage.size.height*radius*5/40);
                UIGraphicsBeginImageContext( imageSize );
                [constimage drawInRect:CGRectMake(0,0,imageSize.width,imageSize.height)];
                UIImage* myImage = UIGraphicsGetImageFromCurrentImageContext();
                UIGraphicsEndImageContext();
                
                // Create a new imageView for that image...
                UIImageView *imageView = [[UIImageView alloc] initWithImage:myImage];
                imageView.tag=total;
                
                // ... and center it at the touch location
                [imageView setCenter:startpointrot];
                imageView.userInteractionEnabled = YES;
                imageView.transform = CGAffineTransformMakeRotation(imageAngle *  M_PI / 180);
                [myView addSubview:imageView];
                
                
                // NSData* data=[[NSString stringWithFormat:@"Touch Began,x: %f,y: %f, phase: %d, id: %d",foot.x,foot.y,foot.phase,foot.stepId]dataUsingEncoding:NSUTF8StringEncoding];
                NSLog(@"touch began timestamp %f: %f, %f type: %d, phase: %d",time,x,y,typeBegan,phase);
                
                if(phase==0)
                {
                    
                    if(typeBegan==0)
                    {
                        if(flag0==1)
                        {
                            flag0=0;
                            AppDelegate_sendPacket(x , y,phase,typeBegan,time,radius*10,mode,0);
                            typeBegan=1;
                        }
                        else if (flag0==0)
                        {
                            flag0=0;
                            AppDelegate_sendPacket(x , y,phase,typeBegan,time,radius*10,mode,0);
                            if(typeEnded==0) typeEnded=1;
                            else if(typeEnded==1) typeEnded=0;
                            typeBegan=1;
                        }
                    }
                    else if(typeBegan==1)
                    {
                        if(flag1==1)
                        {
                            flag1=0;
                            AppDelegate_sendPacket(x , y,phase,typeBegan,time,radius*10,mode,0);
                            typeBegan=0;
                        }
                        else if (flag1==0)
                        {
                            flag1=0;
                            AppDelegate_sendPacket(x , y,phase,typeBegan,time,radius*10,mode,0);
                            if(typeEnded==0) typeEnded=1;
                            else if(typeEnded==1) typeEnded=0;
                            typeBegan=0;
                        }
                        
                    }
                }
            }
        else if(winmode==3)
        {
            startpt3 = [theTouch locationInView:myView];
            
        }
        }
    }
    
    
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *theTouch = [[event allTouches] anyObject];
    CGPoint touchLocation =
    [theTouch locationInView:self.view];
    // struct Footstep foot;
    int phase;
    float x,y;
    xCoord.text = [NSString stringWithFormat:@"x = %f", x];
    yCoord.text = [NSString stringWithFormat:@"y = %f", y];
    x = touchLocation.x/self.view.bounds.size.width;
    y = touchLocation.y/self.view.bounds.size.height;
    phase=[theTouch phase];
    float radius = [[theTouch valueForKey:@"pathMajorRadius"] floatValue];
    // foot.stepId= [theTouch self];
    
    if(radius!=0)
        radius_end=radius;
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    
    NSSet *allTouches = [event allTouches];
    int count = [allTouches count];
    for(int i=0;i<[allTouches count];i++)
    {
        
        UITouch *theTouch = [[allTouches allObjects] objectAtIndex:i];
        if(winmode==1 || winmode==2)
        {
        // struct Footstep foot;
        int phase=[theTouch phase];
        //    foot.stepId= [theTouch self];
        float time=[theTouch timestamp];
        //    unsigned int width=1024;
        //	unsigned int height=768;
        CGPoint endPoint = [theTouch locationInView:self.view];
        float x=endPoint.x/self.view.bounds.size.width;
        float y=endPoint.y/self.view.bounds.size.height;
        float radius = [[theTouch valueForKey:@"pathMajorRadius"] floatValue];
        xCoord.text = [NSString stringWithFormat:
                       @"start = %f, %f", startPoint.x, startPoint.y];
        yCoord.text = [NSString stringWithFormat:
                       @"end = %f, %f", endPoint.x, endPoint.y];
        
        if(phase==3)
        {
            NSLog(@"touch ended timestamp %f: %f, %f type: %d,phase %d, radius %f, mode %d",time,x,y,typeEnded,phase,radius_end,mode);
            if(typeEnded==0)
            {
                if(flag0==0)
                {
                    if(flag==0)
                        AppDelegate_sendPacket(x,y,phase,typeEnded,time,radius_end*10,mode,0);
                    else if(flag==1)
                    {
                        AppDelegate_sendPacket(x,y,phase,typeEnded,time,radius_end*10,mode,imageAngle);
                    }
                    flag0=1;
                    typeEnded=1;
                }
                
            }
            else if(typeEnded==1)
            {
                if(flag1==0)
                {
                    if(flag==0)
                        AppDelegate_sendPacket(x,y,phase,typeEnded,time,radius_end*10,mode,0);
                    else if(flag==1)
                    {
                        AppDelegate_sendPacket(x,y,phase,typeEnded,time,radius_end*10,mode,imageAngle);
                    }
                    flag1=1;
                    typeEnded=0;
                    
                }
                
            }
            
        }
        }
        else if(winmode==3)
        {
            endpt3=[theTouch locationInView:myView];
            NSLog(@"start:%f,%f, end: %f,%f,",startpt3.x/100, (35-startpt3.y/100), endpt3.x/100,(35-endpt3.y/100));
            AppDelegate_sendPacket(startpt3.x/100, (35-startpt3.y/100), 0, 0, endpt3.x/100, (35-endpt3.y/100), 7, 0);
            NSLog(@"end point: %f %f, prev touch: %f, %f",endpt3.x,endpt3.y,prevtouch.x,prevtouch.y);
             myView.transform = CGAffineTransformTranslate(myView.transform, prevtouch.x-endpt3.x, prevtouch.y-endpt3.y);
            prevtouch.x=endpt3.x;
            prevtouch.y=endpt3.y;
            
        }
    }
    mode=self.segmode.selectedSegmentIndex;
    
    
    for(UIView *subview in myView.subviews)
    {   if((subview.tag < total-1)&& (subview.tag>0))
        [subview removeFromSuperview];
    }
    
    // [super touchesEnded:touches withEvent:event];
    
}

- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    NSSet *allTouches = [event allTouches];
    int count = [allTouches count];
    for(int i=0;i<[allTouches count];i++)
    {
        
        UITouch *theTouch = [[allTouches allObjects] objectAtIndex:i];
        // struct Footstep foot;
        int phase=[theTouch phase];
        if(phase==4)
        {
            NSLog(@"in touch cancelled");
            if(typeEnded==0) typeEnded=1;
            else if(typeEnded==1) typeEnded=0;
        }
    }
}
@end
