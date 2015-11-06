#ifdef WIN32
#include "angle.h"
#include "GrGpuResource.h"

GLIMP(PFNGLGETINTEGERVPROC,glGetIntegerv);
namespace libperspesk
{
	

	class WindowRenderTarget : public RenderTarget
	{
	public:
		HWND hWnd;
		SkBitmap Bitmap;
		SkAutoTUnref<SkSurface> Surface;
		SkSurfaceProps SurfaceProps;
		bool IsGpu;
		AngleContext Angle;
		int Width, Height;

		WindowRenderTarget(HWND hWnd, int width, int height)
			: SurfaceProps(SkSurfaceProps::kLegacyFontHost_InitType), Angle(hWnd, width, height)

		{
			this->hWnd = hWnd;
			Width = 0, Height = 0;
			Angle = AngleContext(hWnd, width, height);
			IsGpu = false;
			Resize(width, height);
		}
		


		virtual void Resize(int width, int height) override
		{
			if (Width == width && Height == height)
				return;
			Width = width;
			Height = height;


			//Free resources
			if(IsGpu)
			{
				Angle.detachANGLE();
				IsGpu = false;
			}
			Bitmap = SkBitmap();

			//Initialize
			Angle.fWidth = width;
			Angle.fHeight = height;
			IsGpu = Angle.attachANGLE(0);
			if (IsGpu)
			{
				SkAutoTUnref<GrContext> ctx;
				ctx.reset(GrContext::Create(GrBackend::kOpenGL_GrBackend, GrBackend));

								GrBackendRenderTargetDesc desc;
				desc.fWidth = Width;
				desc.fHeight =Height;
				desc.fConfig = kSkia8888_GrPixelConfig;
				desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
				
				desc.fConfig = kRGBA_8888_GrPixelConfig;
				eglGetConfigAttrib(Angle.fDisplay, Angle.fConfig, EGL_STENCIL_SIZE, &desc.fStencilBits);
				eglGetConfigAttrib(Angle.fDisplay, Angle.fConfig, EGL_SAMPLES, &desc.fSampleCnt);
				desc.fOrigin = GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;
				
				GrGLint buffer;
				glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);

				desc.fRenderTargetHandle = buffer;

				SkAutoTUnref<GrRenderTarget> target(ctx->textureProvider()->wrapBackendRenderTarget(desc));
				Surface.reset(SkSurface::NewRenderTargetDirect(target));
				
			}
			else
			{
				Bitmap.allocPixels(SkImageInfo::MakeN32(width, height, kPremul_SkAlphaType));
				SkSurface* pSurf = SkSurface::NewRasterDirect(Bitmap.info(), Bitmap.getPixels(), Bitmap.rowBytes());
				Surface.reset(pSurf);
			}
		}


		void Present()
		{
			if (IsGpu)
			{
				Surface->getCanvas()->flush();
				Angle.presentANGLE();
			}
			else
			{
				BITMAPINFO bmi;
				memset(&bmi, 0, sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = Bitmap.width();
				bmi.bmiHeader.biHeight = -Bitmap.height(); // top-down image
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = 0;

				HDC hdc = GetDC(hWnd);
				Bitmap.lockPixels();
				int ret = SetDIBitsToDevice(hdc,
					0, 0,
					Bitmap.width(), Bitmap.height(),
					0, 0,
					0, Bitmap.height(),
					Bitmap.getPixels(),
					&bmi,
					DIB_RGB_COLORS);
				Bitmap.unlockPixels();
				ReleaseDC(hWnd, hdc);
			}
		}

		~WindowRenderTarget()
		{
			if (IsGpu)
				Angle.detachANGLE();
		}

		class WinContext : public RenderingContext
		{
			WindowRenderTarget*Target;
		public:
			WinContext(WindowRenderTarget* target)
			{
				Target = target;
				Canvas = target->Surface->getCanvas();
			}

			~WinContext()
			{
				Target->Surface->getCanvas()->flush();
				Target->Present();
			}
		};

		virtual RenderingContext* CreateRenderingContext() override
		{
			if (IsGpu)
				Angle.MakeCurrent();
			Surface->getCanvas()->restoreToCount(1);
			Surface->getCanvas()->save();
			Surface->getCanvas()->clear(SkColorSetARGB(0, 0, 0, 0));
			Surface->getCanvas()->resetMatrix();
			return new WinContext(this);
		}
	};

	extern RenderTarget* CreateRenderTarget(void* nativeHandle, int width, int height)
	{
		return new WindowRenderTarget((HWND)nativeHandle, width, height);
	}
}


#endif