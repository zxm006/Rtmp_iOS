 
#ifndef IOS_NGN_IOS_GLVIEW_H
#define IOS_NGN_IOS_GLVIEW_H

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h> 

@protocol iOSGLViewDelegate <NSObject>
@optional
-(void) glviewAnimationStarted;
-(void) glviewAnimationStopped;
-(void) glviewVideoSizeChanged;
-(void) glviewViewportSizeChanged;
@end

@interface iOSGLView : UIView {
    
}

-(void)setFps:(GLuint)fps;
-(void)startAnimation;
-(void)stopAnimation;
-(void)setOrientation:(UIDeviceOrientation)orientation  __attribute__ ((deprecated));
-(void)setBufferYUV:(const uint8_t*)buffer width:(uint)bufferWidth height:(uint)bufferHeight;
-(void)setDelegate:(id<iOSGLViewDelegate>)delegate;
-(void)setPAR:(int)numerator denominator:(int)denominator;
-(void)setFullscreen:(BOOL)fullscreen;
@property(nonatomic)bool isflushing;
@property(readonly) int viewportX;
@property(readonly) int viewportY;
@property(readonly) int viewportWidth;
@property(readonly) int viewportHeight;
@property(readonly) int videoWidth;
@property(readonly) int videoHeight;
@property(readonly) BOOL animating;
@end

#endif /* IOS_NGN_IOS_GLVIEW_H */
