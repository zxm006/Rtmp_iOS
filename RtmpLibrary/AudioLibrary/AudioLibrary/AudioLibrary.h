//
//  AudioLibrary.h
//  AudioLibrary
//
//  Created by wang guijun on 13-1-19.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//
#ifndef AUDIOLIBRARY_H
#define AUDIOLIBRARY_H
#include <list>
class AudioCapture
{
public:
    AudioCapture(){};
    virtual ~AudioCapture(){};
public:
    virtual bool Start() = 0;
    virtual void Stop() = 0;
public:
    virtual void SetAudioEncodingMode(int type) = 0;
};

class AudioCaptureDataCallBack
{
public:
    AudioCaptureDataCallBack(){};
    virtual ~AudioCaptureDataCallBack(){};
public:
    virtual void On_MediaReceiverCallbackAudio(unsigned char*pData,int nLen) = 0;
};

AudioCapture* CteateAudioCapture(AudioCaptureDataCallBack* pVideoCaptureDataCallBack);
void ReleaseAudioCapture();


class Audioplayback
{
public:
    Audioplayback(){};
    virtual ~Audioplayback(){};
public:
    virtual bool Start() = 0;
    virtual void Stop() = 0; 
    virtual bool SeekCleanBuffer() = 0;
public:
    virtual bool AddAudioData(unsigned char*pData, int nLen, uint32_t nTimeStamp) = 0;
};

Audioplayback* CteateAudioplayback();
void ReleaseAudioplayback();

#endif

