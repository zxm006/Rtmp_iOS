#include "stdafx.h"
#include "Buffer.h"
//#include "ZipEx.h"

#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFFER_BLOCK_SIZE  1024       // Change the allocated size of the buffer in 1 KB sized blocks
#define BUFFER_BLOCK_MASK  0xFFFFFC00 // Aids in rounding to the next biggest KB of size

//---------------------------------------------------------------------------------------
KBuffer::KBuffer()
{
	m_pBufferData=NULL;
	m_nBufferSize=0;
	m_nBufferBound=0;
}

//---------------------------------------------------------------------------------------
KBuffer::~KBuffer()
{
	if(m_pBufferData)
	{
		free(m_pBufferData);
		m_pBufferData=NULL;
	}
}

//---------------------------------------------------------------------------------------
bool KBuffer::SetBuffer(const void* pBufferData,size_t nBufferSize)
{
	if(pBufferData==NULL || nBufferSize==0)
	{
		return false;
	}

	// 预设置缓冲区边界大小(1024进制)
	// 1-1024 becomes 1024, 1025 becomes 2048, and so on.
	m_nBufferBound = ( nBufferSize + BUFFER_BLOCK_SIZE - 1 ) & BUFFER_BLOCK_MASK;

	//分配内存
	m_pBufferData=(unsigned char*)realloc(m_pBufferData,m_nBufferBound );
	if(m_pBufferData==NULL)
	{
		return false;
	}
	memcpy(m_pBufferData,pBufferData,nBufferSize);
	m_nBufferSize = nBufferSize;
	return true;
}

//---------------------------------------------------------------------------------------
unsigned char* KBuffer::GetBuffer(void)
{
	return m_pBufferData;
}

//---------------------------------------------------------------------------------------
size_t KBuffer::GetSize(void)
{
	return m_nBufferSize;
}

//---------------------------------------------------------------------------------------
bool KBuffer::Compress(void)
{
#if 0
	unsigned long nCompressLength = 0;
	unsigned char* pCompressData=KZipEx::Compress(m_pBufferData, static_cast<unsigned long>(m_nBufferSize), &nCompressLength);
	if(pCompressData==NULL || nCompressLength==0)
	{
		return false;
	}
	bool bDone = SetBuffer(pCompressData,nCompressLength);
	delete[] pCompressData;
	return bDone;
#endif

	return false;
}

//---------------------------------------------------------------------------------------
bool KBuffer::Decompress(void)
{
#if 0
	unsigned long nDecompressLength = 0;
	unsigned char* pDecompressData=KZipEx::Decompress(m_pBufferData, static_cast<unsigned long>(m_nBufferSize), &nDecompressLength);
	if(pDecompressData==NULL || nDecompressLength==0)
	{
		return false;
	}
	bool bDone = SetBuffer(pDecompressData,nDecompressLength);
	delete[] pDecompressData;
	return bDone;
#endif

	return false;
}

////=======================================================================================
//// KBufferEx
////=======================================================================================
//
////---------------------------------------------------------------------------------------
//KBufferEx::KBufferEx()
//{
//	m_ulID = 0;
//}
//
////---------------------------------------------------------------------------------------
//KBufferEx::~KBufferEx()
//{
//}

