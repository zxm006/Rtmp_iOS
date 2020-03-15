extern "C"

#include "libavformat/avformat.h"
#include  "libavutil/imgutils.h"

#import <mach/mach_time.h>

#import "CameraHelp.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<stdio.h>
#include<unistd.h>
#import<AVFoundation/AVFoundation.h>

static bool isTakePhoto=NO;

static bool doing =NO;
AVCaptureDevice * g_CaptureDevice=nil;
AVCaptureDevicePosition g_DevicePosition = AVCaptureDevicePositionUnspecified;

NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

@interface CameraHelp (Private)


+(AVCaptureDevice *)cameraAtPosition:(AVCaptureDevicePosition)position;
-(void)startPreview;
-(void)stopPreview;
@end

@implementation CameraHelp  (Private)

+ (AVCaptureDevice *)cameraAtPosition:(AVCaptureDevicePosition)position{
    
    if(g_DevicePosition==position && g_CaptureDevice!=nil)
    {
        return g_CaptureDevice;
    }
    
    for (AVCaptureDevice *device in cameras){
        if (device.position == position){
            g_DevicePosition = position;
            g_CaptureDevice = device;
            return g_CaptureDevice;
        }
    }
    return [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
}

- (void)stopPreview{
    if(self.mCaptureSession){
        if([self.mCaptureSession isRunning]){
            [self.mCaptureSession stopRunning];
           // remove all sublayers
            if(mPreview){
                dispatch_async(dispatch_get_main_queue(), ^{
    
                
                for(CALayer *ly in mPreview.layer.sublayers){
                    if([ly isKindOfClass: [AVCaptureVideoPreviewLayer class]])
                    {
                        [ly removeFromSuperlayer];
                        
                        break;
                    }
                }
                });
            }
        }
    }
}

- (void)startPreview{
    if(self.mCaptureSession && mPreview && mStarted)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        AVCaptureVideoPreviewLayer* previewLayer = [[[AVCaptureVideoPreviewLayer alloc ]initWithSession: self.mCaptureSession]autorelease];
        
        previewLayer.frame = mPreview.bounds;
        previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
        
        [mPreview.layer addSublayer: previewLayer];
        });
        if(![self.mCaptureSession isRunning]){
            [self.mCaptureSession startRunning];
        }
    }
}

@end

static CameraHelp* g_camera = nil;

@implementation CameraHelp
@synthesize isoutPutBuffer;
@synthesize mStarted;

- (id)init
{
    if(g_camera)
        return g_camera;
    else
    {
        if(self = [super init])
        {
            self->mFps = 18;
            self->mFrontCamera = NO;
            mStarted = NO;
            g_camera = self;
            isoutPutBuffer=NO;
           _mCaptureDevice = nil;
            mStarted =NO;
            mPreview=nil;
            _mCaptureSession=nil;
 
            h264Encoder = [H264HwEncoderImpl alloc];
            SpsPpsdata=[[NSMutableData alloc]init];
            [h264Encoder initWithConfiguration];
        }
        return g_camera;
    }
}
-(void)initEncodeWith:(int)width hight:(int)hight
{
    
    self.mWidth=width;
    self.mHeight=hight;
    
}

-(void)setVideoDataOutputBuffer:(id<CameraHelpDelegate>)delegate
{
    if (delegate==nil)
    {
        isoutPutBuffer=NO;
    }
    else
    {
        isoutPutBuffer=YES;
    }
    
    self.outDelegate = delegate;
}

-(void)dealloc
{
    if (h264Encoder) {
        [h264Encoder End];
        [h264Encoder release];
        h264Encoder=nil;
          isinith264Encoder =NO;
    }
    if (mPreview) {
        mPreview=nil;
    }
    if(SpsPpsdata)
    {
        [SpsPpsdata release];
        SpsPpsdata =nil;
    }
 
    if(_mCaptureSession){
        if(_videoInput)
        {
        [_mCaptureSession removeInput:_videoInput];
            self.videoInput =nil;
            
        }
        if([_mCaptureSession isRunning])
        {
         [_mCaptureSession stopRunning];
        }
      
        [_mCaptureSession release];
        _mCaptureSession = nil;
        
    }
 
    [super dealloc];
    
}

+(CameraHelp*)shareCameraHelp
{
    if(!g_camera)
        g_camera = [[CameraHelp alloc] init];
    return g_camera;
}

+(void)closeCamera
{
    if(g_camera)
    {
        [g_camera dealloc];
        g_camera = nil;
    }
}

-(void)prepareVideoCapture:(int) resType  andFps: (int) fps andFrontCamera:(BOOL) bfront andPreview:(UIView*) view
{
    self.mresType = resType;
    self->mFps = fps;
    self->mFrontCamera = bfront;
    if(view)
        self->mPreview =  view  ;
    
    h264Encoder.delegate = self;
    
    if([_mCaptureSession isRunning])
    {
        [self stopVideoCapture];
        [self startVideoCapture];
    }
}

-(void)setPreview:(void*)preview{
    if(preview == nil)
    {
        [self stopPreview];
        mPreview = nil;
        [self startPreview];
    }
    else
    {
        mPreview = (UIView*)preview;
    }
}

-(void)startVideoCapture 
{
     NSLog(@"startVideoCapture");
    //防锁
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    
    if(_mCaptureDevice || _mCaptureSession)
    {
        NSLog(@"Already capturing");
        return;
    }
    
    if((_mCaptureDevice = [CameraHelp cameraAtPosition: AVCaptureDevicePositionFront]) == nil)
    {
        NSLog(@"Failed to get valide capture device");
        return;
    }
    
    NSError *error = nil;
    _videoInput = [AVCaptureDeviceInput deviceInputWithDevice:_mCaptureDevice error:&error];
    if (!_videoInput)
    {
        NSLog(@"Failed to get video input");
        self.mCaptureDevice = nil;
        return;
    }
    
   _mCaptureSession = [[AVCaptureSession alloc] init];
    if( _mresType ==0){
     _mCaptureSession.sessionPreset = AVCaptureSessionPreset352x288;
     }
     else if( _mresType ==1){
    _mCaptureSession.sessionPreset = AVCaptureSessionPreset640x480;//
     }
     else if( _mresType ==2){
     _mCaptureSession.sessionPreset = AVCaptureSessionPresetHigh;
     }
     else {
     _mCaptureSession.sessionPreset = AVCaptureSessionPreset640x480;
     }
    
    [_mCaptureSession addInput:_videoInput];
    AVCaptureVideoDataOutput*avCaptureVideoDataOutput =  [[[AVCaptureVideoDataOutput alloc] init]autorelease];
    
    avCaptureVideoDataOutput.videoSettings = [[[NSDictionary alloc] initWithObjectsAndKeys:
                                              [NSNumber numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange], kCVPixelBufferPixelFormatTypeKey,nil]autorelease];
    
    avCaptureVideoDataOutput.alwaysDiscardsLateVideoFrames = YES;
    [_mCaptureSession beginConfiguration];
    [_mCaptureDevice lockForConfiguration:&error];
    
    [_mCaptureDevice setActiveVideoMinFrameDuration:CMTimeMake(1, 12)];
    [_mCaptureDevice setActiveVideoMaxFrameDuration:CMTimeMake(1, 18)];
    
    [_mCaptureDevice unlockForConfiguration];
    [_mCaptureSession commitConfiguration];
    
    dispatch_queue_t queue = dispatch_queue_create("videoSession--ouput", NULL);
    [avCaptureVideoDataOutput setSampleBufferDelegate:self queue:queue];
    [_mCaptureSession addOutput:avCaptureVideoDataOutput];
    
    dispatch_release(queue);
 
    mStarted = YES;
    doing=NO;
    
    AVCaptureConnection *videoConnection = [avCaptureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    // SET THE ORIENTATION HERE -------------------------------------------------
    [videoConnection setVideoOrientation:AVCaptureVideoOrientationPortrait];
    
    
    if(![_mCaptureSession isRunning]){
        [_mCaptureSession startRunning];
    }
    [self startPreview];
}

-(void)stopVideoCapture
{
    if(_mCaptureSession)
    {
        if(_videoInput)
        {
            [_mCaptureSession removeInput:_videoInput];
            self.videoInput =nil;
        }
        if([_mCaptureSession isRunning])
        {
            [_mCaptureSession stopRunning];
        }
        
        [_mCaptureSession release];
        _mCaptureSession = nil;
        
    }
    if(self.mCaptureDevice){
        
        self.mCaptureDevice = nil;
    }
   	if(mPreview)
    {
        for (UIView *view in mPreview.subviews) {
            [view removeFromSuperview];
        }
    }
}

-(BOOL)setFrontCamera
{
    if(mFrontCamera)
        return YES;
    [self stopVideoCapture];
    mFrontCamera = YES;
    [self startVideoCapture];
    return YES;
}

-(BOOL)setBackCamera{
    if(!mFrontCamera)
        return YES;
    [self stopVideoCapture];
    mFrontCamera = NO;
    [self startVideoCapture];
    return YES;
}


-(void)TakePhoto
{
    isTakePhoto=YES;
}

-(UIImage *)imageFromAVPicture:(AVFrame *)pict  width:(int) width   height:(int )height

{
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, pict->data[0], pict->linesize[0]*height,kCFAllocatorNull);
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(data);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImage = CGImageCreate(width,
                                       height,
                                       8,
                                       24,
                                       pict->linesize[0],
                                       colorSpace,
                                       bitmapInfo,
                                       provider,
                                       NULL,
                                       NO,
                                       kCGRenderingIntentDefault);
    CGColorSpaceRelease(colorSpace);
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    CGImageRelease(cgImage);
    CGDataProviderRelease(provider);
    CFRelease(data);
    
    return image;
}

double MachTimeToSecsCH(uint64_t time)
{
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    return (double)time * (double)timebase.numer /
    (double)timebase.denom /1e9;
}

static  bool isinith264Encoder =NO;

-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    if  (h264Encoder)
    {
        CVImageBufferRef imageBuffer = (CVImageBufferRef)CMSampleBufferGetImageBuffer(sampleBuffer);
  
        if(!isinith264Encoder)
        {
        int nWidth=(int)CVPixelBufferGetWidth(imageBuffer);
        int nHeight= (int)CVPixelBufferGetHeight(imageBuffer);
        self.mWidth = nWidth;
        self.mHeight = nHeight;
            
            int bitrate =350 * 1024;
            
            switch (self.mresType) {
                case 0:
                    bitrate= bitrate;
                    break;
                case 1:
                    bitrate= bitrate*3;
                    break;
                case 2:
                    bitrate=  bitrate*6;
                    break;
                default:
                    bitrate= bitrate*3;
                    break;
            }
            [h264Encoder initEncode:nWidth height:nHeight  framerate:15 bitrate:bitrate];
            isinith264Encoder =YES;
        }
        //          begin =mach_absolute_time();
        [h264Encoder encode:imageBuffer];
    }
}

- (UIImage *) imageFromSampleBuffer:(CMSampleBufferRef) sampleBuffer
{
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGContextRef context = CGBitmapContextCreate(baseAddress, width, height, 8,
                                                 bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    CGImageRef quartzImage = CGBitmapContextCreateImage(context);
    CVPixelBufferUnlockBaseAddress(imageBuffer,0);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    UIImage *image = [UIImage imageWithCGImage:quartzImage];
    CGImageRelease(quartzImage);
    
    return (image);
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
    //    end = mach_absolute_time();
    //
    //    NSLog(@"编码时间==》%g s", MachTimeToSecs(end - begin));
    if (self.outDelegate ) {
            [self.outDelegate On_MediaReceiverCallbackVideo:(unsigned char*)[Bytedata bytes] nLen:[Bytedata length] bKeyFrame:isKeyFrame nWidth:_mWidth nHeight:_mHeight];
    }
}

@end
