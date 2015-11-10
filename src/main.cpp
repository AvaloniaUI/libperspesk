#define DEFINE_METHOD_TABLE
#include "common.h"

namespace libperspesk
{
	extern SkGLContext* GlContext = NULL;
	extern GrContext* Context = NULL;
	extern GrBackendContext GrBackend = NULL;
	static bool Initialized = false;;
	static void* OptionsA[PerspexRenderOption::proMaxOption + 1];
	extern void** Options = OptionsA;
	static void Init()
	{
		for (int c = 0; c <= PerspexRenderOption::proMaxOption; c++)
			Options[c] = nullptr;
		if (Initialized)
			return;
		Initialized = true;
		SkGraphics::Init();
		Context = CreatePlatformGrContext();
		InitSw();
	}

	extern void SetOption(PerspexRenderOption option, void* value)
	{
		if (option > proMaxOption)
			return;
		Options[option] = value;
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