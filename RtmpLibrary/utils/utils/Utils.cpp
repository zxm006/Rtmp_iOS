//#include "../videoSender/stdafx.h"
#include "Utils.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include "File.h"
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#ifdef _LINUX
#include <sys/vfs.h>
#include <linux/if.h>
#include <sys/socket.h>
#endif
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#endif


void makedir( std::string strpath)
{
    // 逐级创建
	std::string::size_type pos = 0, prev_pos = 0;
	std::string strCurPath;
	while ( (pos = strpath.find_first_of('\\', pos)) != std::string::npos)
    {
        strCurPath = strpath.substr( 0, pos);
        if ( access( strCurPath.c_str(), 00) == -1)// 判断文件是否存在
        {
#ifdef _WIN32
            mkdir( strCurPath.c_str());
#else
			mkdir( strCurPath.c_str(),S_IXOTH);
#endif
        }
        pos++;
    }
}

void convpathformat( std::string& strpath)
{
	std::string::size_type pos = strpath.find( '/' );
	while ( pos != std::string::npos )
    {
        strpath.replace( pos, 1, "\\");
        pos = strpath.find( '/');
    }
}

unsigned long get_disk_free( const std::string& strPath)
{
   
#if 0
    unsigned _int64 i64Free = 0;

    unsigned _int64 i64FreeBytesToCaller;
    unsigned _int64 i64TotalBytes;
    unsigned _int64 i64FreeBytes;

	std::string strDisk = strPath.substr( 0, 3);
    BOOL fResult = GetDiskFreeSpaceEx (
        strDisk.c_str(),
        (PULARGE_INTEGER)&i64FreeBytesToCaller,
        (PULARGE_INTEGER)&i64TotalBytes,
        (PULARGE_INTEGER)&i64FreeBytes);
    // 
    if ( fResult )
    {
        i64Free = i64FreeBytesToCaller / Msize;
    }
    return i64Free;
#endif
    return 10000000;
}

void deletefile( const std::string& strfilename)
{
    if ( IsFileExist(strfilename) )
    {
        unlink(strfilename.c_str());
    }
}

//---------------------------------------------------------------------------------------
//初始化随机数
void InitRandom(void)
{
	srand((unsigned)time(NULL));
}

//---------------------------------------------------------------------------------------
//获得一个随机数
int Random(int nNumber)
{ 
    return (int)(nNumber/(float)RAND_MAX * rand()); 
}

//---------------------------------------------------------------------------------------
//获得当前绝对时间
unsigned long GetTimeStamp(void)
{
#ifdef WIN32
	return GetTickCount();//毫秒
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000; 
#endif
}

//---------------------------------------------------------------------------------------
//获得高精度绝对时间
unsigned long GetMicroTimeStamp(void)
{
#ifdef WIN32
	LARGE_INTEGER frequence, privious;
	if(!QueryPerformanceFrequency( &frequence))//取高精度运行计数器的频率
	{
		return GetTickCount();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return GetTickCount();
	}

	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //换算到微秒数

	return dwRet;//微秒
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec; 
#endif
}

//  [12/23/2014 Taor]
/* 获取当前时间(字符串) */
void getDate(const long utime_t,std::string &timenow)
{
	char cszTime[25];
	memset(cszTime, 0, 25);

	time_t rawtime = utime_t;
	struct tm * timeinfo;
	//time ( &rawtime );
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

//---------------------------------------------------------------------------------------
//休眠(毫秒)
void Pause(unsigned long ulMS)
{
#ifdef WIN32
	Sleep(ulMS);
#else
	usleep(ulMS*1000);
#endif
}

//---------------------------------------------------------------------------------------
inline void base64encode(unsigned char* buffer,size_t bufferlen,std::string& encodedstr)
{
	const std::string ENCODETAB = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	bool add_crlf = false;
	size_t i = 0;
	size_t o = 0;
	
	encodedstr = "";
	while (i < bufferlen)
	{
		size_t remain = bufferlen - i;
		if(add_crlf && o && o % 76 == 0)
		{
			encodedstr += "\n";
		}
		switch (remain)
		{
		case 1:
			encodedstr += ENCODETAB[ ((buffer[i] >> 2) & 0x3f) ];
			encodedstr += ENCODETAB[ ((buffer[i] << 4) & 0x30) ];
			encodedstr += "==";
			break;
		case 2:
			encodedstr += ENCODETAB[ ((buffer[i] >> 2) & 0x3f) ];
			encodedstr += ENCODETAB[ ((buffer[i] << 4) & 0x30) + ((buffer[i + 1] >> 4) & 0x0f) ];
			encodedstr += ENCODETAB[ ((buffer[i + 1] << 2) & 0x3c) ];
			encodedstr += "=";
			break;
		default:
			encodedstr += ENCODETAB[ ((buffer[i] >> 2) & 0x3f) ];
			encodedstr += ENCODETAB[ ((buffer[i] << 4) & 0x30) + ((buffer[i + 1] >> 4) & 0x0f) ];
			encodedstr += ENCODETAB[ ((buffer[i + 1] << 2) & 0x3c) + ((buffer[i + 2] >> 6) & 0x03) ];
			encodedstr += ENCODETAB[ (buffer[i + 2] & 0x3f) ];
		}
		o += 4;
		i += 3;
	}
}

//---------------------------------------------------------------------------------------
void Base64Encode(const std::string& InBuffer,std::string& OutBuffer)
{
	base64encode((unsigned char*)InBuffer.c_str(),InBuffer.size(),OutBuffer);
}

//---------------------------------------------------------------------------------------
void Base64Decode(const std::string& InBuffer,std::string& OutBuffer)
{
	const char DECODETAB[] =
	{
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  62,   0,   0,   0,  63, 
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61,   0,   0,   0,   0,   0,   0, 
	  0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14, 
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   0,   0,   0,   0,   0, 
	  0,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40, 
	 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,   0,   0,   0,   0,   0
	};

	size_t i = 0;
	size_t l = InBuffer.length();
	
	OutBuffer = "";
	while (i < l)
	{
		while (i < l && (InBuffer[i] == 13 || InBuffer[i] == 10))
		{
			i++;
		}
		if (i < l)
		{
			char b1 = (char)((DECODETAB[(int)InBuffer[i]] << 2 & 0xfc) + (DECODETAB[(int)InBuffer[i + 1]] >> 4 & 0x03));
			OutBuffer += b1;
			if (InBuffer[i + 2] != '=')
			{
				char b2 = (char)((DECODETAB[(int)InBuffer[i + 1]] << 4 & 0xf0) + (DECODETAB[(int)InBuffer[i + 2]] >> 2 & 0x0f));
				OutBuffer += b2;
			}
			if (InBuffer[i + 3] != '=')
			{
				char b3 = (char)((DECODETAB[(int)InBuffer[i + 2]] << 6 & 0xc0) + DECODETAB[(int)InBuffer[i + 3]]);
				OutBuffer += b3;
			}
			i += 4;
		}
	}
}

//---------------------------------------------------------------------------------------
//从文件中读取并编码
size_t Base64EncodeFromFile(const std::string& filename,std::string& encodedstr)
{
	if(filename.empty())
	{
		return 0;
	}

	FILE* fp = ::fopen(filename.c_str(),"rb");
	if(fp==NULL)
	{
		return 0;
	}

	while(!feof(fp))
	{
		unsigned char pBuffer[2048];
		size_t readlen=::fread(pBuffer,sizeof(unsigned char),2048,fp);
		std::string tmpstr;
		base64encode(pBuffer,readlen,tmpstr);
		encodedstr=encodedstr+tmpstr;
	}
	::fclose(fp);
	return encodedstr.length();
}

//---------------------------------------------------------------------------------------
//解码到文件
void Base64DecodeToFile(const std::string& encodedstr,const std::string& filename)
{
	std::string decodedstr="";
	Base64Decode(encodedstr,decodedstr);
	int decodedlen=decodedstr.length();
	const char* pData=decodedstr.c_str();
	FILE* fp = ::fopen(filename.c_str(),"wb");
	if(fp==NULL)
		return;

	for(int i=0;i<decodedlen;i++)
	{
		::fwrite((char*)pData+i,sizeof(char),1,fp);
	}
	::fclose(fp);
}
/*
//---------------------------------------------------------------------------------------
std::string UPPERCASE(const std::string& source)
{
	std::string destination=source;
	std::transform(destination.begin(), destination.end(), destination.begin(), ::toupper);
	return destination;
}

//---------------------------------------------------------------------------------------
std::string LOWERCASE(const std::string& source)
{
	std::string destination=source;
	std::transform(destination.begin(), destination.end(), destination.begin(), ::tolower);
	return destination;
}

//---------------------------------------------------------------------------------------
std::string TRIMLEFT(const std::string& source, const char* chars2remove)
{
	if(source.empty())
	{
		return "";
	}
	std::string destination=source;
	std::string::size_type pos = destination.find_first_not_of(chars2remove);
	if(pos != std::string::npos)
	{
		destination.erase(0,pos);
	}
	else
	{
		destination.erase(destination.begin(),destination.end()); // make empty
	}
	return destination;
}

//---------------------------------------------------------------------------------------
std::string TRIMRIGHT(const std::string& source, const char* chars2remove)
{
	if(source.empty())
	{
		return "";
	}
	std::string destination=source;
	std::string::size_type pos = destination.find_last_not_of(chars2remove);
	if(pos != std::string::npos)
	{
		destination.erase(pos+1);
	}
	else
	{
		destination.erase(destination.begin(),destination.end()); // make empty
	}
	return destination;
}

//---------------------------------------------------------------------------------------
std::string TRIMSPACE(const std::string& source)
{
	return TRIMRIGHT(TRIMLEFT(source," ")," ");
}

//---------------------------------------------------------------------------------------
std::string LONG2STR(long value)
{
	char tmp[100];
	sprintf(tmp,"%d",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
std::string ULONG2STR(unsigned long value)
{
	char tmp[100];
	sprintf(tmp,"%u",value);
	return tmp;
}


//---------------------------------------------------------------------------------------
std::string INT2STR(int value)
{
	char tmp[100];
	sprintf(tmp,"%d",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
std::string UINT2STR(unsigned int value)
{
	char tmp[100];
	sprintf(tmp,"%u",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
std::string SHORT2STR(short value)
{
	char tmp[100];
	sprintf(tmp,"%d",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
std::string USHORT2STR(unsigned short value)
{
	char tmp[100];
	sprintf(tmp,"%u",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
std::string BYTE2STR(unsigned char value)
{
	char tmp[100];
	sprintf(tmp,"%d",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
std::string BOOL2STR(bool value)
{
	return value?"true":"false";
}

//---------------------------------------------------------------------------------------
std::string DOUBLE2STR(double value)
{
	char tmp[100];
	sprintf(tmp,"%f",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
long STR2LONG(const std::string& value)
{
	return strtol(value.c_str(),NULL,10);
}

//---------------------------------------------------------------------------------------
unsigned long STR2ULONG(const std::string& value)
{
	return strtoul(value.c_str(),NULL,10);
}

//---------------------------------------------------------------------------------------
int STR2INT(const std::string& value)
{
	return atoi(value.c_str());
}

//---------------------------------------------------------------------------------------
unsigned int STR2UINT(const std::string& value)
{
	return atoi(value.c_str());
}

//---------------------------------------------------------------------------------------
short STR2SHORT(const std::string& value)
{
	return atoi(value.c_str());
}

//---------------------------------------------------------------------------------------
unsigned short STR2USHORT(const std::string& value)
{
	return atoi(value.c_str());
}

//---------------------------------------------------------------------------------------
unsigned char STR2BYTE(const std::string& value)
{
	return atoi(value.c_str());
}

//---------------------------------------------------------------------------------------
bool STR2BOOL(const std::string& value)
{
	return value=="true"?true:false;
}

//---------------------------------------------------------------------------------------
double STR2DOUBLE(const std::string& value)
{
	return atof(value.c_str());
}

//---------------------------------------------------------------------------------------
unsigned short BOOL2USHORT(bool value)
{
	return value?1:0;
}

//---------------------------------------------------------------------------------------
bool USHORT2BOOL(unsigned short value)
{
	return value==1?true:false;
}

//---------------------------------------------------------------------------------------
std::string ULONG2HEX(unsigned long value)
{
	char tmp[100];
	sprintf(tmp,"%X",value);
	return tmp;
}

//---------------------------------------------------------------------------------------
unsigned long HEX2ULONG(const std::string& value)
{
	unsigned long ulValue=0;
	sscanf(value.c_str(),"%X",&ulValue);
	return ulValue;
}

//---------------------------------------------------------------------------------------
std::string NONULLSTR(const char* value)
{
	return value==NULL?"":value;
}
*/
//---------------------------------------------------------------------------------------
/*STRING_MAP StrSplit(const std::string& data,const std::string& token)
{
	STRING_MAP mapStringSplit;

	int datalen=(int)data.length();
	int tokenlen=(int)token.length();
	int start=0;
	int end=0;
	int off=0;
	int count=0;
	while(end>=0)
	{
		end=(int)data.find(token,start);
		off=end-start;
		std::string subdata=data.substr(start,off);
		if(!subdata.empty())
		{
			mapStringSplit.insert(STRING_MAP::value_type(count,subdata));
		}
		start=end+tokenlen;
		count++;
	}
	return mapStringSplit;
}

//---------------------------------------------------------------------------------------
STRING_MAP_EX StrSplitEx(const std::string& data,const std::string& token)
{
	STRING_MAP_EX mapStringSplit;
	STRING_MAP mapSegment = StrSplit(data,token);
	for(int i=0;i<mapSegment.size();i++)
	{
		std::string seg = mapSegment[i];
		STRING_MAP b = StrSplit(seg,"=");
		std::string name = b[0];
		std::string value = b[1];
		mapStringSplit.insert(STRING_MAP_EX::value_type(name,value));
	}
	return mapStringSplit;
}*/

//---------------------------------------------------------------------------------------
unsigned long GetFileSize(const std::string& strFileFullPath)
{
	struct stat st;
	if(stat(strFileFullPath.c_str(), &st) == -1)
	{
		return 0;
	}
	return (unsigned long)st.st_size;
}

//---------------------------------------------------------------------------------------
bool DeleteFile(const std::string& strFileFullPath)
{
	if(::unlink(strFileFullPath.c_str())!=0)
	{
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------
pid_t GetProcessID(void)
{
	return ::getpid();
}

//---------------------------------------------------------------------------------------
/*bool URLSplit(const std::string& url,std::string& protocol,std::string& serveraddr,unsigned short& serverport,std::string& filepath)
{
	std::string tempstr;
	size_t pos = url.find(':');

	if (pos == std::string::npos)
	{
		return false;
	}

	protocol = url.substr(0, pos);
	tempstr = url.substr(pos+1);
	pos = tempstr.find("//");
	if((pos == std::string::npos) || (pos != 0))
	{
		return false;
	}

	tempstr = tempstr.substr(pos+2);
	pos = tempstr.find('/');
	if(pos == std::string::npos)
	{
		return false;
	}

	size_t pos2 = tempstr.find(':');
	std::string portstr;
	if(pos2 == std::string::npos)
	{
		serveraddr = tempstr.substr(0, pos);
		filepath = tempstr.substr(pos);
	}
	else if(pos2 < pos)
	{
		portstr = tempstr.substr(pos2 + 1, pos - pos2 - 1);
		serveraddr = tempstr.substr(0, pos2);
		filepath = tempstr.substr(pos);
	}
	
	if(portstr == "")
	{
		serverport = 80;
	}
	else
	{
		serverport = STR2USHORT(portstr.c_str());		
	}
	return true;
}*/

//#include "zlib/zlib.h"

//---------------------------------------------------------------------------------------
// 内存压缩
bool Compress(const char* pSourceData,int nSourceLen,char* pDestData,int& nDestLen)
{
#if 0
	memset(pDestData,0,nDestLen);
	uLong ulDestLen = compressBound(nSourceLen);
	if(compress((Bytef*)pDestData,&ulDestLen,(const Bytef*)pSourceData,nSourceLen)==Z_OK)
	{
		nDestLen = ulDestLen;
		return true;
	}
#endif
	return false;
}

//---------------------------------------------------------------------------------------
// 内存解压
bool UnCompress(const char* pSourceData,int nSourceLen,char* pDestData,int& nDestLen)
{
#if 0
	memset(pDestData,0,nDestLen);
	if(uncompress((Bytef*)pDestData,(uLongf*)&nDestLen,(const Bytef*)pSourceData,nSourceLen)==Z_OK)
	{
		return true;
	}
#endif
	return false;
}

#ifdef WIN32

#if 0
//---------------------------------------------------------------------------------------
//将通用字符串编码成UTF8字符串
std::string U2A(CString csUnicodeString)
{
	int nUnicodeCharCount=WideCharToMultiByte(CP_UTF8,0,csUnicodeString.GetBuffer(),-1,NULL,0,NULL,NULL) ;
	char* szAnsiString = new char[nUnicodeCharCount+1];
	WideCharToMultiByte(CP_UTF8,0,(PWSTR)csUnicodeString.GetBuffer(),-1,szAnsiString,nUnicodeCharCount,NULL,NULL) ;
	std::string result = szAnsiString;
	delete[]szAnsiString;
	szAnsiString=NULL;
	return result;
}

//---------------------------------------------------------------------------------------
//将UTF8字符串解码成通用字符串
CString A2U(const std::string& strAnsiString)
{
	int nUnicodeCharCont=MultiByteToWideChar(CP_UTF8,0,strAnsiString.c_str(),-1,NULL,0);
	WCHAR* szUnicodeString = new WCHAR[nUnicodeCharCont+1];
	MultiByteToWideChar(CP_UTF8,0,strAnsiString.c_str(),-1,szUnicodeString,nUnicodeCharCont);
	CString result(szUnicodeString);
	delete[]szUnicodeString;
	szUnicodeString=NULL;
	return result;
}
#endif

#endif

//---------------------------------------------------------------------------------------
unsigned long GetBitMask(unsigned int nBit)
{
	unsigned long ulBitMask=0;
	for(int i=1; i < nBit+1; i++)
	{
		ulBitMask = ( 1 << ( 32 - i ) ) | ulBitMask;
	}
	return ulBitMask;
}

//---------------------------------------------------------------------------------------
unsigned long SetPrivilege(unsigned long ulPrivilege,unsigned char ucBit,bool bEnabled)
{
	unsigned long ulMask = 1 << (ucBit-1);
	if(bEnabled)
	{
		return ulPrivilege | ulMask;
	}
	else
	{
		unsigned long ulMaskEx = ~ulMask;
		return ulPrivilege & ulMaskEx;
	}
}

//---------------------------------------------------------------------------------------
bool GetPrivilege(unsigned long ulPrivilege,unsigned char ucBit)
{
	unsigned long ulMask = 1 << (ucBit-1);
	unsigned long a = ulPrivilege & ulMask;
	if(a!=0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
