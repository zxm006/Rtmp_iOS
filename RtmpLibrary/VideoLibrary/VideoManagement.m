

#import "VideoManagement.h"
@interface VideoManagement ()
{
     int isencode;
}
@end 

@implementation VideoManagement
@synthesize transferBuffer;
@synthesize recordVideo;
@synthesize bIsEncodingData;

- (id)init
{
    self = [super init];
    if (self) {
        m_pUIImageView=nil;
        m_uiBitRate = 0;
        m_uiFps = 15;
        m_uiWidth = 0;
        m_uiHeight = 0;
        m_pVideoCaptureDataCallBack = NULL;
        h264open = NO;
    }
    self.bIsEncodingData = NO;
    return self;
}

- (void)dealloc
{
    m_pUIImageView=nil;
    m_uiBitRate = 0;
    m_uiFps = 0;
    m_uiWidth = 0;
    m_uiHeight = 0;
    m_pVideoCaptureDataCallBack = NULL;
    h264open = NO;
    //[super dealloc];
}

- (void) setPreview: (void*)preview
{
    [[CameraHelp shareCameraHelp] setPreview:preview];
    
}
-(bool)StartCapture:(int) resType
{
    [[CameraHelp shareCameraHelp] setVideoDataOutputBuffer:self];
    [[CameraHelp shareCameraHelp] prepareVideoCapture:resType andFps:m_uiFps andFrontCamera:NO andPreview:m_pUIImageView];
    //开始捕获
	[[CameraHelp shareCameraHelp] startVideoCapture];
    return true;
}

-(void)StopCapture
{
    if (self.recordVideo==YES) {
//        [[CameraHelp shareCameraHelp]stopVideoRecording];
        self.recordVideo=NO;
    }
    [[CameraHelp shareCameraHelp] setVideoDataOutputBuffer:nil];
    self.transferBuffer =NO;
    
    [[CameraHelp shareCameraHelp] stopVideoCapture];
    [CameraHelp closeCamera];
}


-(void)SetLocalVideoWindow:(UIView *)pUIImageView
{
    m_pUIImageView = pUIImageView;
    
}


-(void)SetBitRate:(unsigned int)uiBitRate
{
    m_uiBitRate = uiBitRate;
}

-(void)SetResolution:(unsigned int)uiWidth uiHeight:(unsigned int)uiHeight
{
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
}

-(void)SetVideoFps:(unsigned int)uiFps
{
    m_uiFps = uiFps;
}

-(void)SetCallBack:(VideoCaptureDataCallBack*)pVideoCaptureDataCallBack
{
    m_pVideoCaptureDataCallBack = pVideoCaptureDataCallBack;
}

-(void)SetFrontAndRearCamera:(bool)isFront
{
    if(isFront)
    {
        [[CameraHelp shareCameraHelp] setFrontCamera];
    }
    else
        [[CameraHelp shareCameraHelp] setBackCamera];
    
    
}


-(void)setBackinitEncod:(id)sender
{
      [[CameraHelp shareCameraHelp] setVideoDataOutputBuffer:self];
    self.transferBuffer =YES;
   
}

-(void)setBack:(id)sender
{
   self.transferBuffer =NO;
    [self Alert:@"现场上传失败" message:@"请稍后重新尝试"];
  
}


-(void) On_MediaReceiverCallbackVideo:(unsigned char*)pData nLen:(int )nLen bKeyFrame:( bool) bKeyFrame  nWidth:(int)nWidth   nHeight:(int) nHeight
{
    if(m_pVideoCaptureDataCallBack)
        m_pVideoCaptureDataCallBack->On_MediaReceiverCallbackVideo((unsigned char*)pData, nLen, bKeyFrame, nWidth, nHeight);
}

-(void)Alert:(NSString*) result message:(NSString*) message
{
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:result message:message
                                                   delegate:self cancelButtonTitle:@"确定" otherButtonTitles: nil];
    [alert show];
}

@end
