#include "common.h"
#include "mac/SkCGUtils.h"
#import "UIKit/UIKit.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define SKGL_CONFIG kEAGLColorFormatRGBA8



namespace libperspesk
{
    static EAGLContext* GlContext;
    static GLuint Framebuffer, RenderBuffer, StencilBuffer;
    
    extern GrContext*CreatePlatformGrContext()
    {
        GlContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        [EAGLContext setCurrentContext:GlContext];
        glGenFramebuffers(1, &Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

        glGenRenderbuffers(1, &RenderBuffer);
        glGenRenderbuffers(1, &StencilBuffer);

        glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RenderBuffer);

        glBindRenderbuffer(GL_RENDERBUFFER, StencilBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, StencilBuffer);

        return GrContext::Create(kOpenGL_GrBackend, (GrBackendContext)GrGLCreateNativeInterface());
    }
    
    class MacRenderTarget : public RenderTarget
    {
    public:
        CAEAGLLayer*Layer;
        SkBitmap Bitmap;
        SkAutoTUnref<SkSurface> Surface;
        int Width, Height;
        MacRenderTarget(void*nativeWindow)
        {
            Layer = (CAEAGLLayer*)CFBridgingRelease(nativeWindow);
            Layer.opaque = true;
            Layer.anchorPoint = CGPointMake(0,0);
            Width=Height=0;
            Layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                   [NSNumber numberWithBool:NO],
                                   kEAGLDrawablePropertyRetainedBacking,
                                   SKGL_CONFIG,
                                   kEAGLDrawablePropertyColorFormat,nil];
        }
        
        void ResetContext()
        {
            [EAGLContext setCurrentContext:GlContext];
            glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer);
        }
        
        void Resize()
        {
            float scale = [UIScreen mainScreen].scale;
            Layer.contentsScale = scale;
            int w = Layer.bounds.size.width *scale;
            int h = Layer.bounds.size.height *scale;
            
            if(w<1)
                w=1;
            if(h<1)
                h=1;
            if(Width != w || Height != h)
            {
                Width = w;
                Height =h;
            }
            
            GrBackendRenderTargetDesc desc;
            ResetContext();
            
            glBindRenderbuffer(GL_RENDERBUFFER, StencilBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, Width, Height);
            
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_STENCIL_SIZE,
                                 &desc.fStencilBits);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_SAMPLES_APPLE,
                                 &desc.fSampleCnt);
            
            glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer);
            [GlContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:Layer];
            
            desc.fWidth = SkScalarRoundToInt(w);
            desc.fHeight = SkScalarRoundToInt(h);
            desc.fConfig = kSkia8888_GrPixelConfig;
            desc.fRenderTargetHandle = Framebuffer;
            desc.fOrigin = GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;

            
            SkAutoTUnref<GrRenderTarget> target(Context->textureProvider()->wrapBackendRenderTarget(desc));
            Surface.reset(SkSurface::NewRenderTargetDirect(target));
            
            /*
            Bitmap.allocN32Pixels(Width, Height);
            Surface.reset(SkSurface::NewRasterDirect(Bitmap.info(), Bitmap.getPixels(), Bitmap.rowBytes()));*/
        }
        
        void Present()
        {
            ResetContext();
            [GlContext presentRenderbuffer:GL_RENDERBUFFER];

            /*
            CGImageRef img = SkCreateCGImageRef(Bitmap);
            Layer.contents = (__bridge id)img;
            CGImageRelease(img);*/
        }
        
        
        class MacRenderingContext : public RenderingContext
        {
        public:
            MacRenderTarget*Target;
            MacRenderingContext(MacRenderTarget* target)
            {
                Target = target;
                Canvas = target->Surface->getCanvas();
                Canvas->restoreToCount(1);
                Canvas->save();
                Canvas->resetMatrix();
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
            ResetContext();
            glDisable(GL_SCISSOR_TEST);
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT);
            glClearStencil(0);
            glClearColor(0, 255, 0, 0);
            glViewport(0, 0, Width, Height);
            
            

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

}
