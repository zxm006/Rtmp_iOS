#ifndef _UU_OPEN_LOCAL_USER_H_
#define _UU_OPEN_LOCAL_USER_H_

#include "VideoLibrary.h"
#include "AudioLibrary.h"
#include "uuRtmpClient.h"
#include <string>

class COpenLocalUser : public VideoCaptureDataCallBack , public AudioCaptureDataCallBack
{
public:
	COpenLocalUser();
	~COpenLocalUser(void);
    
public:
    bool ConnectMediaServer(UIView* pView, const char* czpUrl,int isCapScreen=0, int resType = 0, bool bBefore = true);
    void ReleaseMediaSever();
    
    void  closeBeautify();
    void  openBeautify();
    void  reOrientation();
    void  switchCamera();
private:
	bool  OpenLocalVideo(int resType);
	bool  CloseLocalVideo();

    
    bool OpenCapScreen(int resType);
    bool CloseCapScreen();

	bool OpenLocalAudio();
	bool CloseLocalAudio();
private:
    virtual void On_MediaReceiverCallbackVideo(unsigned char*pData,int nLen, bool bKeyFrame, int nWidth, int nHeight);
    virtual void On_MediaReceiverCallbackAudio(unsigned char*pData,int nLen);
private:
	CRtmpClient*        m_pRtmpClient;
	unsigned long       m_playtick;
    unsigned long       m_vtick;
    unsigned long       m_atick;
    bool                m_bBefore;
	int					m_nWidth;
	int					m_nHeight;
	int                 m_nBitRate;
	int					m_nFrameRate;
    bool                m_isCapScreen;
	UIView*               m_PreviewWnd;
      int m_isnosend  ; 
    
    VideoCapture*       m_pVideoCapture;
    AudioCapture*       m_pAudioCapture;
    VideoCapScreen*     m_VideoCapScreen;
      dispatch_queue_t  m_send_video_queue;
};




#endif
