#include "SkColor.h"
#include "SkPoint.h"
namespace libperspesk {
	enum PerspexGeometryElementType
	{
		pgLineTo,
		pgArcTo,
		pgBezierTo,
		pgBeginFigure,
		pgEndFigure
	};

	enum PerspexBrushType
	{
		brSolid,
		brLinearGradient,
		brRadialGradient,
		brImage
	};

	enum PerspexFontStyle
	{
		fsNormal,
		fsBold,
		fsItalic
	};

	enum PerspexImageType
	{
		piPng, piGif, piJpeg
	};

	enum PerspexGradientSpreadMethod
	{
		grPad,
		grReflect,
		grRepeat,
	};

	enum PerspexPenLineCap
	{
		plcFlat,
		plcRound,
		plcSquare,
		plcTriangle,
	};

	enum PerspexPenLineJoin
	{
		pnjBevel,
		pnjMiter,
		pnjRound,
	};

	enum PerspexTileMode
	{
		ptmNone,
		ptmFlipX,
		ptmFlipY,
		ptmFlipXY,
		ptmTile
	};

	struct PerspexGeometryElement
	{
		PerspexGeometryElementType Type;
		SkPoint Points[3];
		bool Flag;
	};


	struct PerspexFormattedTextLine
	{
		float Top;
		int Start;
		int Length;
		float Height;
		float Width;
	};

	struct PerspexFormattedText
	{
		float WidthConstraint;
		int LineCount;
		PerspexFormattedTextLine*Lines;
		SkRect*CharRects;
	};

	class BitmapContainer;

#define PERSPEX_MAX_GRADIENT_STOPS 1024
#define PERSPEX_MAX_DASHES 1024
	struct PerspexBrush
	{
		PerspexBrushType Type;
		double Opacity;
		uint32_t Color;

		//Strokes
		bool Stroke;
		float StrokeThickness;
		PerspexPenLineJoin StrokeLineJoin;
		float StrokeMiterLimit;
		int StrokeDashCount;
		float StrokeDashOffset;
		PerspexPenLineCap StrokeLineCap;

		//Gradients
		int GradientStopCount;
		PerspexGradientSpreadMethod GradientSpreadMethod;
		SkPoint GradientStartPoint, GradientEndPoint;
		float GradientRadius;

		//Tiling
		BitmapContainer*Bitmap;
		PerspexTileMode BitmapTileMode;
		SkPoint BitmapTranslation;

		//Blobs
		SkColor GradientStopColors[PERSPEX_MAX_GRADIENT_STOPS];
		float GradientStops[PERSPEX_MAX_GRADIENT_STOPS];
		float StrokeDashes[PERSPEX_MAX_DASHES];
	};

	struct PerspexBuffer
	{
		int Size;
		char* Data;
	};

	struct PerspexDrawingContextSettings
	{
		double Opacity;
	};
}