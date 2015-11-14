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

extern "C" void* GetPerspexMethodTable();

@implementation MainView;

- (id) init{
    GetPerspexMethodTable();
    self = [super init];
    _layer = [CAEAGLLayer layer];
    _layer.bounds = CGRectMake(0., 0., 320., 480.);
    _layer.backgroundColor = [[UIColor redColor] CGColor];
    _color = 0;
    [self.layer addSublayer: _layer];
    return self;
}

- (void) draw {
    RenderTarget*target = CreateRenderTarget((void*)CFBridgingRetain(_layer));
    RenderingContext*ctx = target->CreateRenderingContext();
    ctx->Settings.Opacity=1;
    PerspexBrush brush;
    memset(&brush, 0, sizeof(PerspexBrush));
    brush.Color = 0xffff00ff ^ _color;
    brush.Opacity = 1;
    SkRect rc;
    rc.fLeft = rc.fTop = 0;
    rc.fRight = 320;
    rc.fBottom = 480;
    
    DrawRectangle(ctx, &brush, &rc, 0);
    rc.fLeft = 50;
    rc.fTop = 00;
    rc.fBottom = 200;
    rc.fRight = 270;
    brush.Color = 0xff00ff00 ^ _color;
    DrawRectangle(ctx, &brush,  &rc, 10);
    
    rc.fLeft = 340;
    rc.fRight = 400;
    DrawRectangle(ctx, &brush,  &rc, 10);
    
    delete ctx;
    delete target;
}

- (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    _color = _color ^ 0xffffff;
    [self draw];
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