#include "common.h"
#include "SkTextBlob.h"
#include "SkTypeface.h"
#include "SkUtils.h"
#include <vector>

namespace libperspesk
{
	class FormattedText
	{
	public:
		SkPaint Paint;
		PerspexFormattedText Shared;
		std::vector<PerspexFormattedTextLine> Lines;
		std::vector<SkRect> Rects;
		pchar*Data;
		int Length;
		float LineOffset;

		FormattedText(pchar* data, int length)
		{
			Length = length;
			Paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
			Paint.setStyle(SkPaint::kFill_Style);
			Paint.setAntiAlias(true);
			LineOffset = 0;

			Shared.WidthConstraint = -1.0;
			Data = new pchar[length + 1];
			memcpy(Data, data, length * 2);
			Data[length] = 0;

			//Replace 0 characters with zero-width spaces (200B)
			for (int c = 0; c < length;c++)
				if (Data[c] == 0)
					Data[c] = 0x200B; 
		}

		virtual void Draw(RenderingContext*ctx, PerspexBrush* foreground, SkPoint origin)
		{
			SkPaint paint = Paint;
			ConfigurePaint(paint, ctx, foreground);

			/*
			//Debugging code for character positions
			SkPaint dpaint;
			ctx->Canvas->save();
			ctx->Canvas->translate(origin.fX, origin.fY);
			for (int c = 0; c < Lines.size(); c++)
			{
				dpaint.setARGB(255, 0, 0, 0);
				SkRect rc;
				rc.fLeft = 0;
				rc.fTop = Lines[c].Top;
				rc.fRight = Lines[c].Width;
				rc.fBottom = rc.fTop + LineOffset;
				ctx->Canvas->drawRect(rc, dpaint);
			}
			for (int c = 0; c < Length; c++)
			{
				dpaint.setARGB(255, c % 10 * 125 / 10 + 125, (c * 7) % 10 * 250 / 10, (c * 13) % 10 * 250 / 10);
				dpaint.setStyle(SkPaint::kFill_Style);
				ctx->Canvas->drawRect(Rects[c], dpaint);
			}
			ctx->Canvas->restore();
			*/
			for (int c = 0; c < Lines.size(); c++)
			{
				PerspexFormattedTextLine &line = Lines[c];
				ctx->Canvas->drawText(&Data[line.Start], line.Length * 2, origin.fX, origin.fY + line.Top + LineOffset, paint);
			}
		}

		void UpdateExports()
		{
			Shared.LineCount = Lines.size();
			Shared.Lines = Lines.data();
			Shared.CharRects = Rects.data();
		}

		//For some reason breakText fails with large numbers...
#define MAX_LINE_WIDTH 10000

		void Build()
		{
			Lines.clear();
			Rects.resize(Length);
			memset(Rects.data(), 0, sizeof(SkRect)*Length);
			
			int curOff = 0;
			int curY = 0;

			SkPaint::FontMetrics metrics;
			float lineHeight = Paint.getFontMetrics(&metrics);
			LineOffset = -metrics.fTop;

			for (int c = 0; curOff < Length ;c++)
			{
				float lineWidth = -1;
				int measured;
				int extraSkip = 0;
				if (Shared.WidthConstraint <= 0)
				{
					measured = Length;
				}
				else
				{
					float constraint = Shared.WidthConstraint;
					if (constraint > MAX_LINE_WIDTH)
						constraint = MAX_LINE_WIDTH;
					
					measured = Paint.breakText(&Data[curOff], (Length - curOff) * 2, Shared.WidthConstraint, &lineWidth) / 2;
					if (measured == 0)
					{
						measured = 1;
						lineWidth = -1;
					}

					pchar nextChar = 1;
					if (curOff + measured < Length)
						nextChar = Data[curOff + measured];
					if (nextChar != ' ')
					{
						//Perform scan for the last space and end the line there
						for (int si = curOff + measured - 1; si > curOff; si--)
						{
							if (Data[si] == ' ')
							{
								measured = si - curOff;
								extraSkip = 1;
								Rects[si] = SkRect();
								break;
							}
						}
					}


				}
				PerspexFormattedTextLine line;
				line.Start = curOff;
				line.Length = measured;
				line.Width = lineWidth;
				line.Height = lineHeight;
				line.Top = curY;		
				
				if (line.Width < 0)
					line.Width = Rects[line.Start + line.Length - 1].fRight;
				Paint.getTextWidths(&Data[line.Start], (line.Length * 2), nullptr, &Rects[line.Start]);

				//Build character rects
				for (int c = line.Start; c < line.Start + line.Length; c++)
				{
					SkRect rc = Rects[c];
					float w= Paint.measureText(&Data[line.Start], (c - line.Start + 1) * 2);
					rc.offset(w - rc.fRight, line.Top + LineOffset);
					Rects[c] = rc;

				}
				line.Width = Paint.measureText(&Data[line.Start], (line.Length * 2));

				Lines.push_back(line);

				curY += lineHeight;
				curOff += measured + extraSkip;
			}


			UpdateExports();
		}

		~FormattedText()
		{
			delete Data;
		}
	};

	int pstrlen(pchar*str)
	{
		for (int c = 0; ;c++)
			if (str[c] == 0)
				return c;
	}

	extern SkTypeface* CreateTypeface(char* name, int style)
	{
		SkTypeface::Style sstyle = (SkTypeface::Style)style;

		SkTypeface*rv = SkTypeface::CreateFromName(name, sstyle);
		if (rv == nullptr)
			return SkTypeface::CreateFromName(nullptr, sstyle);
		return rv;
	}

	
	extern FormattedText* CreateFormattedText(pchar* text, int len, SkTypeface* typeface, float fontSize, SkPaint::Align align, PerspexFormattedText**exp)
	{
		FormattedText*rv = new FormattedText(text, len);
		rv->Paint.setTypeface(typeface);
		rv->Paint.setTextSize(fontSize);
		//rv->Paint.setTextAlign(align); //TODO: Manually align
		rv->Build();
		*exp = &rv->Shared;
		return rv;
	}

	extern void RebuildFormattedText(FormattedText*txt)
	{
		txt->Build();
	}

	extern void DestroyFormattedText(FormattedText* txt)
	{
		delete txt;
	}

	extern void DrawFormattedText(RenderingContext* ctx, PerspexBrush* brush, FormattedText* text, float x, float y)
	{
		SkPoint pt;
		pt.fX = x;
		pt.fY = y;
		text->Draw(ctx, brush, pt);
	}


}