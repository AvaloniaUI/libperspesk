//
//  main.m
//  skiatestapp
//
//  Created by Keks on 13/11/15.
//  Copyright © 2015 Perspex. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        
        @try{
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        }
        @catch(NSException* exception)
        {
            NSLog(@"Exception %@", [exception description]);
        }
    }
}
