//
//  CapScreenEnc.hpp
//  VideoLibrary
//
//  Created by zhangxinming on 16/4/27.
//  Copyright © 2016年 wang guijun. All rights reserved.
//

#ifndef CapScreenEnc_hpp
#define CapScreenEnc_hpp

#include <stdio.h>
#include "VideoLibrary.h"
#import "CapScreen.h"
class VideoCapScreenEnc : public  VideoCapScreen
{
public:
    VideoCapScreenEnc(VideoCaptureDataCallBack* pVideoCaptureDataCallBack);
    ~VideoCapScreenEnc();
public:
    virtual bool StartCapture(int resType)  ;
    virtual void StopCapture() ;
//    
//public:
//    virtual bool StartEncoding() ;
//    virtual void StopEncoding()  ;
//    virtual bool StartEncodingData() ;
//    virtual void StopEncodeingData()  ;
//    
//    virtual void SetPause(bool isPause = true) ;
//public:
//    virtual void SetBitRate(unsigned int uiBitRate) ;
   virtual void SetResolution(unsigned int uiWidth, unsigned int uiHeight) ;
//    
//    virtual unsigned int GetBitRate()  ;
//    virtual void GetResolution(unsigned int& uiWidth, unsigned int& uiHeight)  ;
//    
//    virtual void SetVideoFps(unsigned int uiFps)  ;
//    virtual unsigned int GetVideoFps()  ;
    VideoCaptureDataCallBack* m_pVideoCaptureDataCallBack;
    CapScreen *m_CapScreen;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
};



#endif /* CapScreenEnc_hpp */
