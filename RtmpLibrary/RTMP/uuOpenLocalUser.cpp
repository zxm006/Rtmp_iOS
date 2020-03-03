#include "uuOpenLocalUser.h"
#define LOG_TAG "cui"
#include <time.h>
#include <sys/time.h>

unsigned long XGetTimestamp(void)
{
#ifdef WIN32
    return timeGetTime();
#else
    struct timeval now;
    gettimeofday(&now,NULL);
    return now.tv_sec*1000+now.tv_usec/1000;
#endif
}

COpenLocalUser::COpenLocalUser()
:m_pVideoCapture(NULL)
,m_pAudioCapture(NULL)
{
    m_nWidth       = 240;
    m_nHeight      = 320;
    m_nFrameRate   = 20;
    m_nBitRate     = 64;
    m_pRtmpClient  = NULL;
    m_playtick     = 0;
    m_vtick        = 0;
    m_atick        = 0;
    m_bBefore      = true;
    m_isCapScreen    =true;
    m_PreviewWnd   = NULL;
    m_pVideoCapture=NULL;
    m_pAudioCapture=NULL;
    m_send_video_queue = dispatch_queue_create("send_video_queue", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(m_send_video_queue, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0));
}

COpenLocalUser::~COpenLocalUser(void)
{
    ReleaseMediaSever();
    if (m_send_video_queue) {
        //        dispatch_release(m_send_video_queue);
        m_send_video_queue=nil;
    }
}

bool COpenLocalUser::ConnectMediaServer(UIView* pView, const char* czpUrl, int isCapScreen,int resType,  bool bBefore)
{
    //!pView ||
    if( !czpUrl)
    {
        return false;
    }
    
    m_nFrameRate   = 20;
    m_nBitRate     = 200;
    m_bBefore      = bBefore;
    m_PreviewWnd   = pView;
    m_isCapScreen=isCapScreen;
    m_pRtmpClient = new CRtmpClient;
    if(!m_pRtmpClient->ConnectServer(czpUrl))
    {
        return false;
    }
    
    switch (isCapScreen) {
        case 0:
        {
            OpenLocalAudio();
        }
            break;
            
        case 1:
        {
            OpenCapScreen(resType);
            OpenLocalAudio();
        }
            break;
        case 2:
        {
            OpenLocalVideo(resType);
            OpenLocalAudio();
        }
            break;
        default:
        {
            OpenLocalAudio();
        }
            break;
    }
    
    m_playtick = XGetTimestamp();
    
    return true;
}

void COpenLocalUser::ReleaseMediaSever()
{
    if (m_isCapScreen)
    {
        CloseCapScreen();
    }
    else
    {
        CloseLocalVideo();
        
    }
    
    CloseLocalAudio();
    
    if(m_pRtmpClient)
    {
        m_pRtmpClient->DisconnectServer();
        delete m_pRtmpClient;
        m_pRtmpClient = NULL;
    }
}

bool COpenLocalUser::OpenLocalVideo(int resType)
{
    if(m_pVideoCapture)
        return false;
    m_isCapScreen = NO;
    m_pVideoCapture = CteateVideoCapture(this);
    m_pVideoCapture->SetLocalVideoWindow(m_PreviewWnd);
    m_pVideoCapture->SetBitRate(m_nBitRate);
    m_pVideoCapture->SetVideoFps(m_nFrameRate);
    
    m_pVideoCapture->StartCapture((UIView*)m_PreviewWnd,resType,true);
    
    return true;
}

bool COpenLocalUser::CloseLocalVideo()
{
    if(!m_pVideoCapture)
        return false;
    
    m_pVideoCapture->StopCapture();
    m_pVideoCapture->StopEncodeingData();
    m_pVideoCapture->StopEncoding();
    
    ReleaseVideoCapture();
    m_pVideoCapture = NULL;
    
    return true;
}

void   COpenLocalUser::closeBeautify()
{
    if(m_pVideoCapture != NULL)
    {
        m_pVideoCapture->closeBeautify();
        
        
    }
}
void  COpenLocalUser::openBeautify()
{
    if(m_pVideoCapture != NULL)
    {
        m_pVideoCapture->openBeautify();
        
        
    }
}

void  COpenLocalUser:: reOrientation()
{
    if(m_pVideoCapture != NULL)
    {
        m_pVideoCapture->reOrientation();
        
        
    }
}

void  COpenLocalUser::switchCamera()
{
    if(m_pVideoCapture != NULL)
    {
        m_pVideoCapture->switchCamera();
        
        
    }
}


bool COpenLocalUser::OpenCapScreen(int resType)
{
    if(m_pVideoCapture)
        return false;
    m_isCapScreen = YES;
    m_VideoCapScreen = CreateCapScreen(this);
    m_VideoCapScreen->StartCapture(resType);
    
    return true;
}

bool COpenLocalUser::CloseCapScreen()
{
    if(!m_VideoCapScreen)
    {
        printf("m_VideoCapScreen = NULL");
        return false;
    }
    ReleaseCapScreen();
    return true;
}

bool COpenLocalUser::OpenLocalAudio()
{
    if(m_pAudioCapture)
        return false;
    
    m_pAudioCapture = CteateAudioCapture(this);
    m_pAudioCapture->Start();
    
    return true;
}

bool COpenLocalUser::CloseLocalAudio()
{
    if(!m_pAudioCapture)
        return false;
    
    m_pAudioCapture->Stop();
    ReleaseAudioCapture();
    m_pAudioCapture = NULL;
    
    return true;
}



void COpenLocalUser::On_MediaReceiverCallbackVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight)
{
    if(pData && nLen > 0 && m_pRtmpClient)
    {
        if (((m_isnosend == 1||m_isnosend==2)&&!bKeyFrame))
        {
            NSLog(@"丢弃p帧\n");
            usleep(50000);
            return;
        }
        else if (m_isnosend == 2 && bKeyFrame)
        {
            m_isnosend=1;
            
            NSLog(@"丢弃关键帧\n");
            usleep(50000);
            return;
        }
        else
        {
          m_isnosend =0;
        }
        
        m_vtick = XGetTimestamp() - m_playtick;
        dispatch_sync(m_send_video_queue, ^{
            unsigned long   diffold =  XGetTimestamp();
            m_pRtmpClient->SendH264VideoPacket(pData, nLen, nWidth, nHeight, m_nFrameRate, bKeyFrame, (unsigned int)m_vtick);
            
            unsigned long   diff  =  XGetTimestamp() - diffold;
            if(diff>100&&diff<200)
            {
                m_isnosend =1;
                //                               printf("视频发送间隔过大 diff = %ld",diff);
            }
            else if(diff>200)
            {
                
                m_isnosend =2;
            }
            
        });
        
        
    }
}

void COpenLocalUser::On_MediaReceiverCallbackAudio(unsigned char*pData,int nLen)
{
    if(pData && nLen > 0 && m_pRtmpClient)
    {
        m_atick = XGetTimestamp() - m_playtick;
        m_pRtmpClient->SendH264AudioPacket(pData, nLen, m_atick);
    }
}

