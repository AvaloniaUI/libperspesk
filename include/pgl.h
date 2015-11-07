#include "common.h"
#include "gl/GrGLInterface.h"
#include "EGL/egl.h"
#include "EGL/EGLPlatform.h"
#include "GLES2/gl2.h"
#include "gl/SkGLContext.h"

#define GLIMP(proto, name) proto name = (proto)eglGetProcAddress(#name)

namespace libperspesk
{
	class GlWindowContext
	{
	public:
		HWND fHWND;
		EGLSurface fSurface;
		int fWidth;
		int fHeight;
		int fSampleCount;
		int fStencilBits;
		bool attach(int msaaSampleCount);
		void detach();
		void present();
		GlWindowContext(HWND wnd, int width, int height);
		void MakeCurrent();
		SkSurface* CreateSurface();
	};


}