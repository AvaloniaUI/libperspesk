#define SK_RELEASE

#ifdef WIN32
#define SK_ANGLE 1
#include "gl/angle/SkANGLEGLContext.h"
#endif
#ifdef __ANDROID__
#include <jni.h>
#include <android/native_window_jni.h>
#endif

#include <SkGraphics.h>
#include "SkData.h"
#include "SkRegion.h"
#include "SkImage.h"
#include "SkCanvas.h"
#include "SkStream.h"
#include "SkSurface.h"
#include "GrContext.h"
#include "SkImageDecoder.h"
#include "gl/SkGLContext.h"
#include "shared.h"
#include "GrTypes.h"
#ifdef LoadImage
#undef LoadImage
#endif

#if USE_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#endif

#ifdef CPU_MIPS
#define SKIA_COLOR_FORMAT kARGB_8888_SkColorType
#else
#define SKIA_COLOR_FORMAT kBGRA_8888_SkColorType
#endif

#define pchar unsigned short

using namespace libperspesk;
namespace libperspesk
{
	extern GrContext* Context;

	class FormattedText;
	class BitmapContainer;
	class RenderingContext
	{
	public:
		SkCanvas* Canvas;
		PerspexDrawingContextSettings Settings;
		virtual ~RenderingContext() {}
	};

	class RenderTarget
	{
	public:
		virtual RenderingContext* CreateRenderingContext() = 0;
		virtual void Resize(int width, int height) = 0;
		virtual ~RenderTarget() {}
	};


	class SwViewport
	{
	public:
		void*Window;
		void  Setup();
		SkAutoTUnref<SkSurface> Surface;
		void Rezise(int width, int height);
		void PrepareToDraw();
		void DrawToWindow();
		~SwViewport();
		SwViewport();
	private:
		SkBitmap Bitmap;
#ifdef USE_X11
		GC Gc;
#elif __ANDROID__
		ANativeWindow* NativeWindow;
#endif

	};

#ifdef __ANDROID__
	extern JNIEnv *Jni;
#endif

	extern void ConfigurePaint(SkPaint& paint, RenderingContext*ctx, PerspexBrush*brush);
	extern GrContext* CreatePlatformGrContext();
	extern void InitSw();
	//Method table

	extern RenderTarget* CreateRenderTarget(void* nativeHandle, int width, int height);
	extern void DrawRectangle(RenderingContext*ctx, PerspexBrush*brush, SkRect*rect, float borderRadius);
	extern void PushClip(RenderingContext*ctx, SkRect*rect);
	extern void PopClip(RenderingContext*ctx);
	extern void SetTransform(RenderingContext*ctx, float*m);
	extern void DrawLine(RenderingContext*ctx, PerspexBrush*brush, float x1, float y1, float x2, float y2);
	extern SkPath* CreatePath(PerspexGeometryElement* elements, int count, SkRect* bounds);
	extern void DisposePath(SkPath*path);
	extern void DrawGeometry(RenderingContext*ctx, SkPath*path, PerspexBrush*fill, PerspexBrush* stroke);
	extern bool LoadImage(void*pData, int len, BitmapContainer**pImage, int* width, int* height);
	extern SkData* SaveImage(BitmapContainer*pImage, PerspexImageType ptype, int quality);
	extern void DrawImage(RenderingContext*ctx, BitmapContainer*image, float opacity, SkRect* srcRect, SkRect*destRect);
	extern BitmapContainer* CreateRenderTargetBitmap(int width, int height);
	extern void ResizeBitmap(BitmapContainer *bmp, int width, int height);
	extern void DisposeImage(BitmapContainer* bmp);
	extern SkTypeface* CreateTypeface(char* name, int style);
	extern FormattedText* CreateFormattedText(pchar* text, int len, SkTypeface* typeface, float fontSize, SkPaint::Align align, PerspexFormattedText**exp);
	extern void RebuildFormattedText(FormattedText*txt);
	extern void DestroyFormattedText(FormattedText* txt);
	extern void DrawFormattedText(RenderingContext* ctx, PerspexBrush* brush, FormattedText* text, float x, float y);


#ifdef DEFINE_METHOD_TABLE

	RenderingContext* RenderTargetCreateRenderingContext(RenderTarget* target)
	{
		return target->CreateRenderingContext();
	};

	void RenderTargetResize(RenderTarget* target, int width, int height)
	{
		target->Resize(width, height);
	}

	void DisposeRenderTarget(RenderTarget* target)
	{
		delete target;
	}

	void DisposeRenderingContext(RenderingContext*ctx)
	{
		delete ctx;
	}

	void DestroySkData(SkData*buffer)
	{
		buffer->unref();
	}

	int GetSkDataSize(SkData*buffer)
	{
		return buffer->size();
	}

	void ReadSkData(SkData*buffer, void* ptr, int count)
	{
		memcpy(ptr, buffer->bytes(), count);
	}

	PerspexDrawingContextSettings* GetDrawingContextSettingsPtr(RenderingContext*ctx)
	{
		return &ctx->Settings;
	}

	static void* MethodTable[] = {
		&CreateRenderTarget,
		&DisposeRenderTarget,
		&RenderTargetResize,
		&RenderTargetCreateRenderingContext,
		&DisposeRenderingContext,
		&DrawRectangle,
		&PushClip,
		&PopClip,
		&SetTransform,
		&DrawLine,
		&CreatePath,
		&DisposePath,
		&libperspesk::DrawGeometry,
		&GetSkDataSize,
		&ReadSkData,
		&DestroySkData,
		&LoadImage,
		&SaveImage,
		&DrawImage,
		&CreateRenderTargetBitmap,
		&ResizeBitmap,
		&DisposeImage,
		&GetDrawingContextSettingsPtr,
		&CreateTypeface,
		&CreateFormattedText,
		&RebuildFormattedText,
		&DestroyFormattedText,
		&DrawFormattedText,
		0
	};

#endif
}
