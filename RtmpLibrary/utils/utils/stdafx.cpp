// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Connector.pch ����Ԥ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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
