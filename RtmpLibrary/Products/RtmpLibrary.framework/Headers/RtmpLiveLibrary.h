//
//  RtmpLiveLibrary.h
//  RtmpLibrary
//
//  Created by zhangxinming on 16/5/23.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#ifndef RtmpLiveLibrary_h
#define RtmpLiveLibrary_h
#include <stdio.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
typedef int (*LIVECALLBACK)(int msgcode, bool issucceed,int funtype);

typedef int (*UNITYSENDMSG)(const char *cunityObj,const char* unityFunSucces,const char* msg);

typedef enum
{
    RESOLUTIO_LOW=0,
    RESOLUTIO_MID=1,
    RESOLUTIO_HIGH=2
  
}ResolutionType;


#ifdef __cplusplus
extern "C" {
#endif
    
@class UIView;
 /**
  unity3d相关
  **/
void setLiveUnitySendmsg(UNITYSENDMSG unitymsg);
    
void InitRtmpLiveClass(const char* unityObject,const char* UnityFunSucces,const char* UnityFunFail);
 
    typedef int (*RTMPCOMMONCALLBACK)(const char *retMsg ,id object);
    
    /////////////////////////////////////////////普通app调用方式
    //初始化视频回调
    void initRTMPCallback(RTMPCOMMONCALLBACK callback,id object);
    
    
    
//屏幕录制直播
void RtmpStartScreenLive(const char* baseurl);
void RtmpStopScreenLive();
    
void RtmpOpenCamera(int x,int y);
void RtmpCloseCamera();
void RtmpReOrientationCamera();
    
//摄像头直播
void RtmpStartCameraLive(const char* baseurl,int resType,UIView *lcoalview);
void RtmpStopCameraLive();
 
    
//音频直播
void RtmpStartVoiceLive(const char* baseurl);
void RtmpStopVoiceLive();
 
//观看直播（底层创建播放的视图窗口）
void RtmpStartWatch(const char * liveUrl);

void RtmpStopWatch();
    
    
void reSetWatchVidoeFrame();
//UI层提供播放的视图窗口
void RtmpStartWatchWithView(const char * liveUrl,UIView* uiview);
    
void RtmpOpenCameraWithView(UIView *camView);
void sWitchCamera();
//void RtmpGetLiveStatus();
//    void RtmpLiveUP();
#ifdef __cplusplus
}
#endif
#endif /* RtmpLiveLibrary_h */
