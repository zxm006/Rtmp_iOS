//
//  OpenAlPlay.h
//  WatchCamera
//
//  Created by zhangxm on 13-3-27.
//
//
 
#import <Foundation/Foundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <OpenAL/oalMacOSX_OALExtensions.h>
@interface AudioPlay : NSObject
    {
        ALCcontext *mContext;
        ALCdevice *mDevice;
        ALuint outSourceId;
        NSMutableDictionary *soundDictionary;
        NSMutableArray *bufferStorageArray;
          ALuint buffer; 
        NSLock* slock;
    }
 
    -(void)LoudSpeaker:(int)bOpen;
    -(void)initOpenAL;
    -(void)openAudioFromQueue:(uint8_t *)data dataSize:(UInt32)dataSize;
    -(BOOL)updataQueueBuffer;
    -(void)playSound;
    -(void)stopSound;
    -(void)cleanUpOpenAL; 

    @end
