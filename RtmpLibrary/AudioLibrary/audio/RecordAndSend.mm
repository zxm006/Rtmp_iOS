//
//  audioClass.m
//  FFPlayer
//
//  Created by user on 12-2-8.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//


#import "RecordAndSend.h"
#import <AVFoundation/AVAudioSession.h>
#include <errno.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdlib.h>  
#include <string.h>  
#include <netinet/in.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include<fcntl.h> 

AudioCaptureDataCallBack* audioCaptureDataCallBack=NULL;
#define kBufferDurationSeconds 0.099999 * 5

#define SAMPLE_PATE  8000
#define WAVE_FRAME_SIZE			1600 //1920
//#define WAVE_COUNT              6
#define EMBEDDED_ARM 7
#define kNumberBuffers 1


AudioStreamBasicDescription		encodemRecordFormat;
AudioQueueRef	 encodemQueue=nil;
AudioQueueBufferRef			mBuffers[kNumberBuffers];
AudioQueueRef	 _recordingQueue=nil;
#define kNumQueueBuffers	1
AudioQueueBufferRef			recordingQueueBuffers[kNumQueueBuffers];

 
void  MyInputBufferHandler(	void *								inUserData,
                           AudioQueueRef						inAQ,
                           AudioQueueBufferRef					inBuffer,
                           const AudioTimeStamp *				inStartTime,
                           UInt32								inNumPackets,
                           const AudioStreamPacketDescription*	inPacketDesc);

  

void  MyInputBufferHandler(	void *								inUserData,
                           AudioQueueRef						inAQ,
                           AudioQueueBufferRef					inBuffer,
                           const AudioTimeStamp *				inStartTime,
                           UInt32								inNumPackets,
                           const AudioStreamPacketDescription*	inPacketDesc)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    RecordAndSend *aqr = (RecordAndSend *)inUserData;
    if (inNumPackets > 0) {
        
    }
    if (![aqr IsRunning])
    {
        [aqr StopRecord];
        if (encodemQueue) {
               AudioQueueStop(encodemQueue, NO);
        }
        [pool release];
        return;
    }
    
#if 0
   char buffer[MAX_UDP_PACK_SIZE];
    
    memset(buffer, 0, MAX_UDP_PACK_SIZE);
    char pSrcData[WAVE_FRAME_SIZE];
    memset(pSrcData, 0, WAVE_FRAME_SIZE);
    char*  pDstData = buffer;
    memcpy((char *)pSrcData, (char *)inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
     
    /* Silk Encoder */
    int sum=0;
    int renum=0;
    if (aqr.codetype==AMR_NBCODE) {
        for( int i=0;i<kNumberBuffers;i++)
        {
              aqr.audioCodes->CodeAudio((char*)(pSrcData + 320*i),320, (char*) (pDstData+13*i),&renum);
            
            sum+=renum;
        }
    }
    else if (aqr.codetype==GSM_CODE)
    {
        for( int i=0;i<kNumberBuffers;i++)
        {
             aqr.audioCodes->CodeAudio((char*)(pSrcData + 320*i),320, (char*) (pDstData+33*i),&renum );
            
             sum+=renum;
        }
    }
    
    else if (aqr.codetype==PCMU_CODE)
    {
         aqr.audioCodes->CodeAudio((char*)pSrcData ,1600, (char*) pDstData,&renum );
         sum+=renum;
    }
    else if (aqr.codetype==PCMA_CODE)
    {
        
        
        aqr.audioCodes->CodeAudio((char*)pSrcData ,1600, (char*) pDstData,&renum );
        
        
        sum+=renum;
    }
    
    if (aqr.codetype==SPEEX_NBCODE)
    {
        
        for( int i=0;i<kNumberBuffers;i++)
        {
            aqr.audioCodes->CodeAudio((char*)(pSrcData + 320*i),160, (char*) (pDstData+56*i),&renum );
            
            sum+=renum;
        }
        

    }
    else if (aqr.codetype==SILK_CODE)
    {
        aqr.audioCodes->CodeAudio((char*)pSrcData ,800, (char*) pDstData,&renum );
       /* do
        {
            if (pSrcData==NULL) {
                memset(pDstData, 0, WAVE_FRAME_SIZE);
                break;
            }
             int retval = SKP_Silk_SDK_Decode( g_psDec, &DecControl, 0, (unsigned char *)pSrcData, renum, (SKP_int16  *)pDstData, &len );
            if( retval )
            {
                printf( "\nSKP_Silk_SDK_Decode returned %d", retval );
            }
            
            outPtr  += len;
            tot_len += len;
           
        } while( DecControl.moreInternalDecoderFrames );
    }*/
    
        sum+=renum;
    }
   
       // [aqr SendWave:buffer len:sum];
    if (audioCaptureDataCallBack) {
        audioCaptureDataCallBack->On_MediaReceiverCallbackAudio((unsigned char*)buffer,sum);
        
    }
     memset(pSrcData, 0, WAVE_FRAME_SIZE);
     memset(buffer, 0, MAX_UDP_PACK_SIZE);
#endif
    
    if (audioCaptureDataCallBack) {
        audioCaptureDataCallBack->On_MediaReceiverCallbackAudio((unsigned char*)inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
    }
    
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL) ;
    
    [pool release];
    
}

@implementation RecordAndSend
@synthesize mIsRunning;
@synthesize codetype;

- (id)init
{
    if(self = [super init]){
        codetype=AMR_NBCODE;
        return self;
    }
    else
        return nil;
}

-(void)dealloc
{   
    if (encodemQueue){
         AudioQueueStop(_recordingQueue, YES);
		AudioQueueDispose(encodemQueue, true);
		encodemQueue = NULL;
	}
     [super dealloc];
}

-(void)setRecordCallBack: (AudioCaptureDataCallBack* )pAudioCaptureDataCallBack
{
    audioCaptureDataCallBack=pAudioCaptureDataCallBack;
}


-(void) SetupAudioEncodeFormat:(UInt32) inFormatID 
{
	memset(&encodemRecordFormat, 0, sizeof(encodemRecordFormat));
    encodemRecordFormat.mSampleRate=44100;
    encodemRecordFormat.mChannelsPerFrame=1;
	encodemRecordFormat.mFormatID = inFormatID;
	
    if (inFormatID == kAudioFormatLinearPCM)
	{
		// if we want pcm, default to signed 16-bit little-endian
		encodemRecordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
		encodemRecordFormat.mBitsPerChannel = 16;
		encodemRecordFormat.mBytesPerPacket = encodemRecordFormat.mBytesPerFrame = (encodemRecordFormat.mBitsPerChannel / 8) * encodemRecordFormat.mChannelsPerFrame;
		encodemRecordFormat.mFramesPerPacket = 1;
	}
    encodemRecordFormat.mReserved = 0;
}

-(int) ComputeRecordBufferSize:(const AudioStreamBasicDescription *)format  seconds:(float) seconds 
{
	int packets, frames, bytes = 0;
    frames = (int)ceil(seconds * format->mSampleRate);
    
    if (format->mBytesPerFrame > 0)
        bytes = frames * format->mBytesPerFrame;
    else {
        UInt32 maxPacketSize;
        if (format->mBytesPerPacket > 0)
            maxPacketSize = format->mBytesPerPacket;	// constant packet size
        else {
            UInt32 propertySize = sizeof(maxPacketSize);
            AudioQueueGetProperty(encodemQueue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize,
                                  &propertySize) ;
        }
        if (format->mFramesPerPacket > 0)
            packets = frames / format->mFramesPerPacket;
        else
            packets = frames;	// worst-case scenario: 1 frame in a packet
        if (packets == 0)		// sanity check
            packets = 1;
        bytes = packets * maxPacketSize;
    }
    
	return bytes;
}

-(bool) IsRunning 
{
    return self.mIsRunning;
}

void audioQueueInputCallback ( void *inUserData,                              AudioQueueRef inAQ,                              AudioQueueBufferRef inBuffer,                              const AudioTimeStamp *inStartTime,                              UInt32 inNumberPacketDescriptions,
                              
                              const AudioStreamPacketDescription *inPacketDescs ) {
    
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    RecordAndSend *aqr = (RecordAndSend *)inUserData;
  
    if (![aqr IsRunning])
    {
   
            return;
    }
    if (audioCaptureDataCallBack) {
        audioCaptureDataCallBack->On_MediaReceiverCallbackAudio((unsigned char*)inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
    }
    
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    [pool release];
}

-  (void) createRecordingQueue {
    
    OSStatus status = noErr;
    // create recording queue to produce AAC
    AudioStreamBasicDescription aacFormat = {0};
    aacFormat.mFormatID = kAudioFormatMPEG4AAC;
    aacFormat.mSampleRate = 44100;
    aacFormat.mChannelsPerFrame = 1;
    
    aacFormat.mFormatFlags = kMPEG4Object_AAC_LC;
    
    status = AudioQueueNewInput(&aacFormat,
                                audioQueueInputCallback,
                                (__bridge void *)(self),
                                NULL,
                                NULL,
                                0,
                                &_recordingQueue);
    if (status != noErr)
    NSLog(@"AudioUnit AudioQueueNewInput  %d!",(int)status);

    UInt32 maxPacketSize = 0;//
    UInt32 propSize = sizeof(maxPacketSize);//
    status = AudioQueueGetProperty(_recordingQueue,                                kAudioQueueProperty_MaximumOutputPacketSize,
                                   &maxPacketSize,
                                   &propSize);
     if (status != noErr)
    NSLog(@"AudioUnit kAudioQueueProperty_MaximumOutputPacketSize  %d!",(int)status);

    // enqueue empty buffers
    for (int i=0; i<kNumQueueBuffers; i++) {
        status = AudioQueueAllocateBuffer(_recordingQueue,                                          maxPacketSize,
                                          &recordingQueueBuffers[i]);
        if (status == noErr) {
            
            status = AudioQueueEnqueueBuffer(_recordingQueue,
                                             recordingQueueBuffers[i],                                             0,
                                             NULL);
            if (status != noErr)
                NSLog(@"AudioUnit AudioQueueEnqueueBuffer %d!",(int)status);  }
    }
    mIsRunning = true;
    status = AudioQueueStart(_recordingQueue,NULL);
    if (status != noErr)
        NSLog(@"AudioUnit AudioQueueStart %d!",(int)status);
}

-(bool)StartRecord
{
 AVAudioSession *session = [AVAudioSession sharedInstance];
       BOOL success;
    NSError* error;
    success = [session setCategory:AVAudioSessionCategoryPlayAndRecord error:&error];
        [session setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionPortOverrideSpeaker error:nil];
   [session requestRecordPermission:^(BOOL granted) {
        if (granted) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self createRecordingQueue];
            });
        }    }];

    return YES;
}

-(void) StopRecord
{
    mIsRunning = false;
     /* Free Encoder */
    if(audioCaptureDataCallBack)
        audioCaptureDataCallBack = NULL;
    if (_recordingQueue)
	{
        AudioQueueStop(_recordingQueue, YES);
		AudioQueueDispose(_recordingQueue, true);
		_recordingQueue = NULL;
	}
}

@end
