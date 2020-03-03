//
//  VideoDecodingDisplay.h
//  VideoLibrary
//
//  Created by wang guijun on 13-1-20.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#ifndef VideoLibrary_VideoDecodingDisplay_h
#define VideoLibrary_VideoDecodingDisplay_h

#include "liveFFmpegdecode.h"
#import "VideoLibrary.h"
#define AFX_EXT_CLASS
#include "Thread.h"
#include <list>
#include "CritSec.h"
extern uint32_t g_nTimeStamp;

typedef struct {
    uint32_t nPlayTimeStamp;
    unsigned long nCurrentTime;
} TimeStampStruct;

class KVideoBuffer
{
public:
    KVideoBuffer():m_cpBuffer(NULL),m_ulLen(0)
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

typedef std::list<KVideoBuffer*> VIDEODDATA_LIST;

class VideoDecodingDisplay : public VideoPalyback , public KThread
{
public:
    VideoDecodingDisplay();
    ~VideoDecodingDisplay();
public:
 
public:
    virtual bool Start();
    virtual void Stop();
public:
    virtual bool StartDisplayVideo();
    virtual void StopDisplayVideo();
    virtual uint32_t GetTimeStamp();
    virtual bool SeekCleanBuffer();
    virtual bool SetPlayAndStop(bool bPaly);
    virtual void  reSetVidoeFrame();
public:
    virtual void SetVideoWindow(void* pVideoWindow, int nVideoType);
    virtual bool AddVideoData(unsigned char*pData,int nLen, uint32_t nTimeStamp);
protected:
    virtual void ThreadProcMain(void);
private:
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    liveFFmpegdecode*       m_liveFFmpegdecode;
    VIDEODDATA_LIST         m_VideoData_List;
    pthread_mutex_t         m_pMutex;
    bool                    m_bWantToStop;
    bool                    m_bThreadEnd;
    size_t                  m_icount;
    uint32_t                m_nBegintime;
    TimeStampStruct         m_nTimeStruct;
    bool                    m_bPalyVideo;
    int                     m_nVideoType;
    KCritSec m_mKCritSec;
};


#endif
