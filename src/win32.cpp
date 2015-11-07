#ifdef WIN32
#include "pgl.h"
#include "GrGpuResource.h"


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
		GlWindowContext Gl;
		int Width, Height;

		WindowRenderTarget(HWND hWnd, int width, int height)
			: SurfaceProps(SkSurfaceProps::kLegacyFontHost_InitType), Gl(hWnd, width, height)

		{
			this->hWnd = hWnd;
			Width = 0, Height = 0;
			Gl = GlWindowContext(hWnd, width, height);
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
				Gl.detach();
				IsGpu = false;
			}
			Bitmap = SkBitmap();

			//Initialize
			Gl.fWidth = width;
			Gl.fHeight = height;
			IsGpu = Gl.attach(0);
			if (IsGpu)
				Surface.reset(Gl.CreateSurface());
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
				Gl.present();
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
				Gl.detach();
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
				Gl.MakeCurrent();
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