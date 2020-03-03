//
//  VideoLibrary.h
//  VideoLibrary
//
//  Created by wang guijun on 13-1-19.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//
#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H
#import <UIKit/UIKit.h>
class VideoCapture
{
public:
    VideoCapture(){};
    virtual ~VideoCapture(){};
public:
    virtual bool StartCapture(UIView *pUIImageView,int resType,bool beautifyace) = 0;
    virtual void StopCapture() = 0;
    virtual bool setPreview(UIView *preview)=0;
    virtual void closeBeautify() = 0;
   virtual void openBeautify() = 0;
    virtual void  reOrientation()= 0;
    virtual void  switchCamera() = 0;
public:
    virtual bool StartEncoding() = 0;
    virtual void StopEncoding() = 0;
    virtual bool StartEncodingData() = 0;
    virtual void StopEncodeingData() = 0;
public:
    virtual void SetFrontAndRearCamera(bool isFront = true) = 0;
    virtual void SetLocalVideoWindow(UIView* pVideoWindow) = 0;
//    virtual void SetPause(bool isPause = true) = 0;
public:
    virtual void SetBitRate(unsigned int uiBitRate) = 0;
    virtual void SetResolution(unsigned int uiWidth, unsigned int uiHeight) = 0;
    
    virtual unsigned int GetBitRate() = 0;
    virtual void GetResolution(unsigned int& uiWidth, unsigned int& uiHeight) = 0;
    
    virtual void SetVideoFps(unsigned int uiFps) = 0;
    virtual unsigned int GetVideoFps() = 0;
};

class VideoCapScreen
{
public:
    VideoCapScreen(){};
    virtual ~VideoCapScreen(){};
public:
    virtual bool StartCapture(int resType) = 0;
    virtual void StopCapture() = 0;
    
//public:
//    virtual bool StartEncoding() = 0;
//    virtual void StopEncoding() = 0;
//    virtual bool StartEncodingData() = 0;
//    virtual void StopEncodeingData() = 0;
//    
//    virtual void SetPause(bool isPause = true) = 0;
//public:
//    virtual void SetBitRate(unsigned int uiBitRate) = 0;
    virtual void SetResolution(unsigned int uiWidth, unsigned int uiHeight) = 0;
//    
//    virtual unsigned int GetBitRate() = 0;
//    virtual void GetResolution(unsigned int& uiWidth, unsigned int& uiHeight) = 0;
//    
//    virtual void SetVideoFps(unsigned int uiFps) = 0;
//    virtual unsigned int GetVideoFps() = 0;
};



class VideoCaptureDataCallBack
{
public:
    VideoCaptureDataCallBack(){};
    ~VideoCaptureDataCallBack(){};
public:
    virtual void On_MediaReceiverCallbackVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight)=0;
};

VideoCapture* CteateVideoCapture(VideoCaptureDataCallBack* pVideoCaptureDataCallBack);
void ReleaseVideoCapture();

VideoCapScreen* CreateCapScreen(VideoCaptureDataCallBack* pVideoCaptureDataCallBack);
void ReleaseCapScreen();

class VideoPalyback
{
public:
    VideoPalyback(){};
    virtual ~VideoPalyback(){};
public:
    virtual bool Start() = 0;
    virtual void Stop() = 0;
public:
    virtual bool StartDisplayVideo() = 0;
    virtual void StopDisplayVideo() = 0;
public:
    virtual void reSetVidoeFrame() = 0;
    virtual uint32_t GetTimeStamp() = 0;
    virtual bool SeekCleanBuffer() = 0;
    virtual bool SetPlayAndStop(bool bPaly) = 0;
public:
    virtual void SetVideoWindow(void* pVideoWindow, int nVideoType) = 0;
    virtual bool AddVideoData(unsigned char*pData,int nLen, uint32_t nTimeStamp) = 0;
};

VideoPalyback* CteateVideoplayback();
void ReleaseVideoplayback(VideoPalyback* pVideoPalyback);

#endif
