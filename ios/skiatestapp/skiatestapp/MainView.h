//
//  MainView.h
//  skiatestapp
//
//  Created by Keks on 14/11/15.
//  Copyright © 2015 Perspex. All rights reserved.
//

#ifndef MainView_h
#define MainView_h

@interface MainView : UIView
{
    CALayer* _layer;
}
- (id) init;
- (void) draw;

@end

@interface MainViewController: UIViewController
- (id) init;
- (void) loadView;
@end

#endif /* MainView_h */
