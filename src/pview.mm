#include "common.h"
#include "mac/SkCGUtils.h"
#import "UIKit/UIKit.h"

namespace libperspesk
{
    class MacRenderTarget : public RenderTarget
    {
    public:
        CALayer*Layer;
        SkBitmap Bitmap;
        SkAutoTUnref<SkSurface> Surface;
        int Width, Height;
        MacRenderTarget(void*nativeWindow)
        {
            Layer = (CALayer*)CFBridgingRelease(nativeWindow);
            Layer.opaque = true;
            Layer.anchorPoint = CGPointMake(0,0);
            Width=Height=0;
        }
        
        void Resize()
        {
            int w = Layer.bounds.size.width;
            int h = Layer.bounds.size.height;
            if(w<1)
                w=1;
            if(h<1)
                h=1;
            if(Width != w || Height != h)
            {
                Width = w;
                Height =h;
            }
            Bitmap.allocN32Pixels(Width, Height);
            Surface.reset(SkSurface::NewRasterDirect(Bitmap.info(), Bitmap.getPixels(), Bitmap.rowBytes()));
        }
        
        void Present()
        {
            CGImageRef img = SkCreateCGImageRef(Bitmap);
            Layer.contents = (__bridge id)img;
            CGImageRelease(img);
        }
        
        
        class MacRenderingContext : public RenderingContext
        {
        public:
            MacRenderTarget*Target;
            MacRenderingContext(MacRenderTarget* target)
            {
                Target = target;
                Canvas = target->Surface->getCanvas();
            }
            
            ~MacRenderingContext()
            {
                Canvas->flush();
                Target->Present();
            }
            
            
        };
        
        RenderingContext*CreateRenderingContext()
        {
            Resize();
            return new MacRenderingContext(this);
        };
    };
    
    
    extern RenderTarget* CreateRenderTarget(void* nativeHandle)
    {
        return new MacRenderTarget(nativeHandle);
    }
    
    extern void InitSw()
    {
        
    }
    extern GrContext*CreatePlatformGrContext()
    {
        return nullptr;
    }
}