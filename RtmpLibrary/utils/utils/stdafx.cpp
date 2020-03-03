// stdafx.cpp : 只包括标准包含文件的源文件
// Connector.pch 将是预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#ifdef _WIN32

void   dbgPrint  (const char *fmt, ...) {
	char  Msg[4096] = "[mfwsdk] ";
	va_list ap;
	va_start(ap,fmt);
	vsprintf(Msg+9,fmt,ap);
	//vfprintf(stderr,   fmt,   ap) ;
	va_end(ap);
	fprintf(stderr, Msg);
	OutputDebugStringA(Msg);
}

#endif
