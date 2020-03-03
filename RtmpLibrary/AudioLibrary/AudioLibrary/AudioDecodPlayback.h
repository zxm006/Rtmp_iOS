//
//  AudioDecodPlayback.h
//  AudioLibrary
//
//  Created by wang guijun on 13-1-20.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#ifndef AudioLibrary_AudioDecodPlayback_h
#define AudioLibrary_AudioDecodPlayback_h

#include "AudioLibrary.h"
#include "AudioDecodes.h"
#include "AudioPlay.h"
#include <list>
#include <pthread/pthread.h>
#include "CritSec.h"
typedef struct {
    uint32_t nPlayTimeStamp;
    uint32_t nCurrentTime;
} TimeStampStruct;

class KBuffer
{
public:
    KBuffer():m_cpBuffer(NULL),m_ulLen(0)
    {}
public:
    void SetBuffer( char* cpBuffer){ m_cpBuffer = cpBuffer;};
    void SetLen(const unsigned long ulLen){ m_ulLen = ulLen;};
    void SetTimeStamp(uint32_t nTimeStamp){ m_nTimeStamp = nTimeStamp;}
    
    char* GetBuffer() const { return m_cpBuffer;};
    const unsigned long GetLen() const { return m_ulLen;};
    uint32_t GetTimeStamp() const { return m_nTimeStamp;};
private:
    char* m_cpBuffer;
    unsigned long m_ulLen;
    uint32_t m_nTimeStamp;
};

typedef std::list<KBuffer*> AUDIODDATA_LIST;

class AudioDecodPlayback : public Audioplayback
{
public:
    AudioDecodPlayback();
    ~AudioDecodPlayback();
public:
    virtual bool Start();
    virtual void Stop();
//    virtual void SetMute();
    virtual bool SeekCleanBuffer();
public:
//    virtual void SetAudioDecodingMode(int type, int nVideoType);
    virtual bool AddAudioData(unsigned char*pData,int nLen, uint32_t nTimeStamp);
public:
    bool GetAudioData(char* &cBuffer, unsigned long &ulLen);
private:
//    pthread_mutex_t m_pMutex;
    AUDIODDATA_LIST m_audioData;
    bool  m_IsSetMute;
    int   m_nVideoType;
    uint32_t                m_nTimeStamp;
    uint32_t                m_nBuffertime;
    TimeStampStruct         m_nTimeStruct;
    AudioDecodes *audioDecode;
    AudioPlay * m_AudioPlay ;
        KCritSec m_mKCritSec;
};

#endif
