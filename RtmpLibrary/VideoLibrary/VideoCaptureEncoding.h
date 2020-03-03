//
//  VideoCaptureEncoding.h
//  VideoLibrary
//
//  Created by wang guijun on 13-1-20.
//  Copyright (c) 2013年 wang guijun. All rights reserved.
//

#ifndef VideoLibrary_VideoCaptureEncoding_h
#define VideoLibrary_VideoCaptureEncoding_h

#import "VideoLibrary.h"
#import "VideoManagement.h"
#import <UIKit/UIKit.h>
#import "BeautifyFace.h"


class VideoCaptureEncod : public VideoCapture
{
public:
    VideoCaptureEncod(VideoCaptureDataCallBack* pVideoCaptureDataCallBack);
    ~VideoCaptureEncod();
public:
    virtual bool StartCapture(UIView *pUIImageView,int resType,bool beautifyace);
    virtual void StopCapture();
    virtual bool setPreview(UIView *preview);
    
    virtual void  openBeautify();
   virtual  void  closeBeautify();
    virtual void  reOrientation() ;
    virtual void  switchCamera();
    
public:
    virtual bool StartEncoding();
    virtual void StopEncoding();
    virtual bool StartEncodingData();
    virtual void StopEncodeingData();
public:
    virtual void SetFrontAndRearCamera(bool isFront = true);
    virtual void SetLocalVideoWindow(UIView* pVideoWindow);
 
public:
    virtual void SetBitRate(unsigned int uiBitRate);
    virtual void SetResolution(unsigned int uiWidth, unsigned int uiHeight);
    
    void reSetCamera();
    void  SetCamera(bool isFront);
   
    virtual unsigned int GetBitRate();
    virtual void GetResolution(unsigned int& uiWidth, unsigned int& uiHeight);
    
    virtual void SetVideoFps(unsigned int uiFps);
    virtual unsigned int GetVideoFps();
        void On_MediaReceiverCallbackCameraVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight);
private:
    unsigned int m_uiBitRate;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    unsigned int m_uiFps;
    bool         m_isFront;
    UIView*        m_pVideoWindow;
    VideoCaptureDataCallBack* m_pVideoCaptureDataCallBack;
    VideoManagement*          m_pVideoManagement;
      BeautifyFace* pBeautifyFace;
};

#endif
