// stdafx.h : ��׼ϵͳ���ļ��İ��ļ���
// ���Ǿ���ʹ�õ�������ĵ�
// �ض�����Ŀ�İ��ļ�
//

#pragma once

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#define HAVE_SYS_UIO_H


#include <stdio.h>
#include <stdlib.h>
#include <string>
 
#ifdef    _WIN32
#ifdef    __MMLIB__
#define   MMEXPORT __declspec(dllexport)
#else
#define   MMEXPORT  
#endif	  //__MMLIB__
#else
#define   MMEXPORT
#endif	  //_WIN32

#ifdef _WIN32
// ��������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ���߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ���Ϊ��Ӧ��ֵ���������� Windows ������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ���߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ���Ϊ��Ӧ��ֵ���������� Windows ������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ���߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ������Ϊ�ʺ� Windows Me ���߰汾����Ӧֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ���߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ���Ϊ��Ӧ��ֵ���������� IE ������汾��ֵ��
#endif

#include <WinSock2.h>
#include <Windows.h>
#include <ws2ipdef.h>
//#include <stdint.h>
#include <fcntl.h>
#include <time.h>

#define  pthread_self()  0

typedef int  socklen_t;
#ifdef WIN32_JNI
#include <jni.h>
#endif
 
void   dbgPrint  (const char *fmt, ...);

#endif

#if  (defined LINUX) || (defined ANDROID) 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h> 
#include <netinet/in.h>
#include <net/if.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>
 
#include <pthread.h>
#include <semaphore.h>
 
#define stricmp  strcasecmp
#define strnicmp strncasecmp
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR (~0)
#define closesocket(x)  close(x)
#define ioctlsocket     ioctl   
typedef unsigned int    BOOL;
typedef unsigned int    DWORD;
typedef unsigned int    UINT;
#define  TRUE           1
#define  FALSE          0
#define  NO_ERROR       0
 
typedef  int            SOCKET;
#include <fcntl.h>
#include <dlfcn.h>
 

#ifdef ANDROID
#include <android/native_window_jni.h>
#include <android/log.h>
 
#ifndef ANDROID_JNI
#define ANDROID_JNI
#endif
#define ANDROID_WINDOW
#include <jni.h>

#define  dbgPrint(...)  __android_log_print( ANDROID_LOG_DEBUG, "mmLib",   __VA_ARGS__)

#endif

#endif


#ifdef CLOUD
#include "cloudSmartPtr.h"
#include "atomicLong.h"
#include "genernalfun.h"
#endif

inline void getCurrentTime(std::string &timenow)
{
	char cszTime[25];
	memset(cszTime, 0, 25);

	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	sprintf(cszTime,"%04d-%02d-%02d %02d:%02d:%02d",
		timeinfo->tm_year+1900,
		timeinfo->tm_mon+1,
		timeinfo->tm_mday, 
		timeinfo->tm_hour, 
		timeinfo->tm_min, 
		timeinfo->tm_sec);

	timenow = std::string(cszTime);
}
