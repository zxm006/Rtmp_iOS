//
//  CapScreenEnc.cpp
//  VideoLibrary
//
//  Created by zhangxinming on 16/4/27.
//  Copyright © 2016年 wang guijun. All rights reserved.
//

#include "CapScreenEnc.h"
VideoCapScreenEnc::VideoCapScreenEnc(VideoCaptureDataCallBack* pVideoCaptureDataCallBack)
:m_pVideoCaptureDataCallBack(pVideoCaptureDataCallBack)
{
//    m_CapScreen = [[CapScreen alloc] init];

    
}

VideoCapScreenEnc::~VideoCapScreenEnc()
{
    //if(m_pVideoManagement!=nil)
    //[m_pVideoManagement release];
//    m_CapScreen=nil;
}

bool VideoCapScreenEnc::StartCapture(int resType)
{
    if(m_CapScreen!=nil)
    {
        [m_CapScreen stopCapScreen];
        m_CapScreen=nil;
    }
     m_CapScreen = [[CapScreen alloc] init];
        [m_CapScreen SetCallBack:m_pVideoCaptureDataCallBack];
    [m_CapScreen startCapScreen:resType];
    return true;
}
void VideoCapScreenEnc::SetResolution(unsigned int uiWidth, unsigned int uiHeight)
{
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
    if(m_CapScreen!=nil)
        [m_CapScreen SetResolution:uiWidth uiHeight:uiHeight];
}


void VideoCapScreenEnc::StopCapture()
{
    if(m_CapScreen!=nil)
    {
        [m_CapScreen stopCapScreen];
//        m_CapScreen=nil;
    }
    
}