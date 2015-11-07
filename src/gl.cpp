#include "pgl.h"
#include "gl/GrGLInterface.h"

namespace libperspesk {

	static EGLConfig EglConfig;
	static EGLSurface EglSurface;
	static EGLContext EglContext;
	static EGLDisplay EglDisplay;

	static const EGLint configAttribs[] = {
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_NONE
	};

	static const EGLint surfaceAttribList[] = {
		EGL_NONE, EGL_NONE
	};

	extern GrContext* CreatePlatformGrContext()
	{
		EGLConfig config;
		EGLSurface surface;
		EGLContext context;
		
		EGLint numConfigs;



#ifdef WIN32
		EglDisplay = SkANGLEGLContext::GetD3DEGLDisplay(EGL_DEFAULT_DISPLAY, false);
#else
		eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif

		if (EGL_NO_DISPLAY == EglDisplay) {
			SkDebugf("Could not create EGL display!");
			return nullptr;
		}

		EGLint majorVersion;
		EGLint minorVersion;
		eglInitialize(EglDisplay, &majorVersion, &minorVersion);

		EGLConfig surfaceConfig;
		eglChooseConfig(EglDisplay, configAttribs, &surfaceConfig, 1, &numConfigs);

		static const EGLint contextAttribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
		};
		EglContext = eglCreateContext(EglDisplay, surfaceConfig, nullptr, contextAttribs);


		static const EGLint surfaceAttribs[] = {
			EGL_WIDTH, 1,
			EGL_HEIGHT, 1,
			EGL_NONE
		};

		EglSurface = eglCreatePbufferSurface(EglDisplay, surfaceConfig, surfaceAttribs);



		eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);
		

		const GrGLInterface* iface =
#ifdef WIN32
			GrGLCreateANGLEInterface();
#else
			GrGLCreateNativeInterface();
#endif
		if (!iface->validate())
			return nullptr;
		return GrContext::Create(kOpenGL_GrBackend, (GrBackendContext)iface);
	}



	GLIMP(PFNGLCLEARCOLORPROC, glClearColor);
	GLIMP(PFNGLCLEARSTENCILPROC, glClearStencil);
	GLIMP(PFNGLFLUSHPROC, glFlush);
	GLIMP(PFNGLVIEWPORTPROC, glViewport);
	GLIMP(PFNGLGETINTEGERVPROC, glGetIntegerv);

	extern SkGLContext* CreatePlatformGlContext()
	{
		return nullptr;
	}

	bool GlWindowContext::attach(int msaaSampleCount) {
		if (EGL_NO_SURFACE == fSurface) {

			EGLint numConfigs;
			eglChooseConfig(EglDisplay, configAttribs, &EglConfig, 1, &numConfigs);

			// Create a surface
			fSurface = eglCreateWindowSurface(EglDisplay, EglConfig,
				(EGLNativeWindowType)fWindow,
				surfaceAttribList);
			if (fSurface == EGL_NO_SURFACE) {

				int err = eglGetError();
				return false;
			}
			eglGetConfigAttrib(EglDisplay, EglConfig, EGL_STENCIL_SIZE, &fStencilBits);
			eglGetConfigAttrib(EglDisplay, EglConfig, EGL_SAMPLES, &fSampleCount);
			glClearStencil(0);
			glClearColor(0, 0, 0, 0);
			glViewport(0, 0, fWidth, fHeight);

			return true;
		}
		return false;
	}

	void GlWindowContext::detach() {
		eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);
		eglDestroySurface(EglDisplay, fSurface);
		fSurface = EGL_NO_SURFACE;
	}

	void GlWindowContext::present() {
		MakeCurrent();
		glFlush();
		eglSwapBuffers(EglDisplay, fSurface);
	}

	void GlWindowContext::MakeCurrent()
	{
		eglMakeCurrent(EglDisplay, fSurface, fSurface, EglContext);
	}

	SkSurface* GlWindowContext::CreateSurface()
	{
		GrBackendRenderTargetDesc desc;
		desc.fWidth = fWidth;
		desc.fHeight = fHeight;
		desc.fConfig = kSkia8888_GrPixelConfig;
		desc.fOrigin = kBottomLeft_GrSurfaceOrigin;

		desc.fConfig = kRGBA_8888_GrPixelConfig;
		desc.fStencilBits = fStencilBits;
		desc.fSampleCnt =  fSampleCount;
		desc.fOrigin = GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;

		GrGLint buffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);

		desc.fRenderTargetHandle = buffer;

		SkAutoTUnref<GrRenderTarget> target(Context->textureProvider()->wrapBackendRenderTarget(desc));
		return SkSurface::NewRenderTargetDirect(target);
	}

	GlWindowContext::GlWindowContext(void* wnd, int width, int height)
	{
		fSampleCount = 0;
		fStencilBits = 0;
		fSurface = EGL_NO_SURFACE;
		fWindow = wnd;
		fWidth = width;
		fHeight = height;
	}
}
