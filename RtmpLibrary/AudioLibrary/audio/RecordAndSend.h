//
//  RecordAndSend.h
//  FFPlayer
//
//  Created by user on 12-2-8.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

#include "AudioLibrary.h"
#include "AudioDefine.h"
@interface RecordAndSend : NSObject
{
    bool mIsRunning;
    CodeType codetype;
}

@property bool mIsRunning;
@property CodeType codetype;

 //录制的函数
-(bool) IsRunning;
-(bool) StartRecord;
-(void) SetupAudioEncodeFormat:(UInt32) inFormatID;
-(int) ComputeRecordBufferSize:(const AudioStreamBasicDescription *)formatmp  seconds:(float) seconds;
-(void) StopRecord;
-(void) setRecordCallBack:(AudioCaptureDataCallBack* )pAudioCaptureDataCallBack;
-(int) InitEncode;
@end
