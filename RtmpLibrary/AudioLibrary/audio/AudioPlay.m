//
//  OpenAlPlay.m
//  WatchCamera
//
//  Created by zhangxm on 13-3-27.
//
//


#import "AudioPlay.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
@implementation AudioPlay

//@synthesize mDevice,mContext,soundDictionary,bufferStorageArray;

#pragma make - openal function
-(void) LoudSpeaker:(int)bOpen
{
//     [ [AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
//     if (bOpen==1)
//    {
//        //扬声器播放模式
//       
//        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
//
//    }
//    else
//    {
//        //听筒播放模式
//        [ [AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
//        
//    }
//    
//       [[AVAudioSession sharedInstance] setActive:YES error:nil];
}



//处理监听触发事件
-(void)sensorStateChange:(NSNotificationCenter *)notification;
{
    
    [[UIDevice currentDevice] setProximityMonitoringEnabled:YES]; //建议在播放之前设置yes，播放结束设置NO，这个功能是开启红外感应
    
    //添加监听
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(sensorStateChange:)
                                                 name:@"UIDeviceProximityStateDidChangeNotification"
                                               object:nil];

    //如果此时手机靠近面部放在耳朵旁，那么声音将通过听筒输出，并将屏幕变暗（省电啊）
    if ([[UIDevice currentDevice] proximityState] == YES)
    {
        NSLog(@"Device is close to user");
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
        
    }
    else
    {
        NSLog(@"Device is not close to user");
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
    }
}


-(void)initOpenAL
{
//    
//    [[UIDevice currentDevice] setProximityMonitoringEnabled:YES]; //建议在播放之前设置yes，播放结束设置NO，这个功能是开启红外感应
//    
//    //添加监听
//    [[NSNotificationCenter defaultCenter] addObserver:self
//                                             selector:@selector(sensorStateChange:)
//                                                 name:@"UIDeviceProximityStateDidChangeNotification"
//                                               object:nil];

    
//    [self LoudSpeaker:0];
   
    slock= [[NSLock alloc] init];
    
//        dispatch_async(dispatch_get_main_queue(), ^{
      [[AVAudioSession sharedInstance] setActive:YES error:nil];
     [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
//          });
    

    mDevice=alcOpenDevice(NULL);
    if (mDevice) {
        mContext=alcCreateContext(mDevice, NULL);
        alcMakeContextCurrent(mContext);
    }
    
    alGenSources(1, &outSourceId);
    alSpeedOfSound(1.0);
    alDopplerVelocity(1.0);
    alDopplerFactor(1.0);
    alSourcef(outSourceId, AL_PITCH, 1.0f);
    alSourcef(outSourceId, AL_GAIN, 1.0f);//控制音量0-1.0
    alSourcei(outSourceId, AL_LOOPING, AL_FALSE);
    alSourcef(outSourceId, AL_SOURCE_TYPE, AL_STREAMING);
    
}


- (void) openAudioFromQueue:(unsigned char*)data dataSize:(UInt32)dataSize
{

    if (slock) {
        [slock lock];
    }
//    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    ALuint bufferID = 0;
    alGenBuffers(1, &bufferID); 
//    NSData * tmpData = [[[NSData  alloc ]initWithBytes:data length:dataSize]autorelease];
    int aSampleRate,aBit,aChannel;
    
    aSampleRate = 44100;//temp->sampleRates;//采样率
    aBit = 16;//temp->aBits;//位数
    aChannel = 1;//temp->Channels; 通道数
    ALenum format = 0;
    
    if (aBit == 8) {
        if (aChannel == 1)
            format = AL_FORMAT_MONO8;
        else if(aChannel == 2)
            format = AL_FORMAT_STEREO8;
        else if( alIsExtensionPresent( "AL_EXT_MCFORMATS" ) )
        {
            if( aChannel == 4 )
            {
                format = alGetEnumValue( "AL_FORMAT_QUAD8" );
            }
            if( aChannel == 6 )
            {
                format = alGetEnumValue( "AL_FORMAT_51CHN8" );
            }
        }
    }
    
    if( aBit == 16 )
    {
        if( aChannel == 1 )
        {
            format = AL_FORMAT_MONO16;
        }
        if( aChannel == 2 )
        {
            // NSLog(@"achhenl= 2!!!!!");
            format = AL_FORMAT_STEREO16;
        }
        if( alIsExtensionPresent( "AL_EXT_MCFORMATS" ) )
        {
            if( aChannel == 4 )
            {
                format = alGetEnumValue( "AL_FORMAT_QUAD16" );
            }
            if( aChannel == 6 )
            {
                NSLog(@"achannel = 6!!!!!!");
                format = alGetEnumValue( "AL_FORMAT_51CHN16" );
            }
        }
    }
    int queued;
    alGetSourcei(outSourceId , AL_BUFFERS_QUEUED , &queued);
   if(queued<50)
    {
        alBufferData(bufferID, format, data, (ALsizei)dataSize,aSampleRate);
        alSourceQueueBuffers(outSourceId, 1, &bufferID);
    }
    else{
//        NSLog(@"音频包多于==================================%d",queued);
    }

    [self updataQueueBuffer];
    
    ALint stateVaue;
    alGetSourcei(outSourceId, AL_SOURCE_STATE, &stateVaue);
//    [tmpData release];
//    [pool drain];
    if (slock) {
         [slock unlock];
    }
 
}

- (BOOL)updataQueueBuffer
{
    ALint stateVaue;
    int processed, queued;
    
    alGetSourcei(outSourceId, AL_BUFFERS_PROCESSED, &processed);
    alGetSourcei(outSourceId, AL_BUFFERS_QUEUED, &queued);
    alGetSourcei(outSourceId, AL_SOURCE_STATE, &stateVaue);
    
    if (stateVaue == AL_STOPPED ||
        stateVaue == AL_PAUSED ||
        stateVaue == AL_INITIAL)
    {
        if (queued < processed || queued == 0 ||(queued == 1 && processed ==1)) {
            NSLog(@"Audio Stop");
            [self stopSound];
//            [self cleanUpOpenAL];
        }
        
        [self playSound];
        return YES;
    }
    alGetSourcei(outSourceId , AL_BUFFERS_PROCESSED ,&processed);
    
    while(processed--)
    {
      
        if (outSourceId) {
            alSourceUnqueueBuffers(outSourceId ,1, & buffer);
            alDeleteBuffers(1 , &buffer);
        }
       else
       {
           break;
       }
        usleep(0);
    }
    
     alGetSourcei(outSourceId , AL_BUFFERS_QUEUED , &queued);
    
    while(queued--)
    {
        
        if (outSourceId) {
            alSourceUnqueueBuffers(outSourceId ,1 ,& buffer);
            alDeleteBuffers(1 , &buffer);
        }
        else
        {
            break;
        }
        usleep(0);
        
    }
//
//    alGetSourcei(outSourceId , AL_BUFFERS_QUEUED , &queued);
//    NSLog(@"===queue ===%d",queued);
    
    return YES;
}


#pragma make - play/stop/clean function
- (void)playSound
{
    alSourcePlay(outSourceId);
}

- (void)stopSound
{
    
    int processed =0;
    
    ALint  state;
    alGetSourcei(outSourceId, AL_SOURCE_STATE, &state);
    if (state != AL_STOPPED)
    {
        
        alSourceStop(outSourceId);
    }
    
    
    alGetSourcei(outSourceId, AL_BUFFERS_PROCESSED, &processed);
    
    while(processed--)
    {
        if (processed<0) {
            break;
        }
        if (outSourceId) {
            alSourceUnqueueBuffers(outSourceId ,1, & buffer);
            alDeleteBuffers(1 , &buffer);
        }
        
    }
    int queued =0;
    alGetSourcei(outSourceId , AL_BUFFERS_QUEUED , &queued);
    
    while(queued--)
    {
        if (queued<0) {
            break;
        }
        if (outSourceId) {
            alSourceUnqueueBuffers(outSourceId ,1 ,& buffer);
            alDeleteBuffers(1 , &buffer);
        }
        
    }
}
- (void)cleanUpOpenAL
{
    if (slock) {
         [slock lock];
    }
    
    [self stopSound];
    
    if(mContext)
    {
        alcDestroyContext(mContext);
        mContext=NULL;
    }
   
    if(mDevice)
    {
    alcCloseDevice(mDevice);
        mDevice=NULL;
    }
    if (slock) {
          [slock unlock];
    }
    
}


-(void)dealloc
{ 
    [self cleanUpOpenAL];
    
  if(slock)
  {
      [slock release];
      slock =nil;
  }
  [super dealloc];
}

@end
