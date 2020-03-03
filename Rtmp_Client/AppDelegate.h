//
//  AppDelegate.h
//  Rtmp_Client
//
//  Created by wang guijun on 15/7/22.
//  Copyright (c) 2015年 wang guijun. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LoginViewController;
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (retain, nonatomic) LoginViewController *loginViewController;
@property (retain, nonatomic) UINavigationController *navigationController;
@end

