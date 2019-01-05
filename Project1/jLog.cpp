#include <stdio.h>
#include <stdarg.h>
#include <string>

#include "jLog.h"
void jLog::Log(const char* format, ...)
{
	va_list args;
	va_start(args,format);
	
	char buf[256] = {0,};
	vsprintf_s(buf, 256, format, args);
	printf(buf);

	va_end(args);
}
