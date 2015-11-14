#ifndef __APPLE__
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


		
		SkSurface* getSurface()
		{
			if (IsGpu)
				return Gl.Surface.get();
			else
				return Sw.Surface.get();
		}

		void FixSize()
		{
			int width, height;
			GetPlatformWindowDimensions(hWnd, &width, &height);
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
			if (Options[proForceSoftware] == nullptr)
			{
				Gl.fWidth = width;
				Gl.fHeight = height;
				IsGpu = Gl.attach(0);
			}
			if (!IsGpu)
			{
				Sw.Rezise(width, height);
			}
		}

		WindowRenderTarget(void* hWnd)
			: SurfaceProps(SkSurfaceProps::kLegacyFontHost_InitType), Gl(hWnd, 1, 1)

		{
			this->hWnd = hWnd;
			Width = 0, Height = 0;
			Sw.Window = hWnd;
			Sw.Setup();
			IsGpu = false;

			FixSize();
		}

		void Present()
		{
			if (IsGpu)
				Gl.present();
			else
				Sw.DrawToWindow();
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
			FixSize();
			if (IsGpu)
				Gl.MakeCurrent();
			else
				Sw.PrepareToDraw();
			SkSurface*s = getSurface();
			if(s==nullptr)
				SkDebugf("No surface to draw, crashing...\n");
			s->getCanvas()->restoreToCount(1);
			s->getCanvas()->save();
			s->getCanvas()->clear(SkColorSetARGB(0, 0, 0, 0));
			s->getCanvas()->resetMatrix();
			return new WinContext(this);
		}
	};

	extern RenderTarget* CreateRenderTarget(void* nativeHandle)
	{
		return new WindowRenderTarget(nativeHandle);
	}
}

#endif