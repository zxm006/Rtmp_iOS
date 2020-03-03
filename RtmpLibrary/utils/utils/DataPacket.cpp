#include "stdafx.h"
#include "DataPacket.h"
#include <stdio.h>

#ifndef WIN32
#include <netinet/in.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//=======================================================================================
// KDataInPacket
//=======================================================================================

//---------------------------------------------------------------------------------------
void KDataOutPacket::Write16(unsigned short w)
{
	if(m_pCursor - m_pBuffer + sizeof(w) > m_nBufferLength)
	{
		int nFilled=m_pCursor - m_pBuffer;
		m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(w))*2;
		m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
		if (m_pBuffer==NULL)
		{
			m_nBufferLength=0;
			Reset();
			return;
		}
		m_pCursor=m_pBuffer+nFilled;
	}

	*(unsigned short *)m_pCursor = w;
	m_pCursor += sizeof(w);
}

//---------------------------------------------------------------------------------------
void KDataOutPacket::Write32(unsigned int dw)
{
	if(m_pCursor - m_pBuffer + sizeof(dw) > m_nBufferLength)
	{
		int nFilled=m_pCursor - m_pBuffer;
		m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(dw))*2;
		m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
		if (m_pBuffer==NULL)
		{
			m_nBufferLength=0;
			Reset();
			return;
		}
		m_pCursor=m_pBuffer+nFilled;
	}

	*(unsigned int *) m_pCursor = dw;
	m_pCursor += sizeof(dw);
}

//---------------------------------------------------------------------------------------
void KDataOutPacket::Write64(unsigned long lw)
{
	if(m_pCursor - m_pBuffer + sizeof(lw) > m_nBufferLength)
	{
		int nFilled=m_pCursor - m_pBuffer;
		m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(lw))*2;
		m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
		if (m_pBuffer==NULL)
		{
			m_nBufferLength=0;
			Reset();
			return;
		}
		m_pCursor=m_pBuffer+nFilled;
	}

	*(unsigned long *) m_pCursor = lw;
	m_pCursor += sizeof(lw);
}

//---------------------------------------------------------------------------------------
void KDataOutPacket::WriteData(unsigned char *buf, unsigned int n)
{
	if(m_pCursor - m_pBuffer + n > m_nBufferLength)
	{
		int nFilled=m_pCursor - m_pBuffer;
		m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + n)*2;
		m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
		if (m_pBuffer==NULL)
		{
			m_nBufferLength=0;
			Reset();
			return;
		}
		m_pCursor=m_pBuffer+nFilled;
	}

	memcpy(m_pCursor, buf, n);
	m_pCursor += n;
}

//---------------------------------------------------------------------------------------
void KDataOutPacket::WriteString(const char *str, unsigned int n)
{
	unsigned short len = n + 1;
	if(m_pCursor - m_pBuffer + n + sizeof(len) > m_nBufferLength)
	{
		int nFilled=m_pCursor - m_pBuffer;
		m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + n + sizeof(len))*2;
		m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
		if (m_pBuffer==NULL)
		{
			m_nBufferLength=0;
			Reset();
			return;
		}
		m_pCursor=m_pBuffer+nFilled;
	}

	*this << len;
	memcpy(m_pCursor, str, len);
	m_pCursor += len;
}

//---------------------------------------------------------------------------------------
KDataOutPacket &KDataOutPacket::operator <<(unsigned char b)
{
	if(m_pCursor - m_pBuffer + sizeof(b) > m_nBufferLength)
	{
		int nFilled=m_pCursor - m_pBuffer;
		m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(b))*2;
		m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
		if (m_pBuffer==NULL)
		{
			m_nBufferLength=0;
			Reset();
			return *this;
		}
		m_pCursor=m_pBuffer+nFilled;
	}

	*(unsigned char *) m_pCursor = b;
	m_pCursor += sizeof(b);

	return *this;
}

//---------------------------------------------------------------------------------------
KDataOutPacket &KDataOutPacket::operator <<(unsigned short w)
{
	Write16(htons(w));
	return *this;
}

//---------------------------------------------------------------------------------------
KDataOutPacket &KDataOutPacket::operator <<(unsigned int dw)
{
	Write32(htonl(dw));
	return *this;
}

//---------------------------------------------------------------------------------------
KDataOutPacket &KDataOutPacket::operator <<(unsigned long lw)
{
	Write64(htonl(lw));
	return *this;
}

//---------------------------------------------------------------------------------------
KDataOutPacket &KDataOutPacket::operator <<(const char *str)
{
	WriteString(str, (int)strlen(str));
	return *this;
}

//=======================================================================================
// KDataInPacket
//=======================================================================================

//---------------------------------------------------------------------------------------
unsigned short KDataInPacket::Read16()
{
	unsigned short w = 0;
	if (m_pCursor <= m_pData + m_nDataLen - sizeof(w))
	{
		w = *(unsigned short *) m_pCursor;
		m_pCursor += sizeof(w);
	}
	return w;
}

//---------------------------------------------------------------------------------------
unsigned int KDataInPacket::Read32()
{
	unsigned int dw = 0;
	if (m_pCursor <= m_pData + m_nDataLen - sizeof(dw))
	{
		dw = *(unsigned int *) m_pCursor;
		m_pCursor += sizeof(dw);
	}
	return dw;
}

//---------------------------------------------------------------------------------------
unsigned int KDataInPacket::Read64()
{
	unsigned long lw = 0;
	if (m_pCursor <= m_pData + m_nDataLen - sizeof(lw))
	{
		lw = *(unsigned long *) m_pCursor;
		m_pCursor += sizeof(lw);
	}
	return lw;
}

//---------------------------------------------------------------------------------------
unsigned char *KDataInPacket::ReadData(int &n)
{
	n = (int)(m_nDataLen - (m_pCursor - m_pData));
	return m_pCursor;
}

//---------------------------------------------------------------------------------------
KDataInPacket &KDataInPacket::operator >>(unsigned char &b)
{
		b = 0;
		if (m_pCursor <= m_pData + m_nDataLen - sizeof(b))
			b = *m_pCursor++;
		return *this;
}

//---------------------------------------------------------------------------------------
KDataInPacket &KDataInPacket::operator >>(unsigned short &w)
{
	w = ntohs(Read16());
	return *this;
}

//---------------------------------------------------------------------------------------
KDataInPacket &KDataInPacket::operator >> (unsigned int &dw)
{
	dw = ntohl(Read32());
	return *this;
}

//---------------------------------------------------------------------------------------
KDataInPacket &KDataInPacket::operator >>(unsigned long &lw)
{
	lw = ntohl(Read64());
	return *this;
}

//---------------------------------------------------------------------------------------
KDataInPacket &KDataInPacket::operator >> (const char* &str)
{
	unsigned short len;
	operator >> (len);
	if (len && m_pCursor <= m_pData + m_nDataLen - len && !m_pCursor[len - 1])
	{
		str = (char *)m_pCursor;
		m_pCursor += len;
	}
	else
	{
		str = "";
	}
	return *this;
}

//---------------------------------------------------------------------------------------
KDataInPacket &KDataInPacket::operator >> (char* str)
{
	const char* cszStr=NULL;
	unsigned short len;
	operator >> (len);
	if (len && m_pCursor <= m_pData + m_nDataLen - len && !m_pCursor[len - 1])
	{
		cszStr = (char *)m_pCursor;
		m_pCursor += len;
	}
	else
	{
		cszStr = "";
	}
	strcpy(str,cszStr);
	return *this;
}
