//
//  MainView.m
//  skiatestapp
//
//  Created by Keks on 14/11/15.
//  Copyright © 2015 Perspex. All rights reserved.
//

#include "common.h"

#import <Foundation/Foundation.h>
#import "UIKit/UIKit.h"
#import "MainView.h"
using namespace libperspesk;

@implementation MainView;

- (id) init{
    self = [super init];
    _layer = [CALayer layer];
    _layer.bounds = CGRectMake(0., 0., 320., 480.);
    //_layer.backgroundColor = [[UIColor redColor] CGColor];
    [self.layer addSublayer: _layer];
    return self;
}

- (void) draw {
    RenderTarget*target = CreateRenderTarget((void*)CFBridgingRetain(_layer));
    RenderingContext*ctx = target->CreateRenderingContext();
    ctx->Settings.Opacity=1;
    PerspexBrush brush;
    memset(&brush, 0, sizeof(PerspexBrush));
    brush.Color = 0xffff00ff;
    brush.Opacity = 1;
    SkRect rc;
    rc.fLeft = rc.fTop = 0;
    rc.fRight = 320;
    rc.fBottom = 480;
    
    DrawRectangle(ctx, &brush, &rc, 0);
    rc.fLeft = 50;
    rc.fTop = 150;
    rc.fBottom = 200;
    rc.fRight = 200;
    brush.Color = 0xff00ff00;
    DrawRectangle(ctx, &brush,  &rc, 10);
    delete ctx;
    delete target;
}
@end

@implementation MainViewController

- (id) init{
    self = [super init];
    return self;
}

- (void) loadView
{
    MainView*mv= [MainView new];
    [mv draw];
    self.view = mv;
    
}


@end