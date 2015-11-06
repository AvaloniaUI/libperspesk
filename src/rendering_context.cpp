#include "common.h"
#include "SkGradientShader.h"
#include "SkDashPathEffect.h"
#include "SkPicture.h"

namespace libperspesk {
	class BitmapContainer : public RenderTarget
	{
	public:
		SkBitmap Bitmap;

		class ImageRenderingContext : public RenderingContext
		{
		public:
			BitmapContainer* Image;
			SkAutoTUnref<SkSurface> Surface;
			bool IsGpu;
			ImageRenderingContext(BitmapContainer*image)
			{
				Image = image;
				IsGpu = false;
				if (Context != nullptr)
					Surface.reset(SkSurface::NewRenderTarget(Context, SkSurface::kNo_Budgeted, Image->Bitmap.info()));
				if (Surface.get() != nullptr)
					IsGpu = true;
				else
				{
					Surface.reset(SkSurface::NewRasterDirect(image->Bitmap.info(), image->Bitmap.getPixels(), image->Bitmap.rowBytes()));
				}
				
				Surface.reset(SkSurface::NewRasterDirect(image->Bitmap.info(), image->Bitmap.getPixels(), image->Bitmap.rowBytes()));

				Canvas = Surface->getCanvas();
				Canvas->clear(SkColor());
			}

			~ImageRenderingContext()
			{
				if (IsGpu)
				{
					SkAutoTUnref<SkImage> image;
					image.reset(Surface->newImageSnapshot(SkSurface::kNo_Budgeted));
					image.get()->readPixels(Image->Bitmap.info(), Image->Bitmap.getPixels(), Image->Bitmap.rowBytes(), 0, 0);
				}
				Surface.reset(nullptr);
			}
		};

		virtual RenderingContext* CreateRenderingContext() override
		{
			return new ImageRenderingContext(this);
		}

		virtual void Resize(int width, int height) override
		{
			Bitmap.allocN32Pixels(width, height);
		}

	};

	inline SkShader::TileMode GetGradientTileMode(PerspexGradientSpreadMethod method)
	{
		if (method == grReflect)
			return SkShader::kMirror_TileMode;
		if (method == grRepeat)
			return SkShader::kRepeat_TileMode;
		return SkShader::kClamp_TileMode;
	}

	extern void ConfigurePaint(SkPaint& paint, RenderingContext*ctx, PerspexBrush*brush)
	{	
		if (brush->Stroke)
		{
			paint.setStyle(SkPaint::kStroke_Style);
			paint.setStrokeWidth(brush->StrokeThickness);
			paint.setStrokeMiter(brush->StrokeMiterLimit);

			SkPaint::Cap cap = SkPaint::Cap::kDefault_Cap;
			if (brush->StrokeLineCap == plcRound)
				cap = SkPaint::Cap::kRound_Cap;
			if (brush->StrokeLineCap == plcSquare)
				cap = SkPaint::Cap::kSquare_Cap;
			paint.setStrokeCap(cap);

			SkPaint::Join  join = SkPaint::Join::kBevel_Join;
			if(brush->StrokeLineJoin == pnjMiter)
				join = SkPaint::Join::kMiter_Join;
			if (brush->StrokeLineJoin == pnjRound)
				join = SkPaint::Join::kRound_Join;
			paint.setStrokeJoin(join);

			if(brush->StrokeDashCount != 0)
			{
				paint.setPathEffect(SkDashPathEffect::Create(brush->StrokeDashes, brush->StrokeDashCount, brush->StrokeDashOffset))->unref();
			}
		}
		else
			paint.setStyle(SkPaint::kFill_Style);

		

		if(brush->Type == brSolid)
		{
			paint.setColor(brush->Color);
		}
		else if(brush->Type == brRadialGradient)
		{
			paint.setAlpha(128);
			paint.setShader(SkGradientShader::CreateRadial(brush->GradientStartPoint, brush->GradientRadius,
				brush->GradientStopColors, brush->GradientStops,
				brush->GradientStopCount, GetGradientTileMode(brush->GradientSpreadMethod)))->unref();
		}
		else if(brush->Type == brLinearGradient)
		{
			paint.setShader(SkGradientShader::CreateLinear(&brush->GradientStartPoint, brush->GradientStopColors, brush->GradientStops,
				brush->GradientStopCount, GetGradientTileMode(brush->GradientSpreadMethod)))->unref();
		}
		else if(brush->Type == brImage)
		{
			SkMatrix matrix;
			matrix.setTranslate(brush->BitmapTranslation);
			SkShader::TileMode tileX = brush->BitmapTileMode == ptmNone ? SkShader::kClamp_TileMode
				: (brush->BitmapTileMode == ptmFlipX || brush->BitmapTileMode == ptmFlipXY) ? SkShader::kMirror_TileMode : SkShader::kRepeat_TileMode;
			SkShader::TileMode tileY = brush->BitmapTileMode == ptmNone ? SkShader::kClamp_TileMode
				: (brush->BitmapTileMode == ptmFlipY || brush->BitmapTileMode == ptmFlipXY) ? SkShader::kMirror_TileMode : SkShader::kRepeat_TileMode;
			
			paint.setShader(SkShader::CreateBitmapShader(brush->Bitmap->Bitmap, tileX, tileY, &matrix))->unref();
		}

		double opacity = brush->Opacity * ctx->Settings.Opacity;
		paint.setAlpha(paint.getAlpha()*opacity);
		paint.setAntiAlias(true);
	}

	inline SkPaint CreatePaint(RenderingContext*ctx, PerspexBrush*brush)
	{
		SkPaint paint;
		ConfigurePaint(paint, ctx, brush);
		return paint;
	}

	extern void DrawRectangle(RenderingContext*ctx, PerspexBrush*brush, SkRect*rect, float borderRadius)
	{
		SkPaint paint = CreatePaint(ctx, brush);
		if (borderRadius == 0)
			ctx->Canvas->drawRect(*rect, paint);
		else
			ctx->Canvas->drawRoundRect(*rect, borderRadius, borderRadius, paint);
	}

	extern void PushClip(RenderingContext*ctx, SkRect*rect)
	{
		ctx->Canvas->save();
		SkRegion rgn;
		rgn.setRect((int)rect->left(), (int)rect->top(), (int)rect->right(), (int)rect->bottom());
		ctx->Canvas->setClipRegion(rgn);

	}

	extern void PopClip(RenderingContext*ctx)
	{
		ctx->Canvas->restore();
	}


	extern void SetTransform(RenderingContext*ctx, float*m)
	{
		SkMatrix matrix;
		matrix.setAll(m[0], m[1], m[2], m[3], m[4], m[5], 0, 0, 1);

		ctx->Canvas->setMatrix(matrix);
	}

	extern void DrawLine(RenderingContext*ctx, PerspexBrush*brush, float x1, float y1, float x2, float y2)
	{
		ctx->Canvas->drawLine(x1, y1, x2, y2, CreatePaint(ctx, brush));
	}

	static void DrawGeometry(RenderingContext*ctx, SkPath*path, PerspexBrush*brush)
	{
		SkPaint pt = CreatePaint(ctx, brush);
		ctx->Canvas->drawPath(*path, pt);
	}

	extern void DrawGeometry(RenderingContext*ctx, SkPath*path, PerspexBrush*fill, PerspexBrush* stroke)
	{
		if (fill != nullptr)
			DrawGeometry(ctx, path, fill);
		if (stroke != nullptr)
			DrawGeometry(ctx, path, stroke);
	}

	extern bool LoadImage(void*pData, int len, BitmapContainer**pImage, int* width, int* height)
	{
		BitmapContainer*img = new BitmapContainer();
		if (!SkImageDecoder::DecodeMemory(pData, len, &img->Bitmap))
		{
			delete img;
			return false;
		}
		*pImage = img;
		*width = img->Bitmap.width();
		*height = img->Bitmap.height();
		SkCanvas*wat;
		return true;
	}

	extern SkData* SaveImage(BitmapContainer*pImage, PerspexImageType ptype, int quality)
	{
		SkImageEncoder::Type type;
		if (ptype == PerspexImageType::piPng)
			type = SkImageEncoder::kPNG_Type;
		if (ptype == PerspexImageType::piGif)
			type = SkImageEncoder::kGIF_Type;
		if (ptype == PerspexImageType::piJpeg)
			type = SkImageEncoder::kJPEG_Type;

		return SkImageEncoder::EncodeData(pImage->Bitmap, type, quality);
	}

	extern void DrawImage(RenderingContext*ctx, BitmapContainer*image, float opacity, SkRect* srcRect, SkRect*destRect)
	{
		SkCanvas* c = ctx->Canvas;
		SkPaint paint;
		paint.setColor(SkColorSetARGB(opacity * 255, 255, 255, 255));
		ctx->Canvas->drawBitmapRect(image->Bitmap, *srcRect, *destRect, &paint);
	}

	extern BitmapContainer* CreateRenderTargetBitmap(int width, int height)
	{
		BitmapContainer*rv = new BitmapContainer();
		rv->Bitmap.allocN32Pixels(width, height);
		return rv;
	}

	extern void ResizeBitmap(BitmapContainer *bmp, int width, int height)
	{
		bmp->Resize(width, height);
	}

	extern void DisposeImage(BitmapContainer* bmp)
	{
		delete bmp;
	}
}