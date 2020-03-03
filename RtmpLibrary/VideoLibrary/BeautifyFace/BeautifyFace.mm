#import "BeautifyFace.h"
#include <pthread.h>

#include "VideoCaptureEncoding.h"

@implementation BeautifyFace

- (id)init {
    if (self = [super  init])
    {
        m_pVideoCamera = nil;
        m_PreView = nil;
        m_pFilterView = nil;
        m_pBeautifyFilter = nil;
        
        h264Encoder = [H264HwEncoderImpl alloc];
        [h264Encoder initWithConfiguration];
        h264Encoder.delegate = self;
        isinith264Encoder = NO;
       nBeautifyFrontCamera = true;
        
        SpsPpsdata=[[NSMutableData alloc]init];
        
    }
 
    return self;
}


 - (void)dealloc
{
       m_VideoCaptureEncod = NULL;
    
    if (h264Encoder)
    {
        [h264Encoder End];
        [h264Encoder release];
        h264Encoder=nil;
        isinith264Encoder =NO;
    }
 
    
    [self StopVideoCapture];
    
    m_pVideoCamera = nil;
    m_PreView = nil;
    m_pFilterView = nil;
    m_pBeautifyFilter = nil;
    
    
  
    if(SpsPpsdata)
    {
        [SpsPpsdata release];
        SpsPpsdata =nil;
        
    }
    
    [super dealloc];
}

-(void)   switchCamera 
{
   
    [m_pVideoCamera rotateCamera];
    
    
}

 - (void)reOrientation
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
   m_pVideoCamera.outputImageOrientation =  [[UIApplication sharedApplication] statusBarOrientation];
    [m_pFilterView setFrame:m_PreView.frame];
    });
   
}

- (void) getOutput:(GPUImageRawDataOutput *)rawDataOutput   outputSize:(CGSize) outputSize
{
 [rawDataOutput setNewFrameAvailableBlock:^{
    __strong GPUImageRawDataOutput *strongOutput = rawDataOutput;
    [strongOutput lockFramebufferForReading];
    // 这里就可以获取到添加滤镜的数据了
    GLubyte *outputBytes = [strongOutput rawBytesForImage];
    NSInteger bytesPerRow = [strongOutput bytesPerRowInOutput];
    CVPixelBufferRef pVImageBufferRef = NULL;
    CVPixelBufferCreateWithBytes(kCFAllocatorDefault, outputSize.width, outputSize.height, kCVPixelFormatType_32BGRA, outputBytes, bytesPerRow, nil, nil, nil, &pVImageBufferRef);
    if(!isinith264Encoder)
    {
        int nWidth=(int)CVPixelBufferGetWidth(pVImageBufferRef);
        int nHeight= (int)CVPixelBufferGetHeight(pVImageBufferRef);
        self.width = nWidth;
        self.height = nHeight;
        
        int bitrate =350 * 1024;
        if (nWidth>0&&nHeight>0)
        {
            h264Encoder.delegate = self;
            [h264Encoder initEncode:nWidth height:nHeight  framerate:15 bitrate:bitrate];
            isinith264Encoder =YES;
        }
    }
    [h264Encoder encode:pVImageBufferRef];
    CFRelease(pVImageBufferRef);
    
    [strongOutput unlockFramebufferAfterReading];
    
}];
}


 - (void)  openBeautify
{
        [m_pVideoCamera removeAllTargets];
    
    GPUImageBeautifyFilter *beautifyFilter = [[GPUImageBeautifyFilter alloc] init];
    

    CGSize outputSize = {480, 640};
    GPUImageRawDataOutput *rawDataOutput = [[GPUImageRawDataOutput alloc] initWithImageSize:CGSizeMake(outputSize.width, outputSize.height) resultsInBGRAFormat:YES];
    [beautifyFilter addTarget:rawDataOutput];
    
    [self getOutput:rawDataOutput     outputSize:outputSize];
    [m_pVideoCamera addTarget:beautifyFilter];
    
    [beautifyFilter addTarget:m_pFilterView];

    
    
}

 - (void)  closeBeautify
{
 
    [m_pVideoCamera removeAllTargets];
   
    CGSize outputSize = {480, 640};
    GPUImageRawDataOutput *rawDataOutput = [[GPUImageRawDataOutput alloc] initWithImageSize:CGSizeMake(outputSize.width, outputSize.height) resultsInBGRAFormat:YES];
    [m_pVideoCamera addTarget:rawDataOutput];
    
    [self getOutput:rawDataOutput     outputSize:outputSize];
    
    [m_pVideoCamera addTarget:m_pFilterView];
 
}

 - (bool)  StartVideoCapture
{
    if(m_PreView == nil)
        return false;
    
    if(nBeautifyFrontCamera)//前置摄像头
    {
        m_pVideoCamera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPreset640x480 cameraPosition:AVCaptureDevicePositionFront];
    }
    else//后置摄像头 
    {
        m_pVideoCamera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPreset640x480 cameraPosition:AVCaptureDevicePositionBack];
    }
     
    m_pVideoCamera.outputImageOrientation =  [[UIApplication sharedApplication] statusBarOrientation];
    m_pVideoCamera.horizontallyMirrorFrontFacingCamera = YES;
    m_pFilterView = [[GPUImageView alloc] initWithFrame:m_PreView.frame];
    m_pFilterView.center = m_PreView.center;
    m_pFilterView.fillMode = kGPUImageFillModePreserveAspectRatioAndFill;
    [m_PreView addSubview:m_pFilterView];
     [m_pVideoCamera addTarget:m_pFilterView];
    
  [self openBeautify];
    
    [m_pVideoCamera startCameraCapture];
 
   
    return true;
}

-(void)  StopVideoCapture
{
    if(m_pVideoCamera == nil )
        return;
     [m_pVideoCamera removeInputsAndOutputs];
    [m_pVideoCamera removeAllTargets];
    [m_pVideoCamera stopCameraCapture];
    
    if (m_pFilterView)
    {
//        dispatch_async(dispatch_get_main_queue(), ^{
        
        [m_pFilterView removeFromSuperview];
        m_pFilterView =nil;
//        });
    }
    m_PreView = nil;
 
    [m_pBeautifyFilter removeAllTargets];
    m_pBeautifyFilter = nil;
    
 
}


 -(bool) AddVideoData:(unsigned char*)pData nLen:(int) nLen iskeyframe:(bool) iskeyframe  nWidth:(int )nWidth  nHeight:(int) nHeight
{
   if (m_VideoCaptureEncod )
    {
        m_VideoCaptureEncod->On_MediaReceiverCallbackCameraVideo((unsigned char*)pData,(int)nLen,iskeyframe ,nWidth,nHeight);
    }
    return true;
}



-(void) setVideoDataOutputBuffer:(VideoCaptureEncod*)videoCaptureEncod
{
    m_VideoCaptureEncod = videoCaptureEncod;
}

-(void)  SetPreview:(UIView*) preview
{
    m_PreView = preview;
}

 

//设置前置摄像头
-(bool)  SetFrontCamera
{
    if(nBeautifyFrontCamera != true)
    {
        nBeautifyFrontCamera = true;
    }
    return YES;
}

//设置后置摄像头
-(bool) SetBackCamera 
{
    if(nBeautifyFrontCamera != false)
    {
        nBeautifyFrontCamera = false;
    }
     return YES;
}


- (void)gotSpsPps:(NSData*)sps pps:(NSData*)pps
{
    if( [SpsPpsdata  length]>0)
    {
        [SpsPpsdata setLength:0];
    }
    const char bytes[] = "\x00\x00\x00\x01";
    size_t length = (sizeof bytes) - 1; //string literals have implicit trailing '\0'
    NSMutableData *ByteHeader = [NSMutableData dataWithBytes:bytes length:length];
    [SpsPpsdata setData:ByteHeader];
    [SpsPpsdata appendData:sps];
    [SpsPpsdata appendData:ByteHeader];
    [SpsPpsdata appendData:pps];
    
    
    
}
- (void)gotEncodedData:(NSData*)data isKeyFrame:(BOOL)isKeyFrame
{
    const char bytes[] = "\x00\x00\x00\x01";
    size_t length = (sizeof bytes) - 1; //string literals have implicit trailing '\0'
    NSMutableData *ByteHeader = [NSMutableData dataWithBytes:bytes length:length];
    NSMutableData *Bytedata = [[[NSMutableData alloc]init]autorelease];
    if (isKeyFrame)
    {
        [Bytedata setData:SpsPpsdata];
        [Bytedata appendData:ByteHeader];
        [Bytedata appendData:data];
    }
    else
    {
        [Bytedata setData:ByteHeader];
        [Bytedata appendData:data];
    }
  [self AddVideoData:(unsigned char*)[Bytedata bytes] nLen:(int)[Bytedata length] iskeyframe:isKeyFrame nWidth:_width nHeight:_height];
     
    
}


@end
