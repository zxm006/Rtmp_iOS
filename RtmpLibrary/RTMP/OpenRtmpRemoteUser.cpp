#include "OpenRtmpRemoteUser.h"

OpenRtmpRemoteUser::OpenRtmpRemoteUser(PLAYCALLBACK playcallback)
: m_ulPeerVideoID(0)
, m_ulPeerAudioID(0)
, m_uiServerPort(0)
, m_pVideoView(NULL)
, m_pVideoPalyback(NULL)
, m_pAudioplayback(NULL)
, m_IsOpenVideo(false)
, m_IsOpenAudio(false)
, m_IsReceiverVideo(false)
, m_IsReceiverAudio(false)
, m_pAudioPacketBeffer(NULL)
, m_pVideoPacketBeffer(NULL)
, m_pVideoPacketLen(100)
, m_pVideoPacketBefferSeek(0)
, m_iVideoBefferState(0)
, m_iCount(0)
, m_RtmpClient(NULL)
, m_nTimeStamp(0)
, m_pUrl(NULL)
{
    m_pAudioPacketBeffer=(char*)malloc(65);
    memset(m_pAudioPacketBeffer, 0, 65);
    m_pVideoPalyback = CteateVideoplayback();
    m_pAudioplayback= CteateAudioplayback();
    
    m_RtmpClient = new CRtmpClient(this);
    m_pUrl = (char*)malloc(1024);
   m_playcallback= playcallback;
    m_nSpsLen = 0;
    m_pSpsData = NULL;
    m_nPpsLen = 0;
    m_pPpsData = NULL;
}

OpenRtmpRemoteUser::~OpenRtmpRemoteUser()
{
    if(m_pVideoPalyback)
        ReleaseVideoplayback(m_pVideoPalyback);
    m_pVideoPalyback = NULL;
    
    if(m_pAudioPacketBeffer!=NULL)
    {
        free(m_pAudioPacketBeffer);
        m_pAudioPacketBeffer = NULL;
    }
    
    if(m_RtmpClient)
    {
        delete m_RtmpClient;
        m_RtmpClient = NULL;
    }
    
    if(m_pAudioplayback)
        ReleaseAudioplayback();
    m_pAudioplayback = NULL;
    
    m_IsOpenVideo = false;
    m_IsOpenAudio = false;
    if(m_pUrl)
        free(m_pUrl);
}

void OpenRtmpRemoteUser::Rtmp_OpenVideoIni(unsigned int nWidth, unsigned int nHeight, unsigned int nFrameRate)
{
    
}

void OpenRtmpRemoteUser::Rtmp_OpenAudioIni(unsigned int nAudioSampleRate, unsigned int nAudioChannels)
{
    
}

void OpenRtmpRemoteUser::Rtmp_GetVideoData(unsigned char *pData, unsigned int nSize, uint32_t nTimeStamp)
{
  bool bIsSps = false;
    if(pData && nSize > 0 && pData[0] == 0x17 && pData[1] == 0x0 && pData[2] == 0x0 && pData[3] ==0x0 && pData[4] ==0x0)
    {
        if(pData[5] == 0x01 && pData[9] == 0xff)
        {
            bIsSps = true;
            if(m_pPpsData == NULL && m_pSpsData == NULL )
            {
                for(int nCount = 10; nCount < nSize; nCount++)
                {
                    if(pData[nCount] == 0xE1)
                    {
                        nCount++;
                        int nSpsLen = 0;
                        nSpsLen = pData[nCount++]<<8;
                        nSpsLen += pData[nCount++];
                        
                        if(nSpsLen + nCount > nSize)
                            return;
                         
                        m_nSpsLen = nSpsLen;
                        m_pSpsData = (char*)malloc(m_nSpsLen);
                        memcpy(m_pSpsData, &pData[nCount], m_nSpsLen);
                        nCount += (nSpsLen - 1);
                    }
                    else if(pData[nCount] == 0x01)
                    {
                        nCount++;
                        int nPpsLen = 0;
                        nPpsLen = pData[nCount++]<<8;
                        nPpsLen += pData[nCount++];
                        
                        if(nPpsLen + nCount > nSize)
                            return;
                        
                        m_nPpsLen = nPpsLen;
                        m_pPpsData = (char*)malloc(m_nPpsLen);
                        memcpy(m_pPpsData, &pData[nCount], m_nSpsLen);
                        nCount += nPpsLen;
                    }
                }
            }
        }
    }
    
    if(pData &&m_pVideoPalyback && !bIsSps)
    {
        char* pBuff = (char*)malloc(nSize + 1024);
        memset(pBuff, 0, nSize + 1024);
        
        int nNaluSize = nSize - 9 + 4 + m_nSpsLen + 4 + m_nPpsLen;
        int nCount = 0;
        if(pData[0] == 0x17)
        {
            pBuff[nCount++] = 0x17;
            pBuff[nCount++] = 0x01;// AVC NALU
            pBuff[nCount++] = 0x00;
            pBuff[nCount++] = 0x00;
            pBuff[nCount++] = 0x00;
            
            // NALU size
            pBuff[nCount++] = nNaluSize>>24;
            pBuff[nCount++] = nNaluSize>>16;
            pBuff[nCount++] = nNaluSize>>8;
            pBuff[nCount++] = nNaluSize&0xff;
            
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 1;
            memcpy(&pBuff[nCount], m_pSpsData, m_nSpsLen);
            nCount += m_nSpsLen;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 1;
            memcpy(&pBuff[nCount], m_pPpsData, m_nPpsLen);
            nCount += m_nPpsLen;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 1;
            memcpy(&pBuff[nCount], pData + 9, nSize);
            nCount += (nSize - 9);
        }
        else
        {
            pBuff[nCount++] = 0x27;
            pBuff[nCount++] = 0x01;// AVC NALU
            pBuff[nCount++] = 0x00;
            pBuff[nCount++] = 0x00;
            pBuff[nCount++] = 0x00;
            
            // NALU size
            pBuff[nCount++] = nNaluSize>>24;
            pBuff[nCount++] = nNaluSize>>16;
            pBuff[nCount++] = nNaluSize>>8;
            pBuff[nCount++] = nNaluSize&0xff;
            
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 0;
            pBuff[nCount++] = 1;
 
            memcpy(&pBuff[nCount], pData + 9, nSize);
             nCount += (nSize - 9);
 
        }
        if(m_pVideoPalyback)
        m_pVideoPalyback->AddVideoData((unsigned char*)pBuff, nCount,nTimeStamp);
        free(pBuff);
    }
 
}

void OpenRtmpRemoteUser::Rtmp_GetAudioData(unsigned char *pData, unsigned int nSize, uint32_t nTimeStamp)
{
    m_nTimeStamp = nTimeStamp;
    if(m_pAudioplayback)
        m_pAudioplayback->AddAudioData(pData, nSize, nTimeStamp);
}

void OpenRtmpRemoteUser::Rtmp_PlayStop()
{
        dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    this->ReleaseMediaSever();
        });
    if (m_playcallback) {
        m_playcallback("Rtmp_PlayStop");
     
    }
}

double OpenRtmpRemoteUser::GetPlayTime()
{
    if(m_RtmpClient)
        return m_RtmpClient->GetPlayTime();
    return 0;
}

bool OpenRtmpRemoteUser::SetPlaySeek(int nTime)
{
    bool re =  false;
    if(m_RtmpClient && m_pVideoPalyback && m_pAudioplayback)
    {
        re = m_RtmpClient->SetPlaySeek(nTime);
        m_pVideoPalyback->SeekCleanBuffer();
        m_pAudioplayback->SeekCleanBuffer();
        return re;
    }
        
    return false;
}

unsigned int OpenRtmpRemoteUser::GetCurrentPlayTime()
{
    if(m_pVideoPalyback)
        return m_pVideoPalyback->GetTimeStamp();
    return 0;
}

bool OpenRtmpRemoteUser::SetPlayAndStop(bool bPaly)
{
    if(m_RtmpClient && m_pVideoPalyback)
    {
        m_pVideoPalyback->SetPlayAndStop(bPaly);
        return m_RtmpClient->SetPlayAndStop(bPaly);
    }
    
    return false;
}

bool OpenRtmpRemoteUser::ConnectMediaServer(void* pView, const char* czpUrl, int nVideoType)
{
    if(m_RtmpClient && czpUrl)
    {
        strncpy(m_pUrl, czpUrl, strlen(czpUrl));
        if(m_RtmpClient->ConnectServer(czpUrl, true))
        {
            OpenVideo(pView, nVideoType);
            StartDisplayVideo();
            OpenAudio(6, nVideoType);
            return true;
        }
        return false;
    }
    
    return false;
}

void OpenRtmpRemoteUser::ReleaseMediaSever()
{
   if(m_RtmpClient)
    {
  
         StopDisplayVideo();
      m_RtmpClient->DisconnectServer();
        CloseAudio();
       
        CloseVideo();
    
        m_RtmpClient=NULL;
    }
}

bool OpenRtmpRemoteUser::OpenVideo(void* pView, int nVideoType, unsigned int uiWidth, unsigned int uiHeight)
{
    if(m_pVideoPalyback == NULL)
        return false;
     m_IsOpenVideo = true;
    m_pVideoPalyback->SetVideoWindow(pView, nVideoType);
 
    m_pVideoPalyback->Start();
    return true;
}

void OpenRtmpRemoteUser::reSetVidoeFrame()
{
    if(m_pVideoPalyback == NULL)
        return ;
    m_pVideoPalyback ->reSetVidoeFrame();
}


void OpenRtmpRemoteUser::CloseVideo()
{
    m_IsOpenVideo = false;
    if(m_pVideoPalyback)
        m_pVideoPalyback->Stop();
}

bool OpenRtmpRemoteUser::OpenAudio(int type, int nVideoType)
{
   m_IsOpenAudio = true;
    return true;
}

void OpenRtmpRemoteUser::CloseAudio()
{
    m_IsOpenAudio = false;

    if(m_pAudioplayback)
        m_pAudioplayback->Stop();
}

bool OpenRtmpRemoteUser::StartDisplayVideo()
{
    if(m_pVideoPalyback == NULL)
        return false;
    m_IsReceiverVideo = true;
    return m_pVideoPalyback->StartDisplayVideo();
}

void OpenRtmpRemoteUser::StopDisplayVideo()
{
    if(m_pVideoPalyback == NULL)
        return ;
    m_IsReceiverVideo = false;
    m_pVideoPalyback->StopDisplayVideo();
}

void OpenRtmpRemoteUser::OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen)
{
    
}

void OpenRtmpRemoteUser::OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen)
{
#ifdef USER_SUBCONTRACTING
    if(pData != NULL && nLen>16 && m_IsReceiverVideo)
    {
        int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pData);
        int nSubHeaderLen=VIDEC_KEY_SUBCONTRACTING_GET_LEN(pData+nHeaderLen);
        
        unsigned short isBeging = VIDEC_KEY_SUBCONTRACTING_GET_BEGING(pData+nHeaderLen);
        unsigned short isEnd = VIDEC_KEY_SUBCONTRACTING_GET_END(pData+nHeaderLen);
        
        if(isBeging == 1 && isEnd == 1)
        {
            if(m_pVideoPalyback)
                m_pVideoPalyback->AddVideoData(pData+(nHeaderLen+nSubHeaderLen), nLen-(nHeaderLen+nSubHeaderLen));
            return;
        }
        
        if(isBeging == 1 && isEnd == 0 && m_iVideoBefferState == 0)
        {
            memcpy(m_pVideoPacketBeffer + m_pVideoPacketBefferSeek, pData+(nHeaderLen+nSubHeaderLen), nLen-(nHeaderLen+nSubHeaderLen));
            m_pVideoPacketBefferSeek += (nLen-(nHeaderLen+nSubHeaderLen));
            m_iVideoBefferState = 1;
        }
        else if(isBeging == 0 && isEnd == 0 && m_iVideoBefferState == 1)
        {
            memcpy(m_pVideoPacketBeffer + m_pVideoPacketBefferSeek, pData+(nHeaderLen+nSubHeaderLen), nLen-(nHeaderLen+nSubHeaderLen));
            m_pVideoPacketBefferSeek += (nLen-(nHeaderLen+nSubHeaderLen));
        }
        else if(isBeging == 0 && isEnd == 1 && m_iVideoBefferState == 1)
        {
            memcpy(m_pVideoPacketBeffer + m_pVideoPacketBefferSeek, pData+(nHeaderLen+nSubHeaderLen), nLen-(nHeaderLen+nSubHeaderLen));
            if(m_pVideoPalyback)
                m_pVideoPalyback->AddVideoData((unsigned char*)m_pVideoPacketBeffer, m_pVideoPacketBefferSeek);
            m_pVideoPacketBefferSeek = 0;
            m_iVideoBefferState = 0;
        }
        else
            assert(0);
    }
#else
    
#endif
}
