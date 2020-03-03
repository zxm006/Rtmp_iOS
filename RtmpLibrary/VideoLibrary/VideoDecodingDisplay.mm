//
//  VideoDecodingDisplay.cpp
//  VideoLibrary
//
//  Created by wang guijun on 13-1-20.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#include "VideoDecodingDisplay.h"
#include <pthread.h>
#include <sys/time.h>
#include "AutoLock.h"

unsigned long XGetTimestampDIS(void);

unsigned long XGetTimestampDIS(void)
{
#ifdef WIN32
    return timeGetTime();
#else
    struct timeval now;
    gettimeofday(&now,NULL);
    return now.tv_sec*1000+now.tv_usec/1000;
#endif
}


uint32_t g_nTimeStamp = 0;

VideoDecodingDisplay::VideoDecodingDisplay()
:m_liveFFmpegdecode(nil)
,m_bThreadEnd(true)
,m_bPalyVideo(true)
{
    
    m_VideoData_List.clear();
    m_bWantToStop = true;
    m_liveFFmpegdecode = [[liveFFmpegdecode alloc] init];
//    m_nBegintime = 0;
    g_nTimeStamp = 0;
}

VideoDecodingDisplay::~VideoDecodingDisplay()
{
    
    if(m_bWantToStop == false)
    {
        
        m_bWantToStop = true ;
        
        //  KAutoLock lock(m_mKCritSec);
        
        VIDEODDATA_LIST::iterator iter = m_VideoData_List.begin();
        while (iter!=m_VideoData_List.end()) {
            free((*iter)->GetBuffer());
            delete *iter;
            iter++;
        }
        m_VideoData_List.clear();
        
        WaitForStop();
    }
    
    
    m_liveFFmpegdecode = nil;
}

bool VideoDecodingDisplay::Start()
{
    
    if(m_liveFFmpegdecode == nil)
        return false;
    m_icount = 0;
    m_bWantToStop = false;
    g_nTimeStamp = 0;
    m_nTimeStruct.nCurrentTime = 0;
    m_nTimeStruct.nPlayTimeStamp = 0;
    m_VideoData_List.clear();
    
    [m_liveFFmpegdecode Beginmpeg_decode_h264];
    
    StartThread();
    
    return true;
}

void VideoDecodingDisplay::Stop()
{
    //
    g_nTimeStamp = 0;
    m_bWantToStop = true;
    
    usleep(10000);
    if(m_liveFFmpegdecode == nil)
        return ;
    
    KAutoLock lock(m_mKCritSec);
    VIDEODDATA_LIST::iterator iter = m_VideoData_List.begin();
    while (iter!=m_VideoData_List.end()) {
        
        free((*iter)->GetBuffer());
        delete *iter;
        iter++;
    }
    m_VideoData_List.clear();
    
    
    [m_liveFFmpegdecode Endinmpeg_decode_h264];
    
    WaitForStop();
    
    
}

bool VideoDecodingDisplay::StartDisplayVideo()
{
    //       KAutoLock lock(m_mKCritSec);
    
    if(m_liveFFmpegdecode == nil)
        return false;
    return [m_liveFFmpegdecode StartDisplayVideo];
}

void VideoDecodingDisplay::StopDisplayVideo()
{
    if(m_liveFFmpegdecode == nil)
        return ;
    [m_liveFFmpegdecode StopDisplayVideo];
}

void VideoDecodingDisplay::SetVideoWindow(void* pVideoWindow, int nVideoType)
{
    if(m_liveFFmpegdecode == nil)
        return ;
    m_nVideoType = nVideoType;
    
    [m_liveFFmpegdecode SetLocalVideoWindow:(__bridge UIView*)pVideoWindow];
    
}

void VideoDecodingDisplay::reSetVidoeFrame()
{
        [m_liveFFmpegdecode reSetVidoeFrame];
}


bool VideoDecodingDisplay::SeekCleanBuffer()
{
    
    VIDEODDATA_LIST::iterator iter = m_VideoData_List.begin();
    while (iter!=m_VideoData_List.end()) {
        
        free((*iter)->GetBuffer());
        delete *iter;
        iter++;
    }
    m_VideoData_List.clear();
//    m_nBegintime = 0;
    m_nTimeStruct.nPlayTimeStamp = 0;
    m_nTimeStruct.nCurrentTime = 0;
    return true;
}

bool VideoDecodingDisplay::AddVideoData(unsigned char*pData,int nLen, uint32_t nTimeStamp)
{
    
    if(m_liveFFmpegdecode == nil)
    {
        
        return false;
    }
    if(m_bWantToStop)
    {
        return true;
        
    }
    //    [m_liveFFmpegdecode ffmpeg_decode_h264:(unsigned char *)pData Len:(int)nLen iCount:1];
    
    char* pVideoPacketBeffer=(char*)malloc(nLen);
    memset(pVideoPacketBeffer, 0, nLen);
    memcpy(pVideoPacketBeffer,pData,nLen);
    
    KVideoBuffer *pKBuffer = new KVideoBuffer;
    pKBuffer->SetBuffer( pVideoPacketBeffer );
    pKBuffer->SetLen( nLen );
    pKBuffer->SetTimeStamp(nTimeStamp);
    
    KAutoLock lock(m_mKCritSec);
    
    m_VideoData_List.push_back(pKBuffer);
    //
    
    return true;
}

uint32_t VideoDecodingDisplay::GetTimeStamp()
{
    return g_nTimeStamp;
}

bool VideoDecodingDisplay::SetPlayAndStop(bool bPaly)
{
    m_bPalyVideo = bPaly;
    return true;
}

void VideoDecodingDisplay::ThreadProcMain(void)
{
    m_bThreadEnd = false;
//    m_nBegintime =(uint32_t)XGetTimestampDIS();
    
    sleep(1);
    
    while(!m_bWantToStop)
    {
        bool bGetVdata = false;
        KVideoBuffer* pRecvBuffer = NULL;
        
        {
            KAutoLock lock(m_mKCritSec);
            size_t icount = m_VideoData_List.size();
            if( m_bPalyVideo&&icount>0)
            {
                pRecvBuffer = (KVideoBuffer*)m_VideoData_List.front();
                bGetVdata =true;
                m_VideoData_List.pop_front();
            }
        }
        
        if(pRecvBuffer != NULL && bGetVdata)
        {
            char* pBuffer = pRecvBuffer->GetBuffer();
            unsigned long uiLen = pRecvBuffer->GetLen();
//            NSLog(@"XGetTimestampDIS() = %ld",XGetTimestampDIS());
            
            [m_liveFFmpegdecode ffmpeg_decode_h264:(unsigned char *)pBuffer Len:(int)uiLen iCount:1];
            free(pBuffer);
            delete pRecvBuffer;
//            uint32_t     ticktime =XGetTimestampDIS() - m_nBegintime;
            
            size_t isize = m_VideoData_List.size();
        
            if(isize>80)
            {
                usleep(500);
            }
            else if(isize>40)
            {
                  usleep(5000);
            }
            else if(isize>20)
            {
                usleep(10000);
            }
             else if(isize>10)
             {
                usleep(15000);
             }
            else
            {
                   usleep(25000);
            }
      }
        else
        {
//            NSLog(@"暂时无数据");
            usleep(500);
        }
        
        
    }
    m_bThreadEnd = true;
}
