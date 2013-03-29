//
//  PopupViewController.m
//  touchMotion
//
//  Created by Abbinayaa on 3/2/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#import "PopupViewController.h"
#import "TouchMotionAppDelegate.h"

@interface PopupViewController ()

@end

@implementation PopupViewController

@synthesize delegate;
@synthesize ipAdd;


- (IBAction)connectPopup:(id)sender
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(cancelButtonClicked:)]) { 
        NSLog(@"ipaddress: %@",ipAdd.text);
        setupNetworkConnection(ipAdd.text);
        [self.delegate cancelButtonClicked:self];
    }
}
@end
