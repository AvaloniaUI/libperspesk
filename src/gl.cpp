#include "pgl.h"
#include "gl/GrGLInterface.h"

namespace libperspesk {

	static EGLConfig EglConfig;
	static EGLSurface EglSurface;
	static EGLContext EglContext;
	static EGLDisplay EglDisplay;

	static GrGLFuncPtr GlGetProc(void* ctx, const char name[])
	{
		return eglGetProcAddress(name);
	}

	static const EGLint surfaceAttribList[] = {
			EGL_NONE, EGL_NONE
	};

	#ifdef WIN32
	static const EGLint configAttribs[] = {
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_NONE
	};




	extern GrContext* CreatePlatformGrContext()
	{
		EGLConfig config;
		EGLSurface surface;
		EGLContext context;
		
		EGLint numConfigs;



		EglDisplay = SkANGLEGLContext::GetD3DEGLDisplay(EGL_DEFAULT_DISPLAY, false);

		if (EGL_NO_DISPLAY == EglDisplay) {
			SkDebugf("Could not create EGL display! Error %04x\n", eglGetError());
			return nullptr;
		}

		EGLint majorVersion;
		EGLint minorVersion;
		eglInitialize(EglDisplay, &majorVersion, &minorVersion);

		eglChooseConfig(EglDisplay, configAttribs, &EglConfig, 1, &numConfigs);

		static const EGLint contextAttribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
		};
		EglContext = eglCreateContext(EglDisplay, EglConfig, nullptr, contextAttribs);


		static const EGLint surfaceAttribs[] = {
			EGL_WIDTH, 1,
			EGL_HEIGHT, 1,
			EGL_NONE
		};

		EglSurface = eglCreatePbufferSurface(EglDisplay, EglConfig, surfaceAttribs);



		eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);
		

		const GrGLInterface* iface = GrGLCreateANGLEInterface();

		printf ("GLInterface: %p\n", iface);
		if (!iface->validate())
			return nullptr;
		return GrContext::Create(kOpenGL_GrBackend, (GrBackendContext)iface);
	}
#else

	static EGLConfig surfaceConfig;
extern GrContext* CreatePlatformGrContext() {
	static const EGLint kEGLContextAttribsForOpenGL[] = {
			EGL_NONE
	};

	static const EGLint kEGLContextAttribsForOpenGLES[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
	};

	static const struct {
		const EGLint *fContextAttribs;
		EGLenum fAPI;
		EGLint fRenderableTypeBit;
		GrGLStandard fStandard;
	} kAPIs[] = {
			{   // OpenGL
					kEGLContextAttribsForOpenGL,
					EGL_OPENGL_API,
					EGL_OPENGL_BIT,
					kGL_GrGLStandard
			},
			{   // OpenGL ES. This seems to work for both ES2 and 3 (when available).
					kEGLContextAttribsForOpenGLES,
					EGL_OPENGL_ES_API,
					EGL_OPENGL_ES2_BIT,
					kGLES_GrGLStandard
			},
	};

	size_t apiLimit = SK_ARRAY_COUNT(kAPIs);
	size_t api = 0;
	/*
	if (forcedGpuAPI == kGL_GrGLStandard) {
		apiLimit = 1;
	} else if (forcedGpuAPI == kGLES_GrGLStandard) {
		api = 1;
	}*/
	SkASSERT(forcedGpuAPI == kNone_GrGLStandard || kAPIs[api].fStandard == forcedGpuAPI);

	SkAutoTUnref<const GrGLInterface> gl;

	for (; nullptr == gl.get() && api < apiLimit; ++api) {
		EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		EGLint majorVersion;
		EGLint minorVersion;
		eglInitialize(EglDisplay, &majorVersion, &minorVersion);

		SkDebugf("Trying API #%i\n", api);
		SkDebugf("VENDOR: %s\n", eglQueryString(EglDisplay, EGL_VENDOR));
		SkDebugf("APIS: %s\n", eglQueryString(EglDisplay, EGL_CLIENT_APIS));
		SkDebugf("VERSION: %s\n", eglQueryString(EglDisplay, EGL_VERSION));
		SkDebugf("EXTENSIONS %s\n", eglQueryString(EglDisplay, EGL_EXTENSIONS));

		if (!eglBindAPI(kAPIs[api].fAPI)) {
			continue;
		}

		EGLint numConfigs = 0;
		const EGLint configAttribs[] = {
				EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
				EGL_RENDERABLE_TYPE, kAPIs[api].fRenderableTypeBit,
				EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE, 8,
				EGL_ALPHA_SIZE, 8,
				EGL_NONE
		};


		if (!eglChooseConfig(EglDisplay, configAttribs, &surfaceConfig, 1, &numConfigs)) {
			SkDebugf("eglChooseConfig failed. EGL Error: 0x%08x\n", eglGetError());
			continue;
		}
		else
			EglConfig  = surfaceConfig;

		if (0 == numConfigs) {
			SkDebugf("No suitable EGL config found.\n");
			continue;
		}

		EglContext = eglCreateContext(EglDisplay, surfaceConfig, nullptr, kAPIs[api].fContextAttribs);
		if (EGL_NO_CONTEXT == EglContext) {
			SkDebugf("eglCreateContext failed.  EGL Error: 0x%08x\n", eglGetError());
			continue;
		}

		static const EGLint kSurfaceAttribs[] = {
				EGL_WIDTH, 1,
				EGL_HEIGHT, 1,
				EGL_NONE
		};

		EglSurface = eglCreatePbufferSurface(EglDisplay, surfaceConfig, kSurfaceAttribs);
		if (EGL_NO_SURFACE == EglSurface) {
			SkDebugf("eglCreatePbufferSurface failed. EGL Error: 0x%08x\n", eglGetError());

			continue;
		}

		if (!eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext)) {
			SkDebugf("eglMakeCurrent failed.  EGL Error: 0x%08x\n", eglGetError());

			continue;
		}

		const GrGLInterface* iface = nullptr;
#ifdef __ANDROID__
		iface =GrGLCreateNativeInterface();
#else
		if(api == 0)
			iface =	GrGLAssembleGLInterface(nullptr, GlGetProc);
		if(api == 1)
			iface =	GrGLAssembleGLESInterface(nullptr, GlGetProc);
#endif
		SkDebugf("GLInterface: %p\n", iface);
		if (!iface->validate())
			return nullptr;

		return GrContext::Create(kOpenGL_GrBackend, (GrBackendContext)iface);
	}
	SkDebugf("Refusing to create GrContext, continuing without HW accelleration");
	return nullptr;
}
	
	
#endif
	
	
#ifndef WIN32
#define GL_APIENTRYP GL_APIENTRY*
	typedef void (GL_APIENTRYP PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	typedef void (GL_APIENTRYP PFNGLCLEARSTENCILPROC) (GLint s);
	typedef void (GL_APIENTRYP PFNGLFLUSHPROC) (void);
	typedef void (GL_APIENTRYP PFNGLVIEWPORTPROC) (GLint x, GLint y, GLsizei width, GLsizei height);
	typedef void (GL_APIENTRYP PFNGLGETINTEGERVPROC) (GLenum pname, GLint *data);
#endif
	GLIMP(PFNGLCLEARCOLORPROC, glClearColor);
	GLIMP(PFNGLCLEARSTENCILPROC, glClearStencil);
	GLIMP(PFNGLFLUSHPROC, glFlush);
	GLIMP(PFNGLVIEWPORTPROC, glViewport);
	GLIMP(PFNGLGETINTEGERVPROC, glGetIntegerv);


	extern SkGLContext* CreatePlatformGlContext()
	{
		return nullptr;
	}

	static void* ConvertWindow(void* window)
	{
#ifndef __ANDROID__
		return window;
#else
		return ANativeWindow_fromSurface(Jni, (jobject)window);
#endif

	}

	bool GlWindowContext::attach(int msaaSampleCount) {
		if(Context  == nullptr)
			return false;
		if (EGL_NO_SURFACE == fSurface) {
			// Create a surface
			fSurface = eglCreateWindowSurface(EglDisplay, EglConfig,
				(EGLNativeWindowType)ConvertWindow(fWindow),surfaceAttribList);
			if (fSurface == EGL_NO_SURFACE) {

				int err = eglGetError();
				printf("Failed to create surface %i\n", err);
				return false;
			}
			eglGetConfigAttrib(EglDisplay, EglConfig, EGL_STENCIL_SIZE, &fStencilBits);
			eglGetConfigAttrib(EglDisplay, EglConfig, EGL_SAMPLES, &fSampleCount);
			glClearStencil(0);
			glClearColor(0, 0, 0, 0);
			glViewport(0, 0, fWidth, fHeight);


			GrBackendRenderTargetDesc desc;
			desc.fWidth = fWidth;
			desc.fHeight = fHeight;
			desc.fConfig = kSkia8888_GrPixelConfig;
			desc.fOrigin = kBottomLeft_GrSurfaceOrigin;

			desc.fConfig = kRGBA_8888_GrPixelConfig;
			desc.fStencilBits = fStencilBits;
			desc.fSampleCnt = fSampleCount;
			desc.fOrigin = GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;

			GrGLint buffer;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);

			desc.fRenderTargetHandle = buffer;

			SkAutoTUnref<GrRenderTarget> target(Context->textureProvider()->wrapBackendRenderTarget(desc));
			Surface.reset(SkSurface::NewRenderTargetDirect(target));
			return true;
		}
		return false;
	}

	void GlWindowContext::detach() {
		Surface.reset();
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
