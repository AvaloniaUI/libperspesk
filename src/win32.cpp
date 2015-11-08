#include "common.h"
namespace libperspesk
{
#ifdef WIN32
	void SwViewport::Rezise(int width, int height)
	{
		Surface.reset(nullptr);
		Bitmap.allocPixels(SkImageInfo::MakeN32(width, height, kPremul_SkAlphaType));
		Surface.reset(SkSurface::NewRasterDirect(Bitmap.info(), Bitmap.getPixels(), Bitmap.rowBytes()));
	}

	void SwViewport::DrawToWindow(void* window)
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

		

		HDC hdc = GetDC((HWND)window);
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

		ReleaseDC((HWND)window, hdc);
	}

	SwViewport::~SwViewport()
	{
	}

	SwViewport::SwViewport()
	{
	}




#endif
}