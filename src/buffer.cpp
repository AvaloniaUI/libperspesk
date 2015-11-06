#include "common.h"


extern void DestroySkData(SkData*buffer)
{
	buffer->unref();
}