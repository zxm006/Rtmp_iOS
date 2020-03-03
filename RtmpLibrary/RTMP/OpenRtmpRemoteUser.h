

#ifndef __VideoSession__OpenRtmpRemoteUser__
#define __VideoSession__OpenRtmpRemoteUser__

#include <iostream>
#include "AudioLibrary.h"
#include "VideoLibrary.h"
#include "uuRtmpClient.h"
typedef int (*PLAYCALLBACK)(const char *info);

class OpenRtmpRemoteUser
: public CRtmpPlayCall
{
public:
    OpenRtmpRemoteUser(PLAYCALLBACK playcallback);
    ~OpenRtmpRemoteUser();
public:
    bool ConnectMediaServer(void* pView, const char* czpUrl, int nVideoType);
    void ReleaseMediaSever();
    
    double GetPlayTime();
    bool SetPlaySeek(int iTime);
    bool SetPlayAndStop(bool bPaly);
    unsigned int GetCurrentPlayTime();
private:
    bool OpenVideo(void* pView, int nVideoType, unsigned int uiWidth = 640, unsigned int uiHeight = 480);
    void CloseVideo();
    bool StartDisplayVideo();
    void StopDisplayVideo();
    
    bool OpenAudio(int type, int nVideoType);
    void CloseAudio();
public:
    bool IsOpenVideo() const{return m_IsOpenVideo;};
    bool IsOpenAudio() const{return m_IsOpenAudio;};
    
    bool IsStartReceiverVideo() const{return m_IsReceiverVideo;};
    bool IsStartReceiverAudio() const{return m_IsReceiverAudio;};
    
    virtual void reSetVidoeFrame();
    
public:
	virtual void OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen);
	virtual void OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen);
    
    virtual void Rtmp_OpenVideoIni(unsigned int nWidth, unsigned int nHeight, unsigned int nFrameRate);
    virtual void Rtmp_OpenAudioIni(unsigned int nAudioSampleRate, unsigned int nAudioChannels);
    virtual void Rtmp_GetVideoData(unsigned char *pData, unsigned int nSize, uint32_t nTimeStamp);
    virtual void Rtmp_GetAudioData(unsigned char *pData, unsigned int nSize, uint32_t nTimeStamp);
    virtual void Rtmp_PlayStop();
private:
    PLAYCALLBACK m_playcallback;
    VideoPalyback*          m_pVideoPalyback;
    CRtmpClient*            m_RtmpClient;
    char*                   m_pAudioPacketBeffer;
    char*                   m_pVideoPacketBeffer;
    unsigned long           m_pVideoPacketLen;
    unsigned long           m_pVideoPacketBefferSeek;
    unsigned int            m_iVideoBefferState;
    unsigned int            m_iCount;
    Audioplayback*          m_pAudioplayback;
	unsigned long           m_ulPeerVideoID ;			//VideoID
	unsigned long           m_ulPeerAudioID ;			//AudioID
private:
    unsigned int            m_uiServerPort;
    void*                   m_pVideoView;
private:
    bool                    m_IsOpenVideo;
    bool                    m_IsOpenAudio;
    bool                    m_IsReceiverVideo;
    bool                    m_IsReceiverAudio;
    uint32_t                m_nTimeStamp;
    uint32_t                m_nPlaySeekTime;
    char*                   m_pUrl;
    
   int  m_nSpsLen  ;
    char*  m_pSpsData  ;
    int m_nPpsLen  ;
    char*  m_pPpsData  ;
};

#endif /* defined(__VideoSession__OpenRtmpRemoteUser__) */
