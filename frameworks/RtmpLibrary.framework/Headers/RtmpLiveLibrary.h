//
//  RtmpLiveLibrary.h
//  RtmpLibrary
//
//  Created by zhangxinming on 16/5/23.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#ifndef RtmpLiveLibrary_h
#define RtmpLiveLibrary_h

typedef int (*LIVECALLBACK)(int msgcode, bool issucceed,int funtype);

typedef int (*UNITYSENDMSG)(const char *cunityObj,const char* unityFunSucces,const char* msg);

typedef enum
{
    RESOLUTIO_LOW=0,
    RESOLUTIO_MID=1,
    RESOLUTIO_HIGH=2
  
}ResolutionType;

#include <stdio.h>
#import <Foundation/Foundation.h>
#ifdef __cplusplus
extern "C" {
#endif

void setLiveUnitySendmsg(UNITYSENDMSG unitymsg);
    
void InitRtmpLiveClass(const char* unityObject,const char* UnityFunSucces,const char* UnityFunFail);
    
 
    
//屏幕录制直播
void RtmpStartScreenLive(const char * liveUrl,int resType);
void RtmpStopScreenLive();
    
void RtmpOpenCamera(int x,int y);
void RtmpCloseCamera();
void RtmpReOrientationCamera();
    
//摄像头直播
void RtmpStartCameraLive(const char *liveUrl,int resType);
void RtmpStopCameraLive();
 
    
//音频直播
void RtmpStartVoiceLive(const char *liveUrl);
void RtmpStopVoiceLive();
 
//观看直播
void RtmpStartWatch(const char * liveUrl);
void RtmpStopWatch();
    
    
#ifdef __cplusplus
}
#endif
#endif /* RtmpLiveLibrary_h */
