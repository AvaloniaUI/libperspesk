#include "gl/SkGLContext.h"
#define SK_ANGLE 1
#define private public
#include "gl/angle/SkANGLEGLContext.h"
#undef private

#include "angle.h"


//This file is mostly copy/pasted from SkOSWindow_win.cpp
typedef GrGLFuncPtr(*GrGLGetProc)(void* ctx, const char name[]);
const GrGLInterface* GrGLAssembleGLESInterface(void* ctx, GrGLGetProc get);

namespace libperspesk
{
	GLIMP(PFNGLCLEARCOLORPROC, glClearColor);
	GLIMP(PFNGLCLEARSTENCILPROC, glClearStencil);
	GLIMP(PFNGLFLUSHPROC, glFlush);
	GLIMP(PFNGLVIEWPORTPROC, glViewport);


	static bool create_ANGLE(EGLNativeWindowType hWnd,
		int msaaSampleCount,
		EGLDisplay* eglDisplay,
		EGLContext* eglContext,
		EGLSurface* eglSurface,
		EGLConfig* eglConfig) {
		static const EGLint contextAttribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE, EGL_NONE
		};
		static const EGLint configAttribList[] = {
			EGL_RED_SIZE,       8,
			EGL_GREEN_SIZE,     8,
			EGL_BLUE_SIZE,      8,
			EGL_ALPHA_SIZE,     8,
			EGL_DEPTH_SIZE,     8,
			EGL_STENCIL_SIZE,   8,
			EGL_NONE
		};
		static const EGLint surfaceAttribList[] = {
			EGL_NONE, EGL_NONE
		};
		
		static const EGLint configAttribs[] = {
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_NONE
		};

		SkANGLEGLContext* angle = static_cast<SkANGLEGLContext*>(GlContext);
		EGLDisplay display = static_cast<EGLDisplay>(angle->fDisplay);
		EGLint numConfigs;
		eglChooseConfig(display, configAttribs, eglConfig, 1, &numConfigs);
	
		// Create a surface
		EGLSurface surface = eglCreateWindowSurface(display, *eglConfig,
			(EGLNativeWindowType)hWnd,
			surfaceAttribList);
		if (surface == EGL_NO_SURFACE) {

			int err = eglGetError();
			return false;
		}

		EGLContext context = angle->fContext;
		// Make the context current
		if (!eglMakeCurrent(display, surface, surface, context)) {
			return false;
		}

		*eglDisplay = display;
		*eglContext = context;
		*eglSurface = surface;
		return true;
	}
	

	bool AngleContext::attachANGLE(int msaaSampleCount) {
		if (EGL_NO_DISPLAY == fDisplay) {
			bool bResult = create_ANGLE((HWND)fHWND,
				msaaSampleCount,
				&fDisplay,
				&fContext,
				&fSurface,
				&fConfig);
			if (false == bResult) {
				return false;
			}
			
				glClearStencil(0);
				glClearColor(0, 0, 0, 0);
				glViewport(0, 0, fWidth, fHeight);
			
			return true;
		}
		return false;
	}

	void AngleContext::detachANGLE() {
		eglMakeCurrent(fDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, fContext);

		//eglDestroyContext(fDisplay, fContext);
		//fContext = EGL_NO_CONTEXT;

		eglDestroySurface(fDisplay, fSurface);
		fSurface = EGL_NO_SURFACE;
		/*
		eglTerminate(fDisplay);
		fDisplay = EGL_NO_DISPLAY;*/
	}

	void AngleContext::presentANGLE() {
		MakeCurrent();
		glFlush();
		eglSwapBuffers(fDisplay, fSurface);
	}

	void AngleContext::MakeCurrent()
	{
		eglMakeCurrent(fDisplay, fSurface, fSurface, fContext);
	}

	AngleContext::AngleContext(HWND wnd, int width, int height)
	{
		fDisplay = EGL_NO_DISPLAY;
		fContext = EGL_NO_CONTEXT;
		fSurface = EGL_NO_SURFACE;
		fConfig = EGL_NO_CONTEXT;
		fHWND = wnd;
		fWidth = width;
		fHeight = height;
	}
}
