#ifndef _UU_RTMP_CLIENT_H_
#define _UU_RTMP_CLIENT_H_

class RTMP;
#include "CritSec.h"
#include "Thread.h"
#include <string.h>

#define BOOL int

typedef struct _NaluUnit
{
	int type;
	int size;
	unsigned char *data;
}NaluUnit;

typedef struct _RTMPMetadata
{
	// video, must be h264 type
	unsigned int nWidth;
	unsigned int nHeight;
	unsigned int nFrameRate; // fps
	unsigned int nVideoDataRate; // bps
	unsigned int nSpsLen;
	unsigned char Sps[1024];
	unsigned int nPpsLen;
	unsigned char Pps[1024];

	// audio, must be aac type
	bool bHasAudio;
	unsigned int nAudioDatarate;
	unsigned int nAudioSampleRate;
	unsigned int nAudioSampleSize;
	int          nAudioFmt;
	unsigned int nAudioChannels;
	char pAudioSpecCfg;
	unsigned int nAudioSpecCfgLen;

} RTMPMetadata,*LPRTMPMetadata;

class CRtmpPlayCall
{
public:
    CRtmpPlayCall(){};
    virtual ~CRtmpPlayCall(){};
public:
    virtual void Rtmp_OpenVideoIni(unsigned int nWidth, unsigned int nHeight, unsigned int nFrameRate) = 0;
    virtual void Rtmp_OpenAudioIni(unsigned int nAudioSampleRate, unsigned int nAudioChannels) = 0;
public:
    virtual void Rtmp_GetVideoData(unsigned char *pData, unsigned int nSize, unsigned int nTimeStamp) = 0;
    virtual void Rtmp_GetAudioData(unsigned char *pData, unsigned int nSize, unsigned int nTimeStamp) = 0;
public:
    virtual void Rtmp_PlayStop() = 0;
};

class CRtmpClient : public KThread
{
public:
    CRtmpClient(CRtmpPlayCall* pRtmpPlayCall = NULL);
    virtual ~CRtmpClient();
public:
    BOOL ConnectServer(const char* url, bool bIsPlay = false);
    void DisconnectServer();
   
    BOOL SendH264VideoPacket(unsigned char *pData, unsigned int nSize, unsigned int nWidth, unsigned int nHeight, unsigned int nFrameRate, bool bIsKeyFrame, unsigned int nTimeStamp);
    BOOL SendH264AudioPacket(unsigned char *pData, unsigned int nSize, unsigned int nTimeStamp);
    bool ReadOneNaluFromBuf(NaluUnit &nalu);
    double GetPlayTime();
    BOOL SetPlayAndStop(bool bPaly);
    BOOL SetPlaySeek(int nTime);
private:
    virtual void ThreadProcMain(void);
    BOOL InitSockets();
    void CleanupSockets();
    int  SendPacket(unsigned int nPacketType,unsigned char *pData,long nSize,unsigned int nTimestamp);
    BOOL SendH264Metadata(LPRTMPMetadata lpMetaData);
        BOOL getH264Metadata(LPRTMPMetadata lpMetaData);
    BOOL SendHAudioMetadata(LPRTMPMetadata lpMetaData);
    BOOL GetNaluInfo(NaluUnit &nalu , unsigned int nSize, unsigned char *pData);
private:

    RTMP* m_pRtmp;
    bool  m_isSendSpsInfo;
    KCritSec   m_mKCritSec;
    bool  m_isSendSpecInfo;
    bool  m_bIsPlay;
    bool  m_ThreadRun;
    bool  m_bIsStop;
    int m_spslen;
    char m_sps[1024];
    int m_ppslen;
    char m_pps[1024];
    int m_spsPpslen;
    char m_spsPps[1024];
    CRtmpPlayCall* m_pRtmpPlayCall;
};

#endif
