//
//  AudioDecodes.h
//  AudioCodes
//
//  Created by zhangxm on 12-12-12.
//  Copyright (c) 2012年 zhangxm. All rights reserved.
//

#ifndef __AudioCodes__AudioDecodes__
#define __AudioCodes__AudioDecodes__
#include "AudioDefine.h"
#include <iostream>
extern "C"
{
#include  <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswresample/swresample.h>
#include"libavutil/opt.h"
}



class AudioDecodes
{
public:
	AudioDecodes();
    virtual ~AudioDecodes();
private:
    int mcodeType;
    void* DecodeHandle;
public:
    int DecodeAudio(uint8_t*inBuffer, unsigned int inSize,char*outBuffer, unsigned int *outSize);
    int initAudioDecode(CodeType codeType);
    int unInitAudioDecode();
    void init_swr (SwrContext **swr_ctx);
    
    AVCodec *mAudioCodects;
    AVCodecContext *mAudioCodecContext;
    AVFrame * mAudioFrameDec;
    AVFrame * mAudioFrameOut;
     SwrContext *mSwrContext;
};

#endif /* defined(__AudioCodes__AudioDecodes__) */
