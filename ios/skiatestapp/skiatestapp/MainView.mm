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
    self = [super initWithFrame: [UIScreen mainScreen].applicationFrame context:(EAGLContext*)CFBridgingRelease(GetPerspexEAGLContext())];
    self.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    self.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    self.drawableStencilFormat = GLKViewDrawableStencilFormat8;
            _color = 0;
    
    [NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(display) userInfo:nil repeats:YES];
    return self;
    

}

static int _frames;
static int _framesTotal = 0;
static double _lastfps = 0;
static double _renderTime = 0;

- (void) drawRect:(CGRect)rect
{
    double started = CACurrentMediaTime();
    RenderTarget*target = CreateRenderTarget((void*)CFBridgingRetain(self));
    RenderingContext*ctx = target->CreateRenderingContext();
    ctx->Settings.Opacity=1;
    
    PerspexBrush brush;
    memset(&brush, 0, sizeof(PerspexBrush));
    //for(int c=0; c<10; c++){
    brush.Opacity = 1;
    brush.Color = 0xff0000ff;
    SkRect rc;
    rc.fLeft = rc.fTop = 0;
    rc.fRight = 1536;
    rc.fBottom = 2048;
    DrawRectangle(ctx, &brush, &rc, 0);
    
    
    float rads = ((float)_framesTotal)/30;
    float cos = cosf(rads);
    float sin = sinf(rads);
    
    float transform[] = {cos, sin, 400, -sin, cos, 400};
    SetTransform(ctx, transform);
    
    brush.Color = 0xffff00ff ^ _color;
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
    //}
    delete ctx;
    delete target;
    _frames++;
    _framesTotal++;
    _renderTime += CACurrentMediaTime() - started;
    double now = CACurrentMediaTime();
    if(now-_lastfps>1)
    {
        printf("FPS: %i, average render time %f\n", (int)(_frames/(now - _lastfps)), _renderTime/_frames);
        _lastfps = now;
        _renderTime = 0;
        _frames = 0;
    
    }
    
}

- (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    _color = _color ^ 0xffffff;
    //[self draw];
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
    //[mv draw];
    self.view = mv;
    
}


@end