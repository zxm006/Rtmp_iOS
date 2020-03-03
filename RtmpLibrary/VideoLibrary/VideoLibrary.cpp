//
//  VideoLibrary.m
//  VideoLibrary
//
//  Created by wang guijun on 13-1-19.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#include "VideoLibrary.h"
#include "VideoCaptureEncoding.h"
#include "CapScreenEnc.h"
#include "VideoDecodingDisplay.h"
#import <UIKit/UIKit.h>
VideoCapture* g_VideoCapture = NULL;

VideoCapture* CteateVideoCapture(VideoCaptureDataCallBack* pVideoCaptureDataCallBack)
{
    if(g_VideoCapture == NULL)
    {
        g_VideoCapture = new VideoCaptureEncod(pVideoCaptureDataCallBack);
        return g_VideoCapture;
    }
    else
        return g_VideoCapture;
}
 
void ReleaseVideoCapture()
{
    if(g_VideoCapture != NULL)
    {
        delete g_VideoCapture;
        g_VideoCapture = NULL;
    }
}

VideoCapScreen* g_VideoCapScreenEnc= NULL;

VideoCapScreen* CreateCapScreen(VideoCaptureDataCallBack* pVideoCaptureDataCallBack)
{
    if(g_VideoCapScreenEnc == NULL)
    {
        g_VideoCapScreenEnc = new VideoCapScreenEnc(pVideoCaptureDataCallBack);
        return g_VideoCapScreenEnc;
    }
    else
        return g_VideoCapScreenEnc;
}



void ReleaseCapScreen()
{
    if(g_VideoCapScreenEnc != NULL)
    {
        g_VideoCapScreenEnc->StopCapture();
        
        delete g_VideoCapScreenEnc;
        g_VideoCapScreenEnc = NULL;
    }
}


VideoPalyback* CteateVideoplayback()
{
    VideoDecodingDisplay* pVideoDecodingDisplay = new VideoDecodingDisplay;
    return pVideoDecodingDisplay;
}

void ReleaseVideoplayback(VideoPalyback* pVideoPalyback)
{
    if(pVideoPalyback)
    {
        VideoDecodingDisplay* pVideoDecodingDisplay = (VideoDecodingDisplay*)pVideoPalyback;
        delete pVideoDecodingDisplay;
    }
}

