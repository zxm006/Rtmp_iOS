//
//  JLView.m
//  View的拖拽
//
//  Created by Mac on 15-3-29.
//  Copyright (c) 2015年 vxinyou. All rights reserved.
//

#import "RtmpTouchMoveView.h"

#import <AVFoundation/AVFoundation.h>
#define WIDTH self.view.frame.size.width
#define HEIGHT self.view.frame.size.height
#define kScreenWidth [[UIScreen mainScreen] bounds].size.width
#define kScreenHeight [[UIScreen mainScreen] bounds].size.height

#define kWidth  100
#define kHeight 100

#import "GPUImageBeautifyFilter.h"


@interface RtmpTouchMoveView ()<AVCaptureVideoDataOutputSampleBufferDelegate>
{
    GPUImageVideoCamera* m_pVideoCamera;
  
    GPUImageView* m_pFilterView;
    GPUImageBeautifyFilter *m_pBeautifyFilter;
}

@property(nonatomic,assign)	BOOL mFrontCamera;
@end

@implementation RtmpTouchMoveView

- (id)init
{
    if(self = [super init])
        {
            m_pVideoCamera = nil;
            m_pFilterView = nil;
            m_pBeautifyFilter = nil;
//            _nBeautifyFrontCamera = YES;
            _mFrontCamera = YES;
        }
        return self;
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event
{
      // 当前触摸点
      CGPoint center = self.center;
  
    if ( center.x<kWidth/2) {
        center.x =kWidth/2;
    }
    if ( center.y<kHeight/2) {
        center.y =kHeight/2;
    }
    
    if ( center.x>kScreenWidth-kWidth/2) {
        center.x =kScreenWidth-kWidth/2;
    }
    if ( center.y>kScreenHeight-kHeight/2) {
        center.y =kScreenHeight-kHeight/2;
    }
    // 修改当前view的中点(中点改变view的位置就会改变)
    self.center = center;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    
    UITouch *touch = [touches anyObject];
    
    // 当前触摸点
    CGPoint currentPoint = [touch locationInView:self.superview];
    // 上一个触摸点
    CGPoint previousPoint = [touch previousLocationInView:self.superview];
    
    // 当前view的中点
    CGPoint center = self.center;
    
    center.x += (currentPoint.x - previousPoint.x);
    center.y += (currentPoint.y - previousPoint.y);
    
  // 修改当前view的中点(中点改变view的位置就会改变)
    self.center = center;
    
    
}

- (void)ChangeFrame
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
//        m_pVideoCamera.outputImageOrientation =  [[UIApplication sharedApplication] statusBarOrientation];
        
        [m_pFilterView setFrame:self.frame];
    });
}

- (void)reOrientation
{
    dispatch_async(dispatch_get_main_queue(), ^{
        m_pVideoCamera.outputImageOrientation =  [[UIApplication sharedApplication] statusBarOrientation];
       
        [m_pFilterView setFrame:self.frame];
    });
}

- (void)  openBeautify
{
    [m_pVideoCamera removeAllTargets];
    GPUImageBeautifyFilter *beautifyFilter = [[GPUImageBeautifyFilter alloc] init];
    [m_pVideoCamera addTarget:beautifyFilter];
   [beautifyFilter addTarget:m_pFilterView];
 
}

- (void)sWitchCamera
{
  if(m_pVideoCamera)
  {
      [m_pVideoCamera rotateCamera];
  }
}

- (void)startPreview
{
//  if(_nBeautifyFrontCamera)//前置摄像头
//    {
        m_pVideoCamera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPreset352x288 cameraPosition:AVCaptureDevicePositionFront];
//    }
//    else//后置摄像头
//    {
//        m_pVideoCamera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPreset352x288 cameraPosition:AVCaptureDevicePositionBack];
//    }
    m_pVideoCamera.outputImageOrientation =  [[UIApplication sharedApplication] statusBarOrientation];
    m_pVideoCamera.horizontallyMirrorFrontFacingCamera = YES;
    m_pFilterView = [[GPUImageView alloc] initWithFrame:self.frame];
    m_pFilterView.center = self.center;
    m_pFilterView.fillMode = kGPUImageFillModePreserveAspectRatioAndFill;
    [self addSubview:m_pFilterView];
    [m_pVideoCamera addTarget:m_pFilterView];
    [self openBeautify];
    [m_pVideoCamera startCameraCapture];
}

- (void)  closeBeautify
{
    [m_pVideoCamera removeAllTargets];
    [m_pVideoCamera addTarget:m_pFilterView];
    
}

- (void)stopPreview
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
    
    [m_pBeautifyFilter removeAllTargets];
    m_pBeautifyFilter = nil;
}

@end
