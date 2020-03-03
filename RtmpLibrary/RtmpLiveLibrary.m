//
//  RtmpLiveLibrary.c
//  RtmpLibrary
//
//  Created by zhangxinming on 16/5/23.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#include "RtmpLiveLibrary.h"
#include "OpenRtmpRemoteUser.h"
#include "uuOpenLocalUser.h"
#import  "LiveUIViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "RtmpTouchMoveView.h"

COpenLocalUser*         m_pCOpenRtmpLocalUser = NULL;
OpenRtmpRemoteUser*         m_pOpenRtmpRemoteUser = NULL;

static RtmpTouchMoveView *m_rtmptouchMoveView =nil;
dispatch_queue_t  m_rtmp_queue = nil;
dispatch_queue_t  m_rtmp_play_queue = nil;
typedef int (*PLAYCALLBACK)(const char *info);

@interface UnityMsgObjLive: NSObject
@property(nonatomic,strong)NSString *functionName;
@property(nonatomic,strong)NSString *UnityObject;
@property(nonatomic,strong)NSString *UnityFunSucces;
@property(nonatomic,strong)NSString *UnityFunFail;

@end

@implementation UnityMsgObjLive

- (void)dealloc
{
    self.functionName =nil;
      self.UnityObject =nil;
      self.UnityFunSucces =nil;
      self.UnityFunFail =nil;
    [super dealloc];
}
@end

static const  NSDictionary *errcodedic = @{@"10001":@"没有摄像头访问权限",@"10002":@"没有读文件的权限",@"10003":@"没有写文件的权限",@"10004":@"没有使用闪光灯权限",@"10005":@"没有外部存储卡读写权限",@"10006":@"没有使用麦克风权限",@"10008":@"没有访问网络的权限",@"11001":@"文件不存在",@"11002":@"文件格式错误",@"11003":@"文件有破损，无法解析",@"11004":@"存储空间不足，无法写入",@"12001":@"网络请求异常，状态码",@"12002":@"返回数据的格式错误",@"40001":@"不合法的调用凭证",@"40002":@"不合法的grant_type",@"40003":@"不合法的OpenID",@"40004":@"不合法的媒体文件类型",@"40007":@"不合法的media_id",@"40008":@"不合法的message_type",@"40009":@"不合法的图片大小",@"40010":@"不合法的语音大小",@"40011":@"不合法的视频大小",@"40012":@"不合法的缩略图大小",@"40013":@"不合法的AppID",@"40014":@"不合法的access_token",@"40020":@"不合法的url长度",@"41001":@"缺失access_token参数",@"41002":@"缺失appid参数",@"41003":@"缺失refresh_token参数",@"41004":@"缺失secret参数",@"41005":@"缺失二进制媒体文件",@"41006":@"缺失media_id参数",@"50001":@"接口未授权"};

UnityMsgObjLive *m_rtmp_unityMsgObjLive;

//NSString *m_rtmp_live_baseurl;

UNITYSENDMSG m_unitysendmsg;

static bool m_isstartlive = NO;

static bool m_isstartwatchlive = NO;

RTMPCOMMONCALLBACK rtmpcommoncllback = NULL;
id rtmpcommoncllbackObject = nil;
/////////////////////////////////////////////普通app调用方式
//初始化视频回调
void initRTMPCallback(RTMPCOMMONCALLBACK callback,id object)
{
    if(m_rtmp_queue)
    {
        dispatch_release(m_rtmp_queue);
        m_rtmp_queue=nil;
    }
    
    m_rtmp_queue = dispatch_queue_create("rtmp_queue", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(m_rtmp_queue, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0));
    
    if(m_rtmp_play_queue)
    {
        dispatch_release(m_rtmp_play_queue);
        m_rtmp_play_queue=nil;
    }
    
    m_rtmp_play_queue = dispatch_queue_create("m_rtmp_play_queue", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(m_rtmp_play_queue, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
    
    
    
    rtmpcommoncllback = callback;
    rtmpcommoncllbackObject = object;
}

void setLiveUnitySendmsg(UNITYSENDMSG unitymsg)
{
    m_unitysendmsg=unitymsg;
}

void InitRtmpLiveClass(const char* unityObject,const char* UnityFunSucces,const char* UnityFunFail)
{
    m_rtmp_unityMsgObjLive=[[UnityMsgObjLive alloc]init];
    m_rtmp_unityMsgObjLive.UnityObject=[NSString stringWithFormat:@"%s",unityObject];
    m_rtmp_unityMsgObjLive.UnityFunSucces=[NSString stringWithFormat:@"%s",UnityFunSucces];
    m_rtmp_unityMsgObjLive.UnityFunFail=[NSString stringWithFormat:@"%s",UnityFunFail];
    m_rtmp_unityMsgObjLive.functionName=[NSString stringWithFormat:@"RtmpLibraryCallback"];
    
    
    if(m_rtmp_queue)
    {
        dispatch_release(m_rtmp_queue);
        m_rtmp_queue=nil;
    }
    
    m_rtmp_queue = dispatch_queue_create("rtmp_queue", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(m_rtmp_queue, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0));
    
    
    
    if(m_rtmp_play_queue)
    {
        dispatch_release(m_rtmp_play_queue);
        m_rtmp_play_queue=nil;
    }
    
    m_rtmp_play_queue = dispatch_queue_create("m_rtmp_play_queue", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(m_rtmp_play_queue, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
    
    NSLog(@" liveRTMP InitLiveClass cunityObj= %@ unityFunSucces = %@ UnityFunFail == %@",m_rtmp_unityMsgObjLive.UnityObject,m_rtmp_unityMsgObjLive.UnityFunSucces,m_rtmp_unityMsgObjLive.UnityFunFail);
    
}

void sendLiveUnityMessage(bool issucceed,const char *msg)
{
    const char *cunityObj = [m_rtmp_unityMsgObjLive.UnityObject UTF8String];
    const char *unityFunSucces = [m_rtmp_unityMsgObjLive.UnityFunSucces UTF8String];
    const char *unityFunFail = [m_rtmp_unityMsgObjLive.UnityFunFail UTF8String];
    
    NSLog(@"liveRTMP sendLiveUnityMessage ");
    
    if (m_unitysendmsg) {
        if (issucceed)
        {
            NSLog(@" liveRTMP cunityObj= %s unityFunSucces = %s msg == %s",cunityObj,unityFunSucces,msg);
            
            m_unitysendmsg(cunityObj,unityFunSucces, msg);
        }
        else
        {
            NSLog(@"liveRTMP cunityObj= %s unityFunSucces = %s msg == %s",cunityObj,unityFunSucces,msg);
            
            m_unitysendmsg(cunityObj,unityFunFail, msg);
        }
     }
    else
    {
        NSLog(@"liveRTMP m_unitysendmsg=NULL");
        
    }
}

int  RtmpLivecallback (int msgcode, bool issucceed,const char* funtype,const char* url)
{
    NSString *smsg=nil;
   if (issucceed&&msgcode==0)
    {
        smsg=[NSString stringWithFormat:@"{\"funtype\":\"%s\",\"url\":\"%s\"}",funtype,url];
    }
    else
    {
        NSString *strmsgcode=[NSString stringWithFormat:@"%d",msgcode];
        NSString *strmsg = [errcodedic objectForKey:strmsgcode];
        if(strmsg)
            smsg=[NSString stringWithFormat:@"{\"code\":\"%@\",\"message\":\"%@\"}",strmsgcode,strmsg];
        else
        {
            smsg=[NSString stringWithFormat:@"{\"code\":%@,\"message\":\"%@\"}",strmsgcode,@"未知错误"];
        }
   }
    NSString *strmsg=[NSString stringWithFormat:@"{\"callBackFun\":\"%s\",\"retMsg\":%@}",funtype,smsg];
    if (rtmpcommoncllback) {
        rtmpcommoncllback([strmsg UTF8String],rtmpcommoncllbackObject);
    }
    
    sendLiveUnityMessage(issucceed,[strmsg UTF8String]);
    return  0;
}



int  RtmpLiveStatuscallback (int msgcode, bool issucceed,const char* LiveStatus,const char* CamStatus)
{
    NSString *smsg=nil;
    if (issucceed&&msgcode==0)
    {
        smsg=[NSString stringWithFormat:@"{\"LiveStatus\":\"%s\",\"CamStatus\":\"%s\"}",LiveStatus,CamStatus];
    }
    else
    {
        NSString *strmsgcode=[NSString stringWithFormat:@"%d",msgcode];
        NSString *strmsg = [errcodedic objectForKey:strmsgcode];
        if(strmsg)
            smsg=[NSString stringWithFormat:@"{\"code\":\"%@\",\"message\":\"%@\"}",strmsgcode,strmsg];
        else
        {
            smsg=[NSString stringWithFormat:@"{\"code\":%@,\"message\":\"%@\"}",strmsgcode,@"未知错误"];
        }
    }
    NSString *strmsg=[NSString stringWithFormat:@"{\"callBackFun\":\"RtmpGetLiveStatus\",\"retMsg\":%@}",smsg];
    
    sendLiveUnityMessage(issucceed,[strmsg UTF8String]);
    return  0;
}

bool checkAudio()
{
    AVAudioSession *avSession = [AVAudioSession sharedInstance];
    
    __block bool ret=YES;
    
    if ([avSession respondsToSelector:@selector(requestRecordPermission:)]) {
        
        [avSession requestRecordPermission:^(BOOL available) {
         ret= available;
         if (available) {
         //completionHandler
         
         }
         else
         {
         dispatch_async(dispatch_get_main_queue(), ^{
            [[[UIAlertView alloc] initWithTitle:@"无法录音" message:@"请在“设置-隐私-麦克风”选项中允许融企信访问你的麦克风" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil] show];
            //        return available;
        });
      }
    }];
        
           [[AVAudioSession sharedInstance] setActive:YES error:nil];
}
    return ret;
}

bool checkVideo()
{
NSString *mediaType = AVMediaTypeVideo;// Or AVMediaTypeAudio
AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:mediaType];

// This status is normally not visible—the AVCaptureDevice class methods for discovering devices do not return devices the user is restricted from accessing.
if(authStatus ==AVAuthorizationStatusAuthorized){
    NSLog(@"Restricted");
    
    return YES;
    
}else if(authStatus == AVAuthorizationStatusDenied){
    // The user has explicitly denied permission for media capture.
    NSLog(@"Denied");     //应该是这个，如果不允许的话
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"提示"
                                                    message:@"请在设备的\"设置-隐私-相机\"中允许访问相机。"
                                                   delegate:nil
                                          cancelButtonTitle:@"确定"
                                          otherButtonTitles:nil];
    [alert show];
return NO;
}
    return YES;
}

int  playcallback (const char *info)
{
    RtmpLivecallback(0,YES,"RtmpSignalLost","");
  
    NSLog(@"RtmpSignalLost");
    
    m_isstartwatchlive = NO;
    return 0;
}



void RtmpOpenCamera(int x,int y )
{
   if (!checkVideo()) {
      RtmpLivecallback(10001,NO,"没有打开摄像头的权限","");
       return  ;
    }
    
    if (m_rtmptouchMoveView) {
        return;
    }
    UIViewController *rootvc = nil;
    
    UIWindow * window = [[UIApplication sharedApplication] keyWindow];
    if (window.windowLevel != UIWindowLevelNormal)
    {
        NSArray *windows = [[UIApplication sharedApplication] windows];
        for(UIWindow * tmpWin in windows)
        {
            if (tmpWin.windowLevel == UIWindowLevelNormal)
            {
                window = tmpWin;
                break;
            }
        }
    }
    
    UIView *frontView = [[window subviews] objectAtIndex:0];
    id nextResponder = [frontView nextResponder];
    
    if ([nextResponder isKindOfClass:[UIViewController class]])
        rootvc = nextResponder;
    else
        rootvc = window.rootViewController;
    
    if (m_rtmptouchMoveView) {
        [m_rtmptouchMoveView stopPreview];
        
        [m_rtmptouchMoveView removeFromSuperview];
        [m_rtmptouchMoveView release];
        m_rtmptouchMoveView = nil;
    }
    
     m_rtmptouchMoveView= [[RtmpTouchMoveView alloc]initWithFrame:CGRectMake(x, y, 200, 200) ];
    m_rtmptouchMoveView.userInteractionEnabled=YES;
    [m_rtmptouchMoveView startPreview];
    
    [ rootvc.view addSubview:m_rtmptouchMoveView];
 
        RtmpLivecallback(0,YES,"RtmpStartLiveCamera","");
        return  ;
   
}



void RtmpOpenMoveCameraWithView(UIView *camView  )
{
    if (!checkVideo()) {
        RtmpLivecallback(10001,NO,"没有打开摄像头的权限","");
        return  ;
    }
    
    if (!camView) {
        return;
    }
 
    m_rtmptouchMoveView= [[RtmpTouchMoveView alloc]initWithFrame:camView.bounds];
    m_rtmptouchMoveView.userInteractionEnabled=YES;
    [m_rtmptouchMoveView startPreview];
    
//   m_rtmptouchMoveView.layer.cornerRadius = m_rtmptouchMoveView.bounds.size.width / 2;
//    m_rtmptouchMoveView.clipsToBounds = YES;
    
    [camView addSubview:m_rtmptouchMoveView];
    
//    RtmpLivecallback(0,YES,"RtmpStartLiveCamera","");
    return  ;
    
}

void   closeMoveCameraBeautify()
{
    if (m_rtmptouchMoveView) {
        [m_rtmptouchMoveView closeBeautify];
    }
}
void  openMoveCameraBeautify()
{
    if (m_rtmptouchMoveView) {
        [m_rtmptouchMoveView openBeautify];
    }
    
}


void sWitchMoveCamera()
{
    if (m_rtmptouchMoveView)
    {
        [m_rtmptouchMoveView sWitchCamera];
    }
}

void RtmpCloseMoveCamera( )
{
    if (m_rtmptouchMoveView)
    {
        [m_rtmptouchMoveView stopPreview];
        [m_rtmptouchMoveView removeFromSuperview];
        [m_rtmptouchMoveView release];
        m_rtmptouchMoveView = nil;
    }
      RtmpLivecallback(0,YES,"RtmpStopLiveCamera","");
}

void RtmpReOrientationMoveCamera()
{
    if (m_rtmptouchMoveView) {
       [m_rtmptouchMoveView reOrientation];
    }
}


void RtmpChangeFrameMoveCamera(CGRect frame)
{
    if (m_rtmptouchMoveView)
    {
        [m_rtmptouchMoveView setFrame:frame];
        [m_rtmptouchMoveView ChangeFrame];
    }
    
}




void RtmpGetLiveStatus()
{
    NSString *islive;
    
    NSString *isCAM;
    
    if(m_isstartlive)
    {
        
        islive = [NSString stringWithFormat:@"true"];
        
    }
    else
    {
        islive = [NSString stringWithFormat:@"false"];
    }
    
    if(m_rtmptouchMoveView&&m_rtmptouchMoveView.isHidden==NO)
    {
        isCAM = [NSString stringWithFormat:@"true"];
    }
    else
    {
        isCAM = [NSString stringWithFormat:@"false"];
    }
    
       RtmpLiveStatuscallback(0,YES,[islive UTF8String],[isCAM UTF8String]);
    
}



void StopLive(const char* funname)
{
    if (!m_isstartlive) {
        return;
    }
     dispatch_async(dispatch_get_main_queue(), ^{
           RtmpCloseMoveCamera();
        });
    dispatch_sync(m_rtmp_queue, ^{
    if (m_pCOpenRtmpLocalUser)
    {
        m_pCOpenRtmpLocalUser->ReleaseMediaSever();
        delete  m_pCOpenRtmpLocalUser;
        m_pCOpenRtmpLocalUser =NULL;
        
    }
       });
    
    RtmpLivecallback(0,YES,funname,"");
    
    m_isstartlive =NO;
    
    [[UIApplication sharedApplication] setIdleTimerDisabled: NO];
}


bool StartLiveWithType(const char *liveurl,int type,const char *funname,int restype,UIView *localview)
{
    [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
   
    NSLog(@"liveurl = %s",liveurl);
    
  if (m_isstartlive) {
        StopLive(funname);
        sleep(1);
    }
    
    if (!checkAudio()) {
        RtmpLivecallback(10006,NO,"没有打开话筒的权限","");
        return NO;
    }
    
    m_pCOpenRtmpLocalUser = new COpenLocalUser;
    __block bool ret=NO;
    dispatch_sync(m_rtmp_queue, ^{
     ret =m_pCOpenRtmpLocalUser->ConnectMediaServer(localview,liveurl,type,restype);
   if(ret)
        {
            RtmpLivecallback(0,YES,funname,liveurl);
            
        }
        else
        {
           RtmpLivecallback(12001,NO,funname,liveurl);
        }
    });
    m_isstartlive = ret;
    return m_isstartlive;
}

void RtmpStartScreenLive(const char* baseurl)
{
    
   int resType =1;
    
    if ( !baseurl) {
             RtmpLivecallback(12001,NO,"RtmpStartScreenAudioLive","");
        return;
    }
    NSString *strv=[NSString stringWithFormat:@"%s",baseurl];
    
    NSURL *url=[NSURL URLWithString:strv];
   
    if ( !url) {
             RtmpLivecallback(12001,NO,"RtmpStartScreenAudioLive","");
        return;
    }
    
    StartLiveWithType([strv UTF8String],1,"RtmpStartScreenAudioLive",resType,NULL);
}

 void RtmpStopScreenLive()
{
    StopLive("RtmpStopScreenAudioLive");
}

void RtmpStartCameraLive(const char* baseurl,int resType,UIView *lcoalview)
{
    if ( !baseurl) {
        RtmpLivecallback(12001,NO,"RtmpStartCameraAudioLive","");
        return;
    }
    
    NSString *strv=[NSString stringWithFormat:@"%s",baseurl];
    
    NSURL *url=[NSURL URLWithString:strv];
    
    if ( !url) {
        RtmpLivecallback(12001,NO,"RtmpStartCameraAudioLive","");
        return;
    }
 
//    CFUUIDRef uuidRef = CFUUIDCreate(kCFAllocatorDefault);
//    NSString *uuid = (NSString *)CFUUIDCreateString (kCFAllocatorDefault,uuidRef);
//    NSString *strurl =[NSString stringWithFormat:@"%@/%@",strv,uuid];
    StartLiveWithType([strv UTF8String],2,"RtmpStartCameraAudioLive",resType,lcoalview);
//    CFRelease(uuidRef);
//    CFRelease(uuid);
}

 void RtmpStopCameraLive()
{
    StopLive("RtmpStopCameraAudioLive");
}

 void RtmpStartVoiceLive(const char* baseurl)
{
   
    if ( !baseurl) {
        RtmpLivecallback(12001,NO,"RtmpStartAudioLive","");
        return;
    }
    NSString *strv=[NSString stringWithFormat:@"%s",baseurl];
    
    NSURL *url=[NSURL URLWithString:strv];
    
    if ( !url) {
        RtmpLivecallback(12001,NO,"RtmpStartAudioLive","");
        return;
    }
    
//    CFUUIDRef uuidRef = CFUUIDCreate(kCFAllocatorDefault);
//    NSString *uuid = (NSString *)CFUUIDCreateString (kCFAllocatorDefault,uuidRef);
//    NSString *strurl =[NSString stringWithFormat:@"%@/%@",strv,uuid];
    StartLiveWithType([strv UTF8String],0,"RtmpStartAudioLive",0,NULL);
//    CFRelease(uuidRef);
//    CFRelease(uuid);
}

void RtmpStopVoiceLive()
{
    StopLive("RtmpStopAudioLive");
}
 
void RtmpStartWatch(const char * liveUrl)
{
    [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
    
 
    
    __block bool ret =NO;
    dispatch_async(m_rtmp_play_queue, ^{
    if (m_pOpenRtmpRemoteUser)
    {
        m_pOpenRtmpRemoteUser->ReleaseMediaSever();
        delete  m_pOpenRtmpRemoteUser;
        m_pOpenRtmpRemoteUser =NULL;
    }
    
    UIViewController *rootvc = nil;
    
    UIWindow * window = [[UIApplication sharedApplication] keyWindow];
    if (window.windowLevel != UIWindowLevelNormal)
    {
        NSArray *windows = [[UIApplication sharedApplication] windows];
        for(UIWindow * tmpWin in windows)
        {
            if (tmpWin.windowLevel == UIWindowLevelNormal)
            {
                window = tmpWin;
                break;
            }
        }
    }
    
//    UIView *frontView = [[window subviews] objectAtIndex:0];
//    id nextResponder = [frontView nextResponder];
//    
//    if ([nextResponder isKindOfClass:[UIViewController class]])
//        rootvc = nextResponder;
//    else
        rootvc = window.rootViewController;
 
        m_pOpenRtmpRemoteUser = new OpenRtmpRemoteUser(playcallback);
        
        ret =  m_pOpenRtmpRemoteUser->ConnectMediaServer ((__bridge void*)rootvc.view,liveUrl,6);
   
        if(ret)
        {
            RtmpLivecallback(0,YES,"RtmpWatchLive","");
            NSLog(@"liveRTMP Livecallback yes ");
        }
        else
        {
            NSLog(@"liveRTMP Livecallback no ");
            RtmpLivecallback(12001,NO,"RtmpWatchLive","");
        }
    });
//    m_isstartwatchlive = ret;
//        usleep(500000);
    return  ;
}


void   RtmpCloseBeautify()
{
    if(m_pCOpenRtmpLocalUser != NULL)
    {
        m_pCOpenRtmpLocalUser->closeBeautify();
        
        
    }
}

void   RtmpOpenBeautify()
{
    if(m_pCOpenRtmpLocalUser != NULL)
    {
        m_pCOpenRtmpLocalUser->openBeautify();
        
        
    }
}

void RtmpReOrientation()
{
    if(m_pCOpenRtmpLocalUser != NULL)
    {
        m_pCOpenRtmpLocalUser->reOrientation();
        
        
    }
}

void  RtmpSwitchCamera()
{
    if(m_pCOpenRtmpLocalUser != NULL)
    {
        m_pCOpenRtmpLocalUser->switchCamera();
        
        
    }
}

void RtmpStartWatchWithView(const char * liveUrl,UIView* uiview)
{
 __block bool ret =NO;
    dispatch_async(m_rtmp_play_queue, ^{
        
        [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
        
 
        m_pOpenRtmpRemoteUser = new OpenRtmpRemoteUser(playcallback);
        
        m_isstartwatchlive =  m_pOpenRtmpRemoteUser->ConnectMediaServer ((__bridge void*)uiview,liveUrl,6);
        
        if(ret)
        {
            RtmpLivecallback(0,YES,"RtmpWatchLive","");
            NSLog(@"liveRTMP Livecallback yes ");
        }
        else
        {
            NSLog(@"liveRTMP Livecallback no ");
            RtmpLivecallback(12001,NO,"RtmpWatchLive","");
        }
    });
 
    return  ;
}

void RtmpStopWatch()
{
    [[UIApplication sharedApplication] setIdleTimerDisabled: NO];
    

      dispatch_async(m_rtmp_play_queue, ^{
          if (m_pOpenRtmpRemoteUser)
            {
 
                
                m_pOpenRtmpRemoteUser->ReleaseMediaSever();
                delete  m_pOpenRtmpRemoteUser;
                m_pOpenRtmpRemoteUser = NULL;
            }
 
     });
 
    
    RtmpLivecallback(0,YES,"RtmpStopWatchLive","");
}



void reSetWatchVidoeFrame()
{
   
        if (m_pOpenRtmpRemoteUser)
        {
            
            
            m_pOpenRtmpRemoteUser->reSetVidoeFrame();
    
        }
 
    
    
}

void RtmpLiveUP()
{

  
    [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
    
    if (m_isstartwatchlive) {
        RtmpStopWatch();
        sleep(1);
    }
    
    if (m_pOpenRtmpRemoteUser)
    {
        m_pOpenRtmpRemoteUser->ReleaseMediaSever();
        delete  m_pOpenRtmpRemoteUser;
        m_pOpenRtmpRemoteUser =NULL;
    }
    
    UIViewController *rootvc = nil;
    
    UIWindow * window = [[UIApplication sharedApplication] keyWindow];
    if (window.windowLevel != UIWindowLevelNormal)
    {
        NSArray *windows = [[UIApplication sharedApplication] windows];
        for(UIWindow * tmpWin in windows)
        {
            if (tmpWin.windowLevel == UIWindowLevelNormal)
            {
                window = tmpWin;
                break;
            }
        }
    }
    
    UIView *frontView = [[window subviews] objectAtIndex:0];
    id nextResponder = [frontView nextResponder];
    
    if ([nextResponder isKindOfClass:[UIViewController class]])
        rootvc = nextResponder;
    else
        rootvc = window.rootViewController;
        NSBundle *bundle =  [NSBundle bundleWithIdentifier:@"identifier.aiyou.RtmpLibrary"];
    
     LiveUIViewController*   m_liveUIViewController = [[LiveUIViewController alloc]initWithNibName:@"LiveUIViewController" bundle: bundle ];
        UIWebView * tmpsubView=nil;
    
        for (UIView *subView in rootvc.view.subviews)
        {
            if ([subView isKindOfClass:[UIWebView class]])
            {
                tmpsubView = (UIWebView*)subView;
      
            }
        }
        if (tmpsubView)
        {
            [rootvc.view insertSubview:m_liveUIViewController.view belowSubview:tmpsubView];
        }
        else
        {
    
            [rootvc.view addSubview:m_liveUIViewController.view];
        }
    
        [m_liveUIViewController.view setFrame:  rootvc.view.frame];
}
