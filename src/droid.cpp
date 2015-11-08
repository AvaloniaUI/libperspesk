#include "common.h"
namespace libperspesk
{
#ifdef __ANDROID__
	extern  JNIEnv *Jni = 0;

	static  jobject jBitmapConfig;

	void SwViewport::DrawToWindow()
	{
		SkDebugf("Releasing native window");
		Surface.reset();
		ANativeWindow_unlockAndPost(NativeWindow);
		ANativeWindow_release(NativeWindow);
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
	void SwViewport::PrepareToDraw() {

		NativeWindow = ANativeWindow_fromSurface(Jni, (jobject)Window);
		ANativeWindow_acquire(NativeWindow);

		SkDebugf("Got native window %p", NativeWindow);
		int width = ANativeWindow_getWidth(NativeWindow);
		int height = ANativeWindow_getHeight(NativeWindow);
		int format = ANativeWindow_getFormat(NativeWindow);
		SkDebugf("Size: %ix%i, Format: %i", width, height, format);
		ANativeWindow_Buffer buffer;
		ARect rc;
		rc.left = 0;
		rc.top =0;
		rc.right = width;
		rc.bottom = height;
		buffer.bits = nullptr;
		ANativeWindow_lock(NativeWindow, &buffer, &rc);
		SkDebugf("Locked data: %p", buffer.bits);
		SkColorType color = SKIA_COLOR_FORMAT;
		int stride  = buffer.stride;

		if(format == WINDOW_FORMAT_RGB_565)
		{
			color = kRGB_565_SkColorType;
			stride*=2;
		}
		else
			stride *= 4;
		SkDebugf("Using color format: %i, stride: %i", color, stride);



		Surface.reset(SkSurface::NewRasterDirect(SkImageInfo::Make(width, height, color, kOpaque_SkAlphaType),
		    buffer.bits, stride));
		SkDebugf("Surface: %p", Surface.get());
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