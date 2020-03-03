//#include "../videoSender/stdafx.h"
#include "MD5.h"
#include <assert.h>
#ifdef WIN32
#include <windows.h>
#endif

#ifndef WIN32
#define ASSERT(x) assert(x)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
// GetMD5
std::string KMD5::GetFileMD5(const std::string& filefullpath)
{
	FILE* f=fopen(filefullpath.c_str(),"rb");
	if(f==NULL)
	{
		return "";
	}

	KMD5 MD5;						//checksum object	
	size_t nLength = 0;				//number of bytes read from the file
	const int nBufferSize = 1024;	//checksum the file in blocks of 1024 bytes
	unsigned char Buffer[nBufferSize];		//buffer for data read from the file

	//checksum the file in blocks of 1024 bytes
	
	while(feof(f))
	{
		nLength=fread((unsigned char*)Buffer,sizeof(unsigned char),nBufferSize,f);
		MD5.Update(Buffer,nLength);
	}
	fclose(f);
	//finalise the checksum and return it
	return MD5.Final();
}

//---------------------------------------------------------------------------------------
std::string KMD5::GetBufferMD5(unsigned char* pBuf, unsigned int nLength)
{
	if(pBuf==NULL || nLength==0)
		return "";

	//calculate and return the checksum
	KMD5 MD5;
	MD5.Update( pBuf, nLength );
	return MD5.Final();
}

//---------------------------------------------------------------------------------------
std::string KMD5::GetStringMD5(const std::string& buf)
{
	unsigned char pBuf[1024];
	unsigned long nLength=(int)buf.length();
	memcpy(pBuf,buf.c_str(),nLength);

	//calculate and return the checksum
	KMD5 MD5;
	MD5.Update( pBuf, nLength );
	return MD5.Final();
}

//---------------------------------------------------------------------------------------
unsigned long KMD5::RotateLeft(unsigned long x, int n)
{
	//check that unsigned long is 4 bytes long - true in Visual C++ 6 and 32 bit Windows
//	assert( sizeof(x) == 4 );

	//rotate and return x
	return (x << n) | (x >> (32-n));
}

//---------------------------------------------------------------------------------------
void KMD5::FF( unsigned long& A, unsigned long B, unsigned long C, unsigned long D, unsigned long X, unsigned long S, unsigned long T)
{
	unsigned long F = (B & C) | (~B & D);
	A += F + X + T;
	A = RotateLeft(A, S);
	A += B;
}

//---------------------------------------------------------------------------------------
void KMD5::GG( unsigned long& A, unsigned long B, unsigned long C, unsigned long D, unsigned long X, unsigned long S, unsigned long T)
{
	unsigned long G = (B & D) | (C & ~D);
	A += G + X + T;
	A = RotateLeft(A, S);
	A += B;
}

//---------------------------------------------------------------------------------------
void KMD5::HH( unsigned long& A, unsigned long B, unsigned long C, unsigned long D, unsigned long X, unsigned long S, unsigned long T)
{
	unsigned long H = (B ^ C ^ D);
	A += H + X + T;
	A = RotateLeft(A, S);
	A += B;
}

//---------------------------------------------------------------------------------------
void KMD5::II( unsigned long& A, unsigned long B, unsigned long C, unsigned long D, unsigned long X, unsigned long S, unsigned long T)
{
	unsigned long I = (C ^ (B | ~D));
	A += I + X + T;
	A = RotateLeft(A, S);
	A += B;
}

//---------------------------------------------------------------------------------------
void KMD5::ByteToDWord(unsigned long* Output, unsigned char* Input, unsigned int nLength)
{
	//entry invariants
	assert( nLength % 4 == 0 );

	//initialisations
	unsigned int i=0;	//index to Output array
	unsigned int j=0;	//index to Input array

	//transfer the data by shifting and copying
	for ( ; j < nLength; i++, j += 4)
	{
		Output[i] = (unsigned long)Input[j]			| 
					(unsigned long)Input[j+1] << 8	| 
					(unsigned long)Input[j+2] << 16 | 
					(unsigned long)Input[j+3] << 24;
	}
}

//---------------------------------------------------------------------------------------
void KMD5::Transform(unsigned char Block[64])
{
	//initialise local data with current checksum
	unsigned long a = m_lMD5[0];
	unsigned long b = m_lMD5[1];
	unsigned long c = m_lMD5[2];
	unsigned long d = m_lMD5[3];

	//copy BYTES from input 'Block' to an array of ULONGS 'X'
	unsigned long X[16];
	ByteToDWord( X, Block, 64 );

	//Perform Round 1 of the transformation
	FF (a, b, c, d, X[ 0], MD5_S11, MD5_T01); 
	FF (d, a, b, c, X[ 1], MD5_S12, MD5_T02); 
	FF (c, d, a, b, X[ 2], MD5_S13, MD5_T03); 
	FF (b, c, d, a, X[ 3], MD5_S14, MD5_T04); 
	FF (a, b, c, d, X[ 4], MD5_S11, MD5_T05); 
	FF (d, a, b, c, X[ 5], MD5_S12, MD5_T06); 
	FF (c, d, a, b, X[ 6], MD5_S13, MD5_T07); 
	FF (b, c, d, a, X[ 7], MD5_S14, MD5_T08); 
	FF (a, b, c, d, X[ 8], MD5_S11, MD5_T09); 
	FF (d, a, b, c, X[ 9], MD5_S12, MD5_T10); 
	FF (c, d, a, b, X[10], MD5_S13, MD5_T11); 
	FF (b, c, d, a, X[11], MD5_S14, MD5_T12); 
	FF (a, b, c, d, X[12], MD5_S11, MD5_T13); 
	FF (d, a, b, c, X[13], MD5_S12, MD5_T14); 
	FF (c, d, a, b, X[14], MD5_S13, MD5_T15); 
	FF (b, c, d, a, X[15], MD5_S14, MD5_T16); 

	//Perform Round 2 of the transformation
	GG (a, b, c, d, X[ 1], MD5_S21, MD5_T17); 
	GG (d, a, b, c, X[ 6], MD5_S22, MD5_T18); 
	GG (c, d, a, b, X[11], MD5_S23, MD5_T19); 
	GG (b, c, d, a, X[ 0], MD5_S24, MD5_T20); 
	GG (a, b, c, d, X[ 5], MD5_S21, MD5_T21); 
	GG (d, a, b, c, X[10], MD5_S22, MD5_T22); 
	GG (c, d, a, b, X[15], MD5_S23, MD5_T23); 
	GG (b, c, d, a, X[ 4], MD5_S24, MD5_T24); 
	GG (a, b, c, d, X[ 9], MD5_S21, MD5_T25); 
	GG (d, a, b, c, X[14], MD5_S22, MD5_T26); 
	GG (c, d, a, b, X[ 3], MD5_S23, MD5_T27); 
	GG (b, c, d, a, X[ 8], MD5_S24, MD5_T28); 
	GG (a, b, c, d, X[13], MD5_S21, MD5_T29); 
	GG (d, a, b, c, X[ 2], MD5_S22, MD5_T30); 
	GG (c, d, a, b, X[ 7], MD5_S23, MD5_T31); 
	GG (b, c, d, a, X[12], MD5_S24, MD5_T32); 

	//Perform Round 3 of the transformation
	HH (a, b, c, d, X[ 5], MD5_S31, MD5_T33); 
	HH (d, a, b, c, X[ 8], MD5_S32, MD5_T34); 
	HH (c, d, a, b, X[11], MD5_S33, MD5_T35); 
	HH (b, c, d, a, X[14], MD5_S34, MD5_T36); 
	HH (a, b, c, d, X[ 1], MD5_S31, MD5_T37); 
	HH (d, a, b, c, X[ 4], MD5_S32, MD5_T38); 
	HH (c, d, a, b, X[ 7], MD5_S33, MD5_T39); 
	HH (b, c, d, a, X[10], MD5_S34, MD5_T40); 
	HH (a, b, c, d, X[13], MD5_S31, MD5_T41); 
	HH (d, a, b, c, X[ 0], MD5_S32, MD5_T42); 
	HH (c, d, a, b, X[ 3], MD5_S33, MD5_T43); 
	HH (b, c, d, a, X[ 6], MD5_S34, MD5_T44); 
	HH (a, b, c, d, X[ 9], MD5_S31, MD5_T45); 
	HH (d, a, b, c, X[12], MD5_S32, MD5_T46); 
	HH (c, d, a, b, X[15], MD5_S33, MD5_T47); 
	HH (b, c, d, a, X[ 2], MD5_S34, MD5_T48); 

	//Perform Round 4 of the transformation
	II (a, b, c, d, X[ 0], MD5_S41, MD5_T49); 
	II (d, a, b, c, X[ 7], MD5_S42, MD5_T50); 
	II (c, d, a, b, X[14], MD5_S43, MD5_T51); 
	II (b, c, d, a, X[ 5], MD5_S44, MD5_T52); 
	II (a, b, c, d, X[12], MD5_S41, MD5_T53); 
	II (d, a, b, c, X[ 3], MD5_S42, MD5_T54); 
	II (c, d, a, b, X[10], MD5_S43, MD5_T55); 
	II (b, c, d, a, X[ 1], MD5_S44, MD5_T56); 
	II (a, b, c, d, X[ 8], MD5_S41, MD5_T57); 
	II (d, a, b, c, X[15], MD5_S42, MD5_T58); 
	II (c, d, a, b, X[ 6], MD5_S43, MD5_T59); 
	II (b, c, d, a, X[13], MD5_S44, MD5_T60); 
	II (a, b, c, d, X[ 4], MD5_S41, MD5_T61); 
	II (d, a, b, c, X[11], MD5_S42, MD5_T62); 
	II (c, d, a, b, X[ 2], MD5_S43, MD5_T63); 
	II (b, c, d, a, X[ 9], MD5_S44, MD5_T64); 

	//add the transformed values to the current checksum
	m_lMD5[0] += a;
	m_lMD5[1] += b;
	m_lMD5[2] += c;
	m_lMD5[3] += d;
}

//---------------------------------------------------------------------------------------
KMD5::KMD5()
{
	// zero members
	memset( m_lpszBuffer, 0, 64 );
	m_nCount[0] = m_nCount[1] = 0;

	// Load magic state initialization constants
	m_lMD5[0] = MD5_INIT_STATE_0;
	m_lMD5[1] = MD5_INIT_STATE_1;
	m_lMD5[2] = MD5_INIT_STATE_2;
	m_lMD5[3] = MD5_INIT_STATE_3;
}

//---------------------------------------------------------------------------------------
void KMD5::DWordToByte(unsigned char* Output, unsigned long* Input, unsigned int nLength )
{
	//entry invariants
	assert( nLength % 4 == 0 );

	//transfer the data by shifting and copying
	unsigned int i = 0;
	unsigned int j = 0;
	for ( ; j < nLength; i++, j += 4) 
	{
		Output[j] =   (unsigned char)(Input[i] & 0xff);
		Output[j+1] = (unsigned char)((Input[i] >> 8) & 0xff);
		Output[j+2] = (unsigned char)((Input[i] >> 16) & 0xff);
		Output[j+3] = (unsigned char)((Input[i] >> 24) & 0xff);
	}
}

//---------------------------------------------------------------------------------------
std::string KMD5::Final()
{
	//Save number of bits
	unsigned char Bits[8];
	DWordToByte( Bits, m_nCount, 8 );

	//Pad out to 56 mod 64.
	unsigned int nIndex = (unsigned int)((m_nCount[0] >> 3) & 0x3f);
	unsigned int nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
	Update( PADDING, nPadLen );

	//Append length (before padding)
	Update( Bits, 8 );

	//Store final state in 'lpszMD5'
	const int nMD5Size = 16;
	unsigned char lpszMD5[ nMD5Size ];
	DWordToByte( lpszMD5, m_lMD5, nMD5Size );

	//Convert the hexadecimal checksum to a CString
	std::string strMD5;
	for ( int i=0; i < nMD5Size; i++) 
	{
		std::string Str;
		if (lpszMD5[i] == 0)
		{
			Str = "00";
		}
		else if (lpszMD5[i] <= 15)
		{
			char pTemp[128];
			sprintf(pTemp,"0%x",lpszMD5[i]);
			Str=pTemp;
		}
		else
		{
			char pTemp[128];
			sprintf(pTemp,"%x",lpszMD5[i]);
			Str=pTemp;
		}

		assert( Str.length() == 2 );
		strMD5 += Str;
	}
	assert( strMD5.length() == 32 );
	return strMD5;
}

//---------------------------------------------------------------------------------------
void KMD5::Update( unsigned char* Input,	unsigned long nInputLen )
{
	//Compute number of bytes mod 64
	unsigned int nIndex = (unsigned int)((m_nCount[0] >> 3) & 0x3F);

	//Update number of bits
	if ( ( m_nCount[0] += nInputLen << 3 )  <  ( nInputLen << 3) )
	{
		m_nCount[1]++;
	}
	m_nCount[1] += (nInputLen >> 29);

	//Transform as many times as possible.
	unsigned int i=0;		
	unsigned int nPartLen = 64 - nIndex;
	if (nInputLen >= nPartLen) 	
	{
		memcpy( &m_lpszBuffer[nIndex], Input, nPartLen );
		Transform( m_lpszBuffer );
		for (i = nPartLen; i + 63 < nInputLen; i += 64) 
		{
			Transform( &Input[i] );
		}
		nIndex = 0;
	} 
	else 
	{
		i = 0;
	}

	// Buffer remaining input
	memcpy( &m_lpszBuffer[nIndex], &Input[i], nInputLen-i);
}
