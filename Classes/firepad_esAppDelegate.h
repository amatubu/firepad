//
//  firepad_esAppDelegate.h
//  firepad_es
//
//  Created by naoki iimura on 7/5/10.
//  Copyright naoki iimura 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface firepad_esAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EAGLView *glView;
    CGSize screenSize;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@end

