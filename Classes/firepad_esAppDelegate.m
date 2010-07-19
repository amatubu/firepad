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
    // スクリーンが回転したかどうかを受けとる
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didRotate:)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:nil];

    // スクリーンの初期サイズを調べておく
    UIScreen *screen = [UIScreen mainScreen];
    screenSize = screen.bounds.size;

    [glView startAnimation];
    return YES;
}

- (void) didRotate:(NSNotification *)notification {
    UIDeviceOrientation orientation = [[notification object] orientation];

    // TODO: 回転をスムーズに
    switch ( orientation ) {
        case UIDeviceOrientationLandscapeLeft:
            NSLog( @"LandscapeLeft" );
            glView.bounds = CGRectMake( 0.0f, 0.0f,
                                        screenSize.height,
                                        screenSize.width );
            glView.transform = CGAffineTransformMakeRotation( M_PI * 90.0f / 180.0f );
            break;
        case UIDeviceOrientationLandscapeRight:
            NSLog( @"LandscapeRight" );
            glView.bounds = CGRectMake( 0.0f, 0.0f,
                                        screenSize.height,
                                        screenSize.width );
            glView.transform = CGAffineTransformMakeRotation( - M_PI * 90.0f / 180.0f );
            break;
        case UIDeviceOrientationPortrait:
            NSLog( @"Portrait" );
            glView.bounds = CGRectMake( 0.0f, 0.0f,
                                        screenSize.width,
                                        screenSize.height );
            glView.transform = CGAffineTransformMakeRotation( 0.0f );
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            NSLog( @"PortraitUpsideDown" );
            glView.bounds = CGRectMake( 0.0f, 0.0f,
                                        screenSize.width,
                                        screenSize.height );
            glView.transform = CGAffineTransformMakeRotation( M_PI );
            break;
        default:
            break;
    }
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
