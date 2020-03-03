//#include "../videoSender/stdafx.h"
#include "File.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------
//构造函数
KFile::KFile(void)
{
	m_pFilePtr=NULL;
}

//---------------------------------------------------------------------------------------
//析构函数
KFile::~KFile(void)
{
}

//---------------------------------------------------------------------------------------
//打开文件
bool KFile::Open(const std::string& strFileFullPath, const std::string& strFileOpenMode)
{
	m_strFileFullPath = strFileFullPath;
	m_strFileOpenMode = strFileOpenMode;
	m_pFilePtr = ::fopen(m_strFileFullPath.c_str(), m_strFileOpenMode.c_str());
	return m_pFilePtr ? true : false;
}

//---------------------------------------------------------------------------------------
//关闭文件
void KFile::Close()
{
	if(m_pFilePtr)
	{
		::fclose(m_pFilePtr);
		m_pFilePtr=NULL;
	}
}

//---------------------------------------------------------------------------------------
// 是否打开
bool KFile::IsOpened(void)
{
	if(m_pFilePtr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//---------------------------------------------------------------------------------------
//读取数据
size_t KFile::Read(void* pData, unsigned long ulBlockSize, unsigned long ulBlockCount)
{
	return m_pFilePtr ? ::fread(pData, ulBlockSize, ulBlockCount, m_pFilePtr) : 0;
}

//---------------------------------------------------------------------------------------
//写入数据
size_t KFile::Write(const void* pData, unsigned long ulBlockSize, unsigned long ulBlockCount)
{
	return m_pFilePtr ? ::fwrite(pData, ulBlockSize, ulBlockCount, m_pFilePtr) : 0;
}

//---------------------------------------------------------------------------------------
//文件定位
long KFile::Seek(long lPos,int nOffSet)
{
	return m_pFilePtr ? ::fseek(m_pFilePtr,nOffSet,lPos) : -1;
}

//---------------------------------------------------------------------------------------
// 清空缓冲
int KFile::Flush()
{
	return m_pFilePtr ? ::fflush(m_pFilePtr) : EOF;
}

//---------------------------------------------------------------------------------------
//returns the current file position
unsigned long KFile::GetCurPos()
{
	return m_pFilePtr ? ::ftell(m_pFilePtr) : -1; 
}

//---------------------------------------------------------------------------------------
//读取字符串
char *KFile::GetString(char* pData, int nLen)
{
	return m_pFilePtr ? ::fgets(pData, nLen, m_pFilePtr) : NULL;
}

//---------------------------------------------------------------------------------------
//显示字符串
void KFile::Printf(const char* cszFormat, ...)
{
	va_list ap;
	va_start(ap, cszFormat);
	fprintf(m_pFilePtr,cszFormat,ap);
	va_end(ap);
}

//---------------------------------------------------------------------------------------
//文件大小
off_t KFile::Size()
{
	struct stat st;
	if(stat(m_strFileFullPath.c_str(), &st) == -1)
	{
		return 0;
	}
	return st.st_size;
}

//---------------------------------------------------------------------------------------
//是否结束
bool KFile::Eof()
{
	if(m_pFilePtr)
	{
		if(feof(m_pFilePtr))
		{
			return true;
		}
	}
	return false;
}

//add by taor 2014-11-07
//获取全路径
std::string KFile::GetFileFullPathName()
{
	return m_strFileFullPath;
}
//end add by taor 2014-11-07

//=======================================================================================
// 文件相关的函数
//=======================================================================================

//---------------------------------------------------------------------------------------
size_t KGetFileSize(const std::string& strFileFullPath)
{
	struct _stat FileStat;
	int result;

	result = _stat(strFileFullPath.c_str(), &FileStat);

	//检查状态是否有效
	if(result != 0 )
	{
		return 0;
	}
	else
	{
		return FileStat.st_size;
	}
}

//---------------------------------------------------------------------------------------
// 获得文件修改时间
time_t KGetFileLastModifiedTime(const std::string& strFileFullPath)
{
	struct _stat FileStat;
	int result;

	result = _stat(strFileFullPath.c_str(), &FileStat);

	//检查状态是否有效
	if(result != 0 )
	{
		return 0;
	}
	else
	{
		return FileStat.st_mtime;
	}
}

//---------------------------------------------------------------------------------------
// 文件是否存在
bool IsFileExist(const std::string& strFileFullPath)
{
	struct stat st;
	if(stat(strFileFullPath.c_str(),&st) == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//---------------------------------------------------------------------------------------
// 目录是否存在
bool IsDirExist(const std::string& dirfullpath)
{
	return IsFileExist(dirfullpath);
}

#ifdef WIN32
#define splitpath	_splitpath
#else
//---------------------------------------------------------------------------------------
// 路径分割函数
void splitpath(const char* path, char* drv, char* dir, char* name, char* ext)
{
        const char* end; /* end of processed string */
        const char* p;   /* search pointer */
        const char* s;   /* copy pointer */

        /* extract drive name */
        if (path[0] && path[1]==':') {
                if (drv) {
                        *drv++ = *path++;
                        *drv++ = *path++;
                        *drv = '\0';
                }
        } else if (drv)
                *drv = '\0';

        /* search for end of string or stream separator */
        for(end=path; *end && *end!=':'; )
                end++;

        /* search for begin of file extension */
        for(p=end; p>path && *--p!='\\' && *p!='/'; )
                if (*p == '.') {
                        end = p;
                        break;
                }

        if (ext)
                for(s=end; (*ext=*s++); )
                        ext++;

        /* search for end of directory name */
        for(p=end; p>path; )
                if (*--p=='\\' || *p=='/') {
                        p++;
                        break;
                }

        if (name) {
                for(s=p; s<end; )
                        *name++ = *s++;

                *name = '\0';
        }

        if (dir) {
                for(s=path; s<p; )
                        *dir++ = *s++;

                *dir = '\0';
        }
}

#endif

//---------------------------------------------------------------------------------------
// 分割文件路径
void SplitFilePath(const std::string& strFileFullPath,std::string& driver,std::string& directory,std::string& filename,std::string& extension)
{
	char szDriver[3];
	char szDirectory[256];
	char szFileName[256];
	char szExtension[256];
	splitpath(strFileFullPath.c_str(),szDriver,szDirectory,szFileName,szExtension);
	driver		= szDriver;
	directory	= szDirectory;
	filename	= szFileName;
	extension	= szExtension;
}

//---------------------------------------------------------------------------------------
std::string TrimRightPath(const std::string& strPath)
{
	int nLength = strPath.length();
	if(strPath[nLength-1]=='/' || strPath[nLength-1]=='\\')
	{
		return strPath.substr(0,nLength-1);
	}
	return strPath;
}

//---------------------------------------------------------------------------------------
std::string TrimLeftPath(const std::string& strPath)
{
	int nLength = strPath.length();
	if(strPath[0]=='/' || strPath[0]=='\\')
	{
		return strPath.substr(1,nLength);
	}
	return strPath;
}

//---------------------------------------------------------------------------------------
std::string TrimAllPath(const std::string& strPath)
{
	return TrimLeftPath(TrimRightPath(strPath));
}

//---------------------------------------------------------------------------------------
// 创建目录
bool MakeDir(const std::string& strPath)
{
	std::string strDir = TrimRightPath(strPath);
	std::string strExecute="";
#ifdef WIN32
	strExecute = "mkdir " + strDir;
#else
	strExecute = "mkdir -p " + strDir;
#endif
	//std::system(strExecute.c_str());
	return IsDirExist(strDir);
}
