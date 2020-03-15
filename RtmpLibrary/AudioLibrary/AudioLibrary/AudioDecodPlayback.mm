#include "AudioDecodPlayback.h"
#include "AutoLock.h"

extern uint32_t g_nTimeStamp;

AudioDecodPlayback::AudioDecodPlayback()
{
    m_AudioPlay = nil ;
    audioDecode=new AudioDecodes;
    audioDecode->initAudioDecode( AAC_CODE);
    m_IsSetMute = false;
}

AudioDecodPlayback::~AudioDecodPlayback()
{
    if(audioDecode)
    {
        delete audioDecode;
        audioDecode=NULL;
    }
    m_IsSetMute = false;
}

bool AudioDecodPlayback::Start()
{
   KAutoLock lock(m_mKCritSec);
    if(m_AudioPlay)
    {
        [m_AudioPlay cleanUpOpenAL];
        [m_AudioPlay release];
        m_AudioPlay=nil;
    }
    
    m_AudioPlay=[[AudioPlay alloc]init];
    [m_AudioPlay initOpenAL];
    
     [m_AudioPlay LoudSpeaker:true];
  
    return true;
}

void AudioDecodPlayback::Stop()
{
//   KAutoLock lock(m_mKCritSec);
  
 if(m_AudioPlay)
    {
//        [m_AudioPlay cleanUpOpenAL];
        [m_AudioPlay release];
        m_AudioPlay=nil;
    }

 
}


bool AudioDecodPlayback::SeekCleanBuffer()
{
 

    return true;
}

bool AudioDecodPlayback::AddAudioData(unsigned char*pData,int nLen, uint32_t nTimeStamp)
{
     KAutoLock lock(m_mKCritSec);
    
    if(pData==NULL || nLen<=100 || m_IsSetMute)
    {
        return false;
    }
  
 
     char pDstData[4096];
    memset(pDstData,0,4096);
  
   unsigned int sum=0;
    audioDecode->DecodeAudio((uint8_t *)pData,nLen,pDstData,&sum);
//    
     if(m_AudioPlay)
      {
       [m_AudioPlay openAudioFromQueue: (unsigned char *)pDstData dataSize: sum];
    }
 
 
 

    return true;
}

