//
//  AudioDecodes.cpp
//  AudioCodes
//
//  Created by zhangxm on 12-12-12.
//  Copyright (c) 2012年 zhangxm. All rights reserved.
//

#include "AudioDecodes.h"

AudioDecodes::AudioDecodes()
{
     mAudioCodects=NULL;
     mAudioCodecContext=NULL;
     mAudioFrameDec=NULL;
     mAudioFrameOut=NULL;
         mSwrContext=NULL;
}

AudioDecodes::~AudioDecodes()
{
    this->unInitAudioDecode();
}


void  AudioDecodes::init_swr (SwrContext **swr_ctx)
{
    int ret;
    mSwrContext=swr_alloc();
    mSwrContext=swr_alloc_set_opts(mSwrContext,1,AV_SAMPLE_FMT_S16,44100,
                              mAudioCodecContext->channel_layout,mAudioCodecContext->sample_fmt,
                              44100,0,NULL);
    
    if((ret = swr_init(*swr_ctx)) < 0) {
        printf("Failed to initialize the resampling context\n");
    }
    
//    av_frame_unref(mAudioFrameOut);
    
    if(mAudioFrameOut)
    {
        av_frame_free(&mAudioFrameOut);
        mAudioFrameOut = NULL;
    }
    
    mAudioFrameOut=av_frame_alloc();
   
    
    mAudioFrameOut->nb_samples = (int)av_rescale_rnd(mAudioFrameDec->nb_samples, 44100,    44100, AV_ROUND_UP);
    mAudioFrameOut->channels=1;
    av_samples_alloc(mAudioFrameOut->data, &mAudioFrameOut->linesize[0],1,
                     mAudioFrameOut->nb_samples,AV_SAMPLE_FMT_S16, 0);
    
}

int AudioDecodes::initAudioDecode(CodeType codeType)
{
    mcodeType=codeType;
 
        av_register_all();
        
        mAudioCodects = avcodec_find_decoder(AV_CODEC_ID_AAC);
        if (!mAudioCodects) {
            return  -1;
        }
        
        mAudioCodecContext= avcodec_alloc_context3(mAudioCodects);
        mAudioCodecContext->channels = 1;
        mAudioCodecContext->sample_rate = 44100;
        mAudioCodecContext->bit_rate=16;
//        mAudioCodecContext->flags |= CODEC_FLAG_LOW_DELAY;
//        mAudioCodecContext->flags2 |= CODEC_FLAG2_FAST;
        if (avcodec_open2(mAudioCodecContext, mAudioCodects,0) < 0)
        {
            avcodec_close(mAudioCodecContext);
            av_free(mAudioCodecContext);
            return 0;
        }
        
        mAudioFrameDec=av_frame_alloc();
        if (!mAudioFrameDec) {
            avcodec_close(mAudioCodecContext);
            av_free(mAudioCodecContext);
            return 0;
        }
        
        mAudioFrameOut=av_frame_alloc();
        if (!mAudioFrameOut) {
            avcodec_close(mAudioCodecContext);
            av_free(mAudioCodecContext);
            av_free(mAudioFrameDec);
            return 0;
        }
  
    return 1;
    
}

int AudioDecodes::DecodeAudio(uint8_t*inBuffer, unsigned int inSize,char*outBuffer, unsigned int *outSize)
{
       if(mAudioCodecContext==NULL)
            return 0;
        AVPacket avpkt1;
        av_init_packet(&avpkt1);
        avpkt1.data = (uint8_t *)inBuffer;
        avpkt1.size = inSize;
        int len=0;
  
            int got_Audio=0;
         if (avcodec_send_packet(mAudioCodecContext,&avpkt1)==0) {
                
                got_Audio= avcodec_receive_frame(mAudioCodecContext, mAudioFrameDec);
            }
            else
            {
                got_Audio=-1;
            }
            
            if (got_Audio<0) {
                       av_packet_unref(&avpkt1);
                return  0;
            }
            if (got_Audio==0)
            {
                if(!mSwrContext)
                {
                    this->init_swr(&mSwrContext);
                }
             swr_convert(mSwrContext, mAudioFrameOut->data, mAudioFrameOut->nb_samples,
            (const uint8_t **)mAudioFrameDec->data, mAudioFrameDec->nb_samples);
 
                int data_size =av_samples_get_buffer_size(NULL,mAudioFrameOut->channels,mAudioFrameOut->nb_samples,AV_SAMPLE_FMT_S16, 0);
                if(data_size > 0)
                {
                    memcpy((uint8_t*)outBuffer+len, mAudioFrameOut->data[0], data_size);
                    len += data_size;
                    *outSize = len;
                }
            }
            av_packet_unref(&avpkt1);
    return 0;
}

int AudioDecodes::unInitAudioDecode()
{
            if(mAudioCodecContext)
        {
            avcodec_close(mAudioCodecContext);
            av_free(mAudioCodecContext);
       
            mAudioCodecContext = NULL;
           
        }
    
    if(mSwrContext)
    {
        swr_free(&mSwrContext);
        mSwrContext = NULL;
    }
    if(mAudioFrameDec)
    {
        av_frame_free(&mAudioFrameDec);
        mAudioFrameDec = NULL;
    }
    if(mAudioFrameOut)
    {
        av_frame_free(&mAudioFrameOut);
        mAudioFrameOut = NULL;
    }
 
    return 0;
}
