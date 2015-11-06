#include "common.h"

namespace libperspesk
{
	extern SkPath* CreatePath(PerspexGeometryElement* elements, int count, SkRect* bounds)
	{
		SkPath* rv = new SkPath();
		for (int c = 0; c < count; c++)
		{
			PerspexGeometryElement& el = elements[c];
			if (el.Type == PerspexGeometryElementType::pgArcTo)
			{
				float radius = el.Points[2].fX;
				rv->arcTo(el.Points[0], el.Points[1], radius);
			}
			else if (el.Type == PerspexGeometryElementType::pgLineTo)
				rv->lineTo(el.Points[0]);
			else if (el.Type == PerspexGeometryElementType::pgBezierTo)
				rv->cubicTo(el.Points[0], el.Points[1], el.Points[2]);
			else if (el.Type == PerspexGeometryElementType::pgBeginFigure)
				//TODO: Do something about isFilled in Flag
				rv->moveTo(el.Points[0]);
			else if (el.Type == PerspexGeometryElementType::pgEndFigure)
			{
				if (el.Flag)
					rv->close();
			}
		}
		*bounds = rv->getBounds();
		return rv;
	}

	extern void DisposePath(SkPath*path)
	{
		delete path;
	}
}