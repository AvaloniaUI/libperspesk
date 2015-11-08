#include "common.h"
#include "gl/GrGLInterface.h"
#include "EGL/egl.h"
#include "EGL/eglplatform.h"
#include "GLES2/gl2.h"

#include "gl/SkGLContext.h"

#define GLIMP(proto, name) proto name = (proto)eglGetProcAddress(#name)
typedef GrGLFuncPtr (*GrGLGetProc)(void* ctx, const char name[]);
extern const GrGLInterface* GrGLAssembleGLESInterface(void* ctx, GrGLGetProc get);
extern const GrGLInterface* GrGLAssembleGLInterface(void* ctx, GrGLGetProc get);
namespace libperspesk
{
	class GlWindowContext
	{
	public:
		void* fWindow;
		EGLSurface fSurface;
		int fWidth;
		int fHeight;
		int fSampleCount;
		int fStencilBits;
		bool attach(int msaaSampleCount);
		void detach();
		void present();
		GlWindowContext(void* wnd, int width, int height);
		void MakeCurrent();
		SkAutoTUnref<SkSurface> Surface;
	};


}