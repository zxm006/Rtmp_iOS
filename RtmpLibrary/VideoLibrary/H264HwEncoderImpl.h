//
//  H264HwEncoderImpl.h
//  h264v1
//
//  Created by Ganvir, Manish on 3/31/15.
//  Copyright (c) 2015 Ganvir, Manish. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <AVFoundation/AVFoundation.h>

  

@protocol H264HwEncoderImplDelegate <NSObject>

- (void)gotSpsPps:(NSData*)sps pps:(NSData*)pps;
- (void)gotEncodedData:(NSData*)data isKeyFrame:(BOOL)isKeyFrame;

@end
@interface H264HwEncoderImpl : NSObject 

- (void) initWithConfiguration; 
- (void) initEncode:(int)width  height:(int)height  framerate:(int)fps bitrate:(int)bt;
- (void) changeResolution:(int)width  height:(int)height;
- (void) encode:(CVImageBufferRef )sampleBuffer;
- (void) End;


@property (assign, nonatomic) NSString *error;
@property (assign, nonatomic) id<H264HwEncoderImplDelegate> delegate;

@end
