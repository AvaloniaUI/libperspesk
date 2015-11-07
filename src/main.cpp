#define DEFINE_METHOD_TABLE
#include "common.h"

namespace libperspesk
{
	extern SkGLContext* GlContext = NULL;
	extern GrContext* Context = NULL;
	extern GrBackendContext GrBackend = NULL;
	static bool Initialized = false;;
	static void Init()
	{
		if (Initialized)
			return;
		Initialized = true;
		SkGraphics::Init();
		Context = CreatePlatformGrContext();
	}
}

extern "C"
#ifdef WIN32
__declspec(dllexport)
#endif
void* GetPerspexMethodTable()
{
	Init();
	return &MethodTable;
}