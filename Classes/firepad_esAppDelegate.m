//
//  firepad_esAppDelegate.m
//  firepad_es
//
//  Created by naoki iimura on 7/5/10.
//  Copyright naoki iimura 2010. All rights reserved.
//

#import "firepad_esAppDelegate.h"
#import "EAGLView.h"

@implementation firepad_esAppDelegate

@synthesize window;
@synthesize glView;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
    [glView startAnimation];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)dealloc
{
    [window release];
    [glView release];

    [super dealloc];
}

@end
