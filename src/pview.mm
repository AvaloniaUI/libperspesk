#include "common.h"
#include "mac/SkCGUtils.h"
#import "UIKit/UIKit.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#import "GLKit/GLKView.h"
#define SKGL_CONFIG kEAGLColorFormatRGBA8



namespace libperspesk
{
    static EAGLContext* GlContext;
    static GLuint Framebuffer, RenderBuffer, StencilBuffer;

    int SkForceLinking(bool doNotPassTrue) {
        if (doNotPassTrue) {
            SkASSERT(false);
            CreateJPEGImageDecoder();
            CreateBMPImageDecoder();
            CreatePNGImageEncoder_IOS();
            return -1;
        }
        return 0;
    }
    extern "C" void* GetPerspexEAGLContext(){
        return (void*)CFBridgingRetain(GlContext);
    }
    
    
    extern GrContext*CreatePlatformGrContext()
    {
        SkForceLinking(true);
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
        GLKView* View;
        SkAutoTUnref<SkSurface> Surface;
        MacRenderTarget(void*nativeWindow)
        {
        }
        
        void Prepare()
        {
            GLint w, h;
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h);
            
            GrBackendRenderTargetDesc desc;

            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_STENCIL_SIZE,
                                 &desc.fStencilBits);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_SAMPLES_APPLE,
                                 &desc.fSampleCnt);
            
            GLint fb;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING_APPLE, &fb);
            desc.fRenderTargetHandle = fb;
            desc.fWidth = SkScalarRoundToInt(w);
            desc.fHeight = SkScalarRoundToInt(h);
            desc.fConfig = kSkia8888_GrPixelConfig;
            desc.fOrigin = GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;

            
            SkAutoTUnref<GrRenderTarget> target(Context->textureProvider()->wrapBackendRenderTarget(desc));
            Surface.reset(SkSurface::NewRenderTargetDirect(target));
            
            glDisable(GL_SCISSOR_TEST);
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT);
            glClearStencil(0);
            glClearColor(0, 255, 0, 0);
            glViewport(0, 0, w, h);
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
            }
        };
        
        RenderingContext*CreateRenderingContext()
        {
            Prepare();
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
