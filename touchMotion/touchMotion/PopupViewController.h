//
//  PopupViewController.h
//  touchMotion
//
//  Created by Abbinayaa on 3/2/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol MJSecondPopupDelegate;


@interface PopupViewController : UIViewController
{
    UITextField *ipAdd;
    
}
@property (retain, nonatomic) IBOutlet UITextField *ipAdd;
@property (nonatomic, assign) id <MJSecondPopupDelegate> delegate;

- (IBAction)connectPopup:(id)sender;
@end

@protocol MJSecondPopupDelegate<NSObject>
@optional
- (void)cancelButtonClicked:(PopupViewController*)popupViewController;

@end