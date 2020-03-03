
#import <UIKit/UIKit.h>
#import "CapScreen.h"
#import <AVFoundation/AVFoundation.h>
#import <QuartzCore/QuartzCore.h>
#import <AssetsLibrary/AssetsLibrary.h>
#import "H264HwEncoderImpl.h"
#import <mach/mach_time.h>




double MachTimeToSecs(uint64_t time)

{
  mach_timebase_info_data_t timebase;
 mach_timebase_info(&timebase);
 return (double)time * (double)timebase.numer /
 (double)timebase.denom /1e9;
 
}
//static struct SwsContext *encodeCapScreen_convert_ctx=NULL;
#define SCREEN_WIDTH ([UIScreen mainScreen].bounds.size.width)
#define SCREEN_HEIGHT ([UIScreen mainScreen].bounds.size.height)
@interface CapScreen()<H264HwEncoderImplDelegate>
{
    unsigned int m_uiFps;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    unsigned int m_resType;
    
    int isencode;
    H264HwEncoderImpl *h264Encoder;
    dispatch_queue_t _render_queue;
    CGSize _viewSize;
    CGFloat _scale;
    CGColorSpaceRef _rgbColorSpace;
    CVPixelBufferPoolRef _outputBufferPool;
}
@property (assign, nonatomic) CADisplayLink *displayLink;
@property (nonatomic) CFTimeInterval firstTimeStamp;
@property (nonatomic) BOOL isRecording;
@end

@implementation CapScreen

#pragma mark - initializers

+ (instancetype)sharedInstance {
    static dispatch_once_t once;
    static CapScreen *sharedInstance;
    dispatch_once(&once, ^{
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    if (_isRecording)
    {
        [self stopCapScreen];
        [self startCapScreen:m_resType];
    }
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _viewSize = [UIApplication sharedApplication].delegate.window.bounds.size;
        h264Encoder = [H264HwEncoderImpl alloc];
        SpsPpsdata=[[NSMutableData alloc]init];
        [h264Encoder initWithConfiguration];
      [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationDidBecomeActive:)
                                                     name:UIApplicationDidBecomeActiveNotification object:nil];
          _render_queue = dispatch_queue_create("Record.screen.queue", DISPATCH_QUEUE_SERIAL);
        dispatch_set_target_queue(_render_queue, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0));
        _scale = [UIScreen mainScreen].scale;
        // record half size resolution for retina iPads
        if ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) && _scale > 1) {
            _scale = 1.0;
        }
        
    }
    return self;
}

#pragma mark - public

-(void)SetResolution:(unsigned int)uiWidth uiHeight:(unsigned int)uiHeight
{
    //    m_uiWidth = uiWidth;
    //    m_uiHeight = uiHeight;
}

- (void)initCapScreen
{
    _rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    
    NSDictionary *bufferAttributes = @{(id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
                                       (id)kCVPixelBufferCGBitmapContextCompatibilityKey : @YES,
                                       (id)kCVPixelBufferWidthKey : @(_viewSize.width * _scale),
                                       (id)kCVPixelBufferHeightKey : @(_viewSize.height * _scale),
                                       (id)kCVPixelBufferBytesPerRowAlignmentKey : @(_viewSize.width * _scale * 4)
                                       };
    
    _outputBufferPool = NULL;
    CVPixelBufferPoolCreate(NULL, NULL, (__bridge CFDictionaryRef)(bufferAttributes), &_outputBufferPool);
}

- (BOOL)startCapScreen:(int)resType
{
    _isRecording = YES;
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    
    int bitrate =1000 * 1024;
    
    UIInterfaceOrientation  tion=  [[UIApplication sharedApplication] statusBarOrientation];
    
    if (tion != UIInterfaceOrientationLandscapeLeft&&tion != UIInterfaceOrientationLandscapeRight )
    {
        m_uiWidth   = 90 ;//  _viewSize.width-(   (long)_viewSize.width%8);
        m_uiHeight = 160;//  _viewSize.height-(   (long)_viewSize.height%8);
        
        
    }
    else
    {
        m_uiWidth   = 160 ;
        m_uiHeight = 90;
        
    }
    
    switch (resType) {
        case 0:
        {
            m_uiWidth   = m_uiWidth*2;
            m_uiHeight = m_uiHeight*2;
        }
            break;
        case 1:
        {
            m_uiWidth   = m_uiWidth*4 ;
            m_uiHeight = m_uiHeight*4;
        }
            break;
        case 2:
        {
            m_uiWidth   = m_uiWidth*6;
            m_uiHeight = m_uiHeight*6;
            bitrate=  bitrate*4;
        }
            break;
        default:
        {
            bitrate= bitrate*3;
        }
            break;
    }
    
    _viewSize.width=m_uiWidth;
    _viewSize.height=m_uiHeight;
    [self initCapScreen];
    
    [h264Encoder initEncode:m_uiWidth height:m_uiHeight framerate:20 bitrate:bitrate ];
     m_resType = resType;
    
    h264Encoder.delegate = self;
    
    if(_displayLink)
    {
        [_displayLink invalidate];
        _displayLink=nil;
    }
    
//    NSTimeInterval period = 0.5; //设置时间间隔
    
//    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
//    
//    dispatch_source_t _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
//    
//    dispatch_source_set_timer(_timer, dispatch_walltime(NULL, 0), period * NSEC_PER_SEC, 0); //每秒执行
//    
//    dispatch_source_set_event_handler(_timer, ^{
//        
//        dispatch_sync(dispatch_get_main_queue(), ^{
//            
//        //在这里执行事件
//        [self writeVideoFrame];
//        });
//    });
//    
//    dispatch_resume(_timer);
    
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(captureScreenshot)];
    _displayLink.frameInterval = 2;
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
    _isRecording=YES;
    
    return _isRecording;
}

- (void)stopCapScreen
{
    
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
    if (_isRecording) {
        _isRecording = NO;
        if (_displayLink) {
            [_displayLink invalidate];
            _displayLink=nil;
        }
       
        [self completeRecordingSession];
    }
    
    [h264Encoder End];
    
}


- (CGAffineTransform)videoTransformForDeviceOrientation
{
    CGAffineTransform videoTransform;
    switch ([UIDevice currentDevice].orientation) {
        case UIDeviceOrientationLandscapeLeft:
            videoTransform = CGAffineTransformMakeRotation(-M_PI_2);
            break;
        case UIDeviceOrientationLandscapeRight:
            videoTransform = CGAffineTransformMakeRotation(M_PI_2);
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            videoTransform = CGAffineTransformMakeRotation(M_PI);
            break;
        default:
            videoTransform = CGAffineTransformIdentity;
    }
    return videoTransform;
}

- (void)completeRecordingSession
{
    [self cleanup];
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
   if (isKeyFrame) {
        
        [Bytedata setData:SpsPpsdata];
        [Bytedata appendData:ByteHeader];
        [Bytedata appendData:data];
//        NSLog(@"KeyFrame == %lu",(unsigned long)[Bytedata length]);
    }
    else
    {
        [Bytedata setData:ByteHeader];
        [Bytedata appendData:data];
//         NSLog(@"p Frame == %lu",(unsigned long)[Bytedata length]);
        //        NSLog(@"isKeyFrame=no");
    }
    
    
    
    if(m_pVideoCaptureDataCallBack)
    {
        
        m_pVideoCaptureDataCallBack->On_MediaReceiverCallbackVideo((unsigned char*)[Bytedata bytes],[Bytedata length],isKeyFrame , m_uiWidth, m_uiHeight);
    }
    
}


-(void)SetCallBack:(VideoCaptureDataCallBack*)pVideoCaptureDataCallBack
{
    m_pVideoCaptureDataCallBack = pVideoCaptureDataCallBack;
}

- (void)cleanup
{
    
    self.firstTimeStamp = 0;
    CGColorSpaceRelease(_rgbColorSpace);
    CVPixelBufferPoolRelease(_outputBufferPool);
    
}
- (void)CapScreenOutput:(CVPixelBufferRef) pixelBufferRef
{
    if (h264Encoder) {
        [h264Encoder encode:pixelBufferRef];
        return;
    }
}

- (CVPixelBufferRef)pixelBufferFromCGImage:(CGImageRef)image{
    
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
                             [NSNumber numberWithBool:YES], kCVPixelBufferCGImageCompatibilityKey,
                             [NSNumber numberWithBool:YES], kCVPixelBufferCGBitmapContextCompatibilityKey,
                             nil];
    
    CVPixelBufferRef pxbuffer = NULL;
    
    CGFloat frameWidth = CGImageGetWidth(image);
    CGFloat frameHeight = CGImageGetHeight(image);
    
    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault,
                                          frameWidth,
                                          frameHeight,
                                          kCVPixelFormatType_32ARGB,
                                          (__bridge CFDictionaryRef) options,
                                          &pxbuffer);
    
    NSParameterAssert(status == kCVReturnSuccess && pxbuffer != NULL);
    
    CVPixelBufferLockBaseAddress(pxbuffer, 0);
    void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);
    NSParameterAssert(pxdata != NULL);
    
    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGContextRef context = CGBitmapContextCreate(pxdata,
                                                 frameWidth,
                                                 frameHeight,
                                                 8,
                                                 CVPixelBufferGetBytesPerRow(pxbuffer),
                                                 rgbColorSpace,
                                                 (CGBitmapInfo)kCGImageAlphaNoneSkipFirst);
    NSParameterAssert(context);
    CGContextConcatCTM(context, CGAffineTransformIdentity);
    CGContextDrawImage(context, CGRectMake(0,
                                           0,
                                           frameWidth,
                                           frameHeight),
                       image);
    CGColorSpaceRelease(rgbColorSpace);
    CGContextRelease(context);
    
    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);
    
    return pxbuffer;
}

- (UIImage *)capWindow: (UIWindow *) window
{
    CGSize capsize = window.bounds.size;
    if(m_uiWidth>m_uiHeight&&capsize.width<capsize.height)
    {
        int width=capsize.width;
        capsize.width=capsize.height;
        capsize.height =width;
   }
    else
    {
        [window snapshotViewAfterScreenUpdates:NO];
        
//        UIImage *snapshotImage = [window imageFromView:window];
    }
    
   UIGraphicsBeginImageContextWithOptions(capsize, YES,0);
    NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
                                [window methodSignatureForSelector:
                                 @selector(drawViewHierarchyInRect:afterScreenUpdates:)]];
    [invocation setTarget:window];
    [invocation setSelector:@selector(drawViewHierarchyInRect:afterScreenUpdates:)];
    CGRect arg2 = window.bounds;
    BOOL arg3 = NO;
    [invocation setArgument:&arg2 atIndex:2];
    [invocation setArgument:&arg3 atIndex:3];
    [invocation invoke];
    
    UIImage *screenshot = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
//    
//    uint64_t end = mach_absolute_time();
//    NSLog(@".bounds.size ==》%g s", MachTimeToSecs(end - begin));
//    
    return screenshot;
}


- (void)captureScreenshot {
 
    
    UIWindow * keyWindow = [[UIApplication sharedApplication] keyWindow];
    if([keyWindow isKindOfClass:NSClassFromString(@"_UIAlertControllerShimPresenterWindow")])
    {
        NSArray *windows = [[UIApplication sharedApplication] windows];
        UIImage *keyWindowimage = [self capWindow:keyWindow];
        UIImage *windows1 = [self capWindow:windows[0]];
        
        UIGraphicsBeginImageContext(_viewSize);
        [windows1 drawAtPoint:CGPointMake(0,0)];
        [keyWindowimage drawAtPoint:CGPointMake(0,0)];
        
        UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        
       dispatch_sync(_render_queue, ^{
            
              CVPixelBufferRef  image= [self pixelBufferFromCGImage:newImage.CGImage];
            [self CapScreenOutput:image];
                 CVPixelBufferRelease(image);
        });
              newImage= nil;
      
        
//    [newImage release];
      
    }
    else
    {      //            uint64_t begin = mach_absolute_time();
        dispatch_sync(_render_queue, ^{
        UIImage *  keyWindowimage = [self capWindow:keyWindow];
        CVPixelBufferRef  image= [self pixelBufferFromCGImage:keyWindowimage.CGImage];
        [self CapScreenOutput:image];
        CVPixelBufferRelease(image);
        keyWindowimage=nil;
        });
     }
    
}

- (void)captureScreenshot_old
{
    UIWindow * keyWindow = [[UIApplication sharedApplication] keyWindow];
     UIView *newView = [keyWindow snapshotViewAfterScreenUpdates:YES];
    UIGraphicsBeginImageContextWithOptions(newView.bounds.size, NO, 0)
    ;
    [newView drawViewHierarchyInRect:keyWindow.bounds afterScreenUpdates:YES];
    UIImage *copied = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
     dispatch_sync(_render_queue, ^{
        
        CVPixelBufferRef  image= [self pixelBufferFromCGImage:copied.CGImage];
        [self CapScreenOutput:image];
        CVPixelBufferRelease(image);
    });

    
//
//      dispatch_sync(_render_queue, ^{
//        
//        if (!self.firstTimeStamp) {
//            self.firstTimeStamp = _displayLink.timestamp;
//        }
//        
//        CVPixelBufferRef pixelBuffer = NULL;
//          CGContextRef bitmapContext =NULL;
//          
//  [self createPixelBufferAndBitmapContext:&pixelBuffer bitmapContext:&bitmapContext];
//        
//        dispatch_async(dispatch_get_main_queue(), ^{
//            
//            //            NSLog(@"scale= %.2f",[UIScreen mainScreen].scale);
//            
//            UIGraphicsPushContext(bitmapContext);
//            {
//                UIWindow * keywindow = [[UIApplication sharedApplication] keyWindow];
//                
//                if([keywindow isKindOfClass:NSClassFromString(@"_UIAlertControllerShimPresenterWindow")])
//                {
//                    NSArray *windows = [[UIApplication sharedApplication] windows];
//                    
//                    for (UIWindow *window  in windows)
//                    {
//                        [window drawViewHierarchyInRect:CGRectMake(0, 0, _viewSize.width, _viewSize.height) afterScreenUpdates:NO];
//                        
//                    }
//                    
//                }
//                else
//                {
//                    [keywindow drawViewHierarchyInRect:CGRectMake(0, 0, _viewSize.width, _viewSize.height) afterScreenUpdates:NO];
//                }
//                
//            }
//            
//            UIGraphicsPopContext();
//            
//        });
//        
//        [self CapScreenOutput:pixelBuffer];
//        CVPixelBufferRelease(pixelBuffer);
//          CGContextRelease(bitmapContext);
//        
//    });
    //
    //    uint64_t end = mach_absolute_time();
    //    NSLog(@"截图时间==》%g s", MachTimeToSecs(end - begin));
}

- (void)writeVideoFrame
{
 [self captureScreenshot];
   
}

- (void)createPixelBufferAndBitmapContext:(CVPixelBufferRef *)pixelBuffer bitmapContext:(CGContextRef *)bitmapContext
{
    CVPixelBufferPoolCreatePixelBuffer(NULL, _outputBufferPool, pixelBuffer);
    CVPixelBufferLockBaseAddress(*pixelBuffer, 0);
    
   * bitmapContext = CGBitmapContextCreate(CVPixelBufferGetBaseAddress(*pixelBuffer),
                                          CVPixelBufferGetWidth(*pixelBuffer),
                                          CVPixelBufferGetHeight(*pixelBuffer),
                                          8, CVPixelBufferGetBytesPerRow(*pixelBuffer), _rgbColorSpace,
                                          kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst
                                          );
    CGContextScaleCTM(   * bitmapContext, _scale, _scale);
    CGAffineTransform flipVertical =  CGAffineTransformMake(1, 0, 0, -1, 0, _viewSize.height);
    CGContextConcatCTM(   * bitmapContext, flipVertical);
    
    return ;
}

-(void)dealloc
{
    //    if (_append_pixelBuffer_queue) {
    //            dispatch_release(_append_pixelBuffer_queue);
    //        _append_pixelBuffer_queue=nil;
    //    }
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidBecomeActiveNotification object:nil];
    

    
    if (_render_queue) {
        dispatch_release(_render_queue);
        _render_queue=nil;
    }
    
    //
    //    if (_frameRenderingSemaphore) {
    //        dispatch_release(_frameRenderingSemaphore);
    //        _frameRenderingSemaphore=nil;
    //    }
    
    //    if (_pixelAppendSemaphore) {
    //        dispatch_release(_pixelAppendSemaphore);
    //        _pixelAppendSemaphore=nil;
    //    }
    
    [super dealloc];
}



@end
