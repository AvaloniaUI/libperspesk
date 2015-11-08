#include "common.h"
namespace libperspesk
{
#ifdef __ANDROID__
	extern  JNIEnv *Jni = 0;

	void SwViewport::DrawToWindow()
	{
	}

	extern void InitSw()
	{
	}

	void SwViewport::Setup()
	{
	}

	void SwViewport::Rezise(int width, int height)
	{
	}

	SwViewport::~SwViewport()
	{
	}

	SwViewport::SwViewport()
	{
	}



#endif
}
#ifdef __ANDROID__
extern "C" void PerspexJniInit(JNIEnv *env)
{
	Jni = env;
}
#endif