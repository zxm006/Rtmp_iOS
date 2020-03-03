//
//  AudioEncodeCapture.cpp
//  AudioLibrary
//
//  Created by wang guijun on 13-1-20.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#include "AudioEncodeCapture.h"
#import "RecordAndSend.h"
RecordAndSend *recordAndSend=nil ;
 AudioCaptureDataCallBack* m_pAudioCaptureDataCallBack;

AudioEncodeCapture::AudioEncodeCapture(AudioCaptureDataCallBack* pAudioCaptureDataCallBack)
:m_pAudioCaptureDataCallBack(pAudioCaptureDataCallBack)
{
    recordAndSend=[[RecordAndSend alloc]init];
    [recordAndSend setRecordCallBack:pAudioCaptureDataCallBack];
}

AudioEncodeCapture::~AudioEncodeCapture()
{
//    [recordAndSend release];
    recordAndSend =nil;
}

bool AudioEncodeCapture::Start()
{
    [recordAndSend StartRecord];
    return true;
}

void AudioEncodeCapture::Stop()
{
    [recordAndSend StopRecord];
}

void AudioEncodeCapture::SetAudioEncodingMode(int type)
{
    recordAndSend.codetype=(CodeType)type;
}