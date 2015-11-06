#include "common.h"
#include "gl/GrGLInterface.h"
#include "EGL/egl.h"
#include "EGL/EGLPlatform.h"
#include "GLES2/gl2.h"
#include "gl/SkGLContext.h"

#include "gl/angle/SkANGLEGLContext.h"

#define GLIMP(proto, name) proto name = (proto)eglGetProcAddress(#name)

namespace libperspesk
{
	class AngleContext
	{
	public:
		HWND fHWND;
		EGLDisplay          fDisplay;
		EGLContext          fContext;
		EGLSurface          fSurface;
		EGLConfig           fConfig;
		int fWidth;
		int fHeight;
		bool attachANGLE(int msaaSampleCount);
		void detachANGLE();
		void presentANGLE();
		AngleContext(HWND wnd, int width, int height);
		void MakeCurrent();
	};


}