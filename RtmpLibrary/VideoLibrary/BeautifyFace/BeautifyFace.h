
#import <UIKit/UIKit.h>
 
#import "GPUImageBeautifyFilter.h"
#include "Thread.h"
#include <list>
#import "H264HwEncoderImpl.h"

class VideoCaptureEncod;
@class GPUImageBeautifyFilter;

@interface BeautifyFace: NSObject <H264HwEncoderImplDelegate>
{
    GPUImageVideoCamera* m_pVideoCamera;
    UIView* m_PreView;
    GPUImageView* m_pFilterView;
   GPUImageBeautifyFilter *m_pBeautifyFilter;
   VideoCaptureEncod *m_VideoCaptureEncod;
        H264HwEncoderImpl *h264Encoder;
    NSMutableData *  SpsPpsdata;
    bool nBeautifyFrontCamera;
    
    bool isinith264Encoder;
    
}

@property (assign,nonatomic)int width;
@property (assign,nonatomic)int height;
 -(bool) StartVideoCapture;
 -(void) StopVideoCapture ;
 -(void)   openBeautify;
 -(void)  closeBeautify;
 - (void)  switchCamera;

 -(void) reOrientation;
//设置前置摄像头
 -(bool) SetFrontCamera;
//设置后置摄像头
 -(bool) SetBackCamera;
 -(void) setVideoDataOutputBuffer:(VideoCaptureEncod*)videoCaptureEncod;
 -(void) SetPreview:(UIView*) preview;
 -(bool) AddVideoData:(unsigned char*)pData nLen:(int) nLen iskeyframe:(bool) iskeyframe  nWidth:(int )nWidth  nHeight:(int) nHeight;

 @end

