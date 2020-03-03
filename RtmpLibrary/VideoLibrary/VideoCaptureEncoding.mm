 
#include "VideoCaptureEncoding.h"


VideoCaptureEncod::VideoCaptureEncod(VideoCaptureDataCallBack* pVideoCaptureDataCallBack)
:m_pVideoCaptureDataCallBack(pVideoCaptureDataCallBack)
,m_pVideoWindow(nil)
,m_uiFps(15)
,m_uiWidth(640)
,m_uiHeight(480)
,m_pVideoManagement(nil)
{
      pBeautifyFace =   [[BeautifyFace alloc]init];
 
}

VideoCaptureEncod::~VideoCaptureEncod()
{
 
    if(pBeautifyFace)
    {
        [pBeautifyFace release];
    pBeautifyFace =nil;
    }
    
}

bool VideoCaptureEncod::StartCapture(UIView *pUIImageView,int resType,bool beautifyace)
{
 
    if(pBeautifyFace)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [pBeautifyFace SetPreview:pUIImageView];
            [pBeautifyFace setVideoDataOutputBuffer: this];
            [pBeautifyFace SetFrontCamera];
            [pBeautifyFace StartVideoCapture];
        });
    }
    
    return true;
}

void VideoCaptureEncod::openBeautify( )
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
    
        [pBeautifyFace openBeautify];
    });
    
}

void VideoCaptureEncod::closeBeautify( )
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
        
        [pBeautifyFace closeBeautify];
    });
    
}


  void  VideoCaptureEncod::reOrientation()
{
    if(pBeautifyFace)
    {
        [pBeautifyFace reOrientation];
    }
}

  void  VideoCaptureEncod::switchCamera()
{
    if(pBeautifyFace)
    {
        [pBeautifyFace switchCamera];
    }
}

void VideoCaptureEncod::SetCamera(bool isFront)
{
 
        if(pBeautifyFace)
        {
            [pBeautifyFace StopVideoCapture];
            //            pBeautifyFace->SetPreview(m_pUIImageView);
            if(isFront)//true:前置, false:后置
            {
                [pBeautifyFace SetFrontCamera];
            }
            else
            {
                [pBeautifyFace  SetBackCamera];
            }
            [pBeautifyFace StartVideoCapture];
            
        }
}

void VideoCaptureEncod::StopCapture()
{
 
    if(pBeautifyFace)
    {
        [pBeautifyFace setVideoDataOutputBuffer: NULL];
        
        [pBeautifyFace StopVideoCapture];
    }

    
}

bool VideoCaptureEncod::setPreview(UIView *preview)
{
//    [m_pVideoManagement setPreview:preview];
    if(pBeautifyFace)
        [pBeautifyFace SetPreview: preview];
    return  YES;
}

bool VideoCaptureEncod::StartEncoding()
{
    if(m_pVideoManagement!=nil)
    {
        if(m_pVideoCaptureDataCallBack)
            [m_pVideoManagement SetCallBack:m_pVideoCaptureDataCallBack]; 
    }
    return true;
}

void VideoCaptureEncod::StopEncoding()
{
    if(m_pVideoManagement!=nil)
    {
        [m_pVideoManagement SetCallBack:nil]; 
    }
}

void VideoCaptureEncod::On_MediaReceiverCallbackCameraVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight)
{
    if (m_pVideoCaptureDataCallBack)
    {
        m_pVideoCaptureDataCallBack->On_MediaReceiverCallbackVideo(pData,nLen,bKeyFrame,nWidth,nHeight);
    }
}


void VideoCaptureEncod::SetLocalVideoWindow(UIView* pVideoWindow)
{
    m_pVideoWindow = pVideoWindow;
    if(m_pVideoManagement!=nil)
        [m_pVideoManagement SetLocalVideoWindow:m_pVideoWindow];
}

void VideoCaptureEncod::reSetCamera()
{
   
        
        if(pBeautifyFace)
        {
            [pBeautifyFace reSetCamera];
        }
    
}

void VideoCaptureEncod::SetFrontAndRearCamera(bool isFront)
{
    m_isFront = isFront;
    if(m_pVideoManagement!=nil)
    {
        [m_pVideoManagement SetFrontAndRearCamera:isFront];
    }
}

void VideoCaptureEncod::SetBitRate(unsigned int uiBitRate)
{
    m_uiBitRate = uiBitRate;
    if(m_pVideoManagement!=nil)
        [m_pVideoManagement SetBitRate:m_uiBitRate];
    
}

void VideoCaptureEncod::SetResolution(unsigned int uiWidth, unsigned int uiHeight)
{
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
    if(m_pVideoManagement!=nil)
        [m_pVideoManagement SetResolution:uiWidth uiHeight:uiHeight];
}
    
unsigned int VideoCaptureEncod::GetBitRate()
{
    return m_uiBitRate;
}

void VideoCaptureEncod::GetResolution(unsigned int& uiWidth, unsigned int& uiHeight)
{
    uiWidth = m_uiWidth;
    uiHeight = m_uiHeight;
}

void VideoCaptureEncod::SetVideoFps(unsigned int uiFps)
{
    m_uiFps = uiFps;
    if(m_pVideoManagement!=nil)
        [m_pVideoManagement SetVideoFps:m_uiFps];
}

unsigned int VideoCaptureEncod::GetVideoFps()
{
    return m_uiFps;
}

bool VideoCaptureEncod::StartEncodingData()
{
    if(m_pVideoManagement)
        m_pVideoManagement.bIsEncodingData = YES;
    return true;
}

void VideoCaptureEncod::StopEncodeingData()
{
    if(m_pVideoManagement)
        m_pVideoManagement.bIsEncodingData = NO;
}



