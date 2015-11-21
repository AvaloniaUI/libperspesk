//
//  MainView.h
//  skiatestapp
//
//  Created by Keks on 14/11/15.
//  Copyright © 2015 Perspex. All rights reserved.
//

#ifndef MainView_h
#define MainView_h
#import <GLKit/GLKView.h>

@interface MainView : GLKView
{
    unsigned int _color;
}
- (id) init;

@end

@interface MainViewController: UIViewController
- (id) init;
- (void) loadView;
@end

#endif /* MainView_h */
