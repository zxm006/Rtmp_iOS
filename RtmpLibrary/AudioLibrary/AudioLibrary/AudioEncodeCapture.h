//
//  AudioEncodeCapture.h
//  AudioLibrary
//
//  Created by wang guijun on 13-1-20.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#ifndef __AudioLibrary__AudioEncodeCapture__
#define __AudioLibrary__AudioEncodeCapture__
#include <iostream>
#include "AudioLibrary.h"

class AudioEncodeCapture : public AudioCapture
{
public:
    AudioEncodeCapture(AudioCaptureDataCallBack* pAudioCaptureDataCallBack);
    ~AudioEncodeCapture();
public:
    virtual bool Start();
    virtual void Stop();
public:
    virtual void SetAudioEncodingMode(int type);
private:
    AudioCaptureDataCallBack* m_pAudioCaptureDataCallBack;
};

#endif /* defined(__AudioLibrary__AudioEncodeCapture__) */
