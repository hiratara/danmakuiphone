//
//  DanmakuIphoneAppDelegate.m
//  DanmakuIphone
//
//  Created by たらら on 09/02/14.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "DanmakuIphoneAppDelegate.h"
#import "EAGLView.h"

@implementation DanmakuIphoneAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	glView.animationInterval = 1.0 / 60.0;
	[glView startAnimation];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 60.0;
}


- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
