#include "pgl.h"
#include "GrGpuResource.h"

namespace libperspesk
{
	class WindowRenderTarget : public RenderTarget
	{
	public:
		void* hWnd;
		SwViewport Sw;
		SkSurfaceProps SurfaceProps;
		bool IsGpu;
		GlWindowContext Gl;
		int Width, Height;

		WindowRenderTarget(void* hWnd, int width, int height)
			: SurfaceProps(SkSurfaceProps::kLegacyFontHost_InitType), Gl(hWnd, width, height)

		{
			this->hWnd = hWnd;
			Width = 0, Height = 0;
			Gl = GlWindowContext(hWnd, width, height);
			IsGpu = false;
			Resize(width, height);
		}
		
		SkSurface* getSurface()
		{
			if (IsGpu)
				return Gl.Surface.get();
			else
				return Sw.Surface.get();
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

			//Initialize
			Gl.fWidth = width;
			Gl.fHeight = height;
			IsGpu = Gl.attach(0);
			if (!IsGpu)
			{
				Sw.Rezise(width, height);
			}
		}


		void Present()
		{
			if (IsGpu)
				Gl.present();
			else
				Sw.DrawToWindow(hWnd);
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
				Canvas = target->getSurface()->getCanvas();
			}

			~WinContext()
			{
				Target->getSurface()->getCanvas()->flush();
				Target->Present();
			}
		};

		virtual RenderingContext* CreateRenderingContext() override
		{
			if (IsGpu)
				Gl.MakeCurrent();
			SkSurface*s = getSurface();
			s->getCanvas()->restoreToCount(1);
			s->getCanvas()->save();
			s->getCanvas()->clear(SkColorSetARGB(0, 0, 0, 0));
			s->getCanvas()->resetMatrix();
			return new WinContext(this);
		}
	};

	extern RenderTarget* CreateRenderTarget(void* nativeHandle, int width, int height)
	{
		return new WindowRenderTarget(nativeHandle, width, height);
	}
}