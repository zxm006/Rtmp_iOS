//
//  AudioLibrary.m
//  AudioLibrary
//
//  Created by wang guijun on 13-1-19.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//
#include "AudioLibrary.h"
#include "AudioEncodeCapture.h"
#include "AudioDecodPlayback.h"

static int g_iCount = 0;
AudioEncodeCapture* g_pAudioEncodeCapture = NULL;
AudioDecodPlayback* g_pAudioDecodPlayback = NULL;


AudioCapture* CteateAudioCapture(AudioCaptureDataCallBack* pVideoCaptureDataCallBack)
{
    if(g_pAudioEncodeCapture == NULL)
    {
        g_pAudioEncodeCapture = new AudioEncodeCapture(pVideoCaptureDataCallBack);
    }
    
    return g_pAudioEncodeCapture;
}

void ReleaseAudioCapture()
{
    if(g_pAudioEncodeCapture != NULL)
        delete g_pAudioEncodeCapture;
    g_pAudioEncodeCapture = NULL;
}

Audioplayback* CteateAudioplayback()
{
    if(g_pAudioDecodPlayback == NULL)
    {
        g_pAudioDecodPlayback = new AudioDecodPlayback;
        g_pAudioDecodPlayback->Start();
    }
    g_iCount++;
    return g_pAudioDecodPlayback;
}

void ReleaseAudioplayback()
{
    if(g_iCount == 1)
    {
        if(g_pAudioDecodPlayback != NULL)
        {
            g_pAudioDecodPlayback->Stop();
            delete g_pAudioDecodPlayback;
        }
        g_pAudioDecodPlayback = NULL;
        g_iCount = 0;
    }
    else
        g_iCount--;
}