#include "common.h"
namespace libperspesk {
#ifdef USE_X11
	static Display* XDisp;

	static bool convertBitmapToXImage(XImage& image, const SkBitmap& bitmap) {
		sk_bzero(&image, sizeof(image));

		int bitsPerPixel = bitmap.bytesPerPixel() * 8;
		image.width = bitmap.width();
		image.height = bitmap.height();
		image.format = ZPixmap;
		image.data = (char *) bitmap.getPixels();
		image.byte_order = LSBFirst;
		image.bitmap_unit = bitsPerPixel;
		image.bitmap_bit_order = LSBFirst;
		image.bitmap_pad = bitsPerPixel;
		image.depth = 24;
		image.bytes_per_line = bitmap.rowBytes() - bitmap.width() * 4;
		image.bits_per_pixel = bitsPerPixel;
		return XInitImage(&image);
	}

	void SwViewport::DrawToWindow()
	{
		XImage image;
		if (!convertBitmapToXImage(image, Bitmap)) {
			return;
		}

		XPutImage(XDisp,
		          (Drawable)Window,
		          Gc,
		          &image,
		          0, 0,     // src x,y
		          0, 0,     // dst x,y
		          Bitmap.width(), Bitmap.height());

	}

	extern void InitSw()
	{
		XDisp = XOpenDisplay(nullptr);
		if(XDisp == nullptr)
			SkDebugf("Failed to open X11 display!");
	}

	void SwViewport::Setup()
	{
		Gc = XCreateGC(XDisp, (Drawable)Window, 0, nullptr);
	}

	void SwViewport::Rezise(int width, int height)
	{

		Surface.reset(nullptr);
		Bitmap.allocPixels(SkImageInfo::Make(width, height, SKIA_COLOR_FORMAT, kPremul_SkAlphaType));
		Surface.reset(SkSurface::NewRasterDirect(Bitmap.info(), Bitmap.getPixels(), Bitmap.rowBytes()));
	}

	SwViewport::~SwViewport()
	{
	}

	SwViewport::SwViewport()
	{
		Gc = nullptr;
	}


#endif
}