//
//  AudioCodes.cpp
//  AudioCodes
//
//  Created by zhangxm on 12-12-12.
//  Copyright (c) 2012年 zhangxm. All rights reserved.
//

#include "AudioCodes.h"

//extern "C"
//{
//#include"gsm.h"
//#include "interf_enc.h"
//#include "g711.h"
//#import "SKP_Silk_SDK_API.h"
//}

/////////////
//SKP_int32 API_fs_Hz = SAMPLE_PATE;
//SKP_int32 max_internal_fs_Hz = 0;
//SKP_int32 targetRate_bps = 8000;
//SKP_int32 packetSize_ms = 100;
//SKP_int32 packetLoss_perc = 0;
//SKP_int32 complexity_mode = 0;
//SKP_int32 smplsSinceLastPacket=0;
//SKP_int32 INBandFEC_enabled = 0;
//SKP_int32 DTX_enabled = 0;

//SKP_SILK_SDK_EncControlStruct encControl; // Struct for input to encoder
/////////////////////////

#define SPEEX_BUFFER_MAX_SIZE		1024
#define SPEEX_DEFAULT_QUALITY		6
unsigned int enFrameSize=0;

AudioCodes::AudioCodes()
{
    
}

AudioCodes::~AudioCodes()
{
    
}

int AudioCodes::initAudioCode(CodeType codeType)
{
//    mcodeType=codeType;
//    if (mcodeType==AMR_NBCODE)
//    {
//        CodeHandle = Encoder_Interface_init(0);
//    }
//    else if (mcodeType==GSM_CODE)
//    {
//        CodeHandle=gsm_create(); ;
//    }
//    else if (mcodeType==PCMU_CODE)
//    {
//        
//    }
//    else if (mcodeType==PCMA_CODE)
//    {
//        
//    }
//    else if (mcodeType==SPEEX_NBCODE)
//    {
//#if 0
//        static int quality = SPEEX_DEFAULT_QUALITY;
//        CodeHandle=speex_encoder_init(&speex_nb_mode);
//        speex_encoder_ctl(CodeHandle, SPEEX_SET_QUALITY, &quality);
//        speex_encoder_ctl(CodeHandle, SPEEX_GET_FRAME_SIZE, &enFrameSize);
//        speex_bits_init(&speexEncoderBits);
//        speex_bits_reset(&speexEncoderBits);
//#endif
//    }
//    else if (mcodeType==SILK_CODE)
//    {
//        SKP_int32  ret=0;
//        SKP_int32 encSizeBytes=0;
//        ret = SKP_Silk_SDK_Get_Encoder_Size( &encSizeBytes );
//        if( ret )
//        {
//            printf( "\nSKP_Silk_create_encoder returned %d", ret );
//        }
//        CodeHandle = malloc( encSizeBytes );
//        ret = SKP_Silk_SDK_InitEncoder( CodeHandle, &encControl );
//        if( ret )
//        {
//            printf( "\nSKP_Silk_reset_encoder returned %d", ret );
//        }
//        encControl.API_sampleRate        = API_fs_Hz;
//        encControl.maxInternalSampleRate = 8000;		//max_internal_fs_Hz;
//        encControl.packetSize            = ( packetSize_ms * API_fs_Hz ) / 1000;
//        encControl.packetLossPercentage  = packetLoss_perc;
//        encControl.useInBandFEC          = INBandFEC_enabled;
//        encControl.useDTX                = DTX_enabled;
//        encControl.complexity            = complexity_mode;
//        encControl.bitRate               = ( targetRate_bps > 0 ? targetRate_bps : 0 );
//    }
    return 0;
}

int AudioCodes::CodeAudio(char*inBuffer,int inSize,char*outBuffer,int *outSize)
{
    if (mcodeType==AMR_NBCODE) {
        Encoder_Interface_Encode(CodeHandle, MR475, (short*)(inBuffer), (unsigned char*)(outBuffer), 0);
        *outSize=13;
    }
    else if(mcodeType==GSM_CODE)
    {
        gsm_encode((struct gsm_state*)CodeHandle, (gsm_signal *)inBuffer, (gsm_byte *)outBuffer);
        *outSize=33;
    }
    else if (mcodeType==PCMU_CODE)
    {
        int16_t* pin_data;
        unsigned int out_size;
       
        if(inBuffer==NULL)
        {
            return 0;
        }
        
        out_size = (inSize >> 1);
        *outSize=out_size;
        pin_data = (int16_t*)inBuffer;
        
        for( unsigned int i = 0; i<out_size; i++){
            outBuffer[i] = linear2ulaw(pin_data[i]);
        }
        return out_size;
    }
    else if (mcodeType==PCMA_CODE)
    {
        unsigned int i;
        int16_t* pin_data;
        unsigned int out_size;
        if(inBuffer==NULL)
        {
            return 0;
        }
        out_size = (inSize >> 1);
        *outSize=out_size;
       
        pin_data = (int16_t*)inBuffer;
        for(i = 0; i<out_size; i++){
            outBuffer[i] = linear2alaw(pin_data[i]);
        }
        return out_size;
    }
    else if (mcodeType==SPEEX_NBCODE)
    {
#if 0
        speex_bits_reset(&speexEncoderBits);
        speex_encode_int(CodeHandle, (spx_int16_t*)inBuffer, &speexEncoderBits);
        int out_size = speex_bits_write(&speexEncoderBits, outBuffer,enFrameSize/2);
        *outSize=out_size;
#endif
    }
    else if (mcodeType==SILK_CODE)
    {
//        SKP_int16 nBytes = 1600;
//        size_t counter =  ( packetSize_ms * API_fs_Hz ) / 1000;	//100 * 8000
//        /* Silk Encoder */
//        SKP_int32  ret= SKP_Silk_SDK_Encode( CodeHandle, &encControl, (short *)inBuffer, (SKP_int16)counter, (unsigned char *)outBuffer, &nBytes );
//        if (ret==0) {
//            *outSize=nBytes;
//        }
//        else
//        {
//             printf( "\nSKP_Silk_Encode error %d", nBytes );
//              *outSize=0;
//        }
    }
    return 0;
}

int AudioCodes::unInitAudioCode()
{
    if (mcodeType==AMR_NBCODE)
    {
        Encoder_Interface_exit(CodeHandle);
        CodeHandle=NULL;
    }
    else if(mcodeType==GSM_CODE)
    {
        if(CodeHandle)
        {
            gsm_destroy((struct gsm_state*)CodeHandle);
            CodeHandle=NULL;
        }
    }
    else if (mcodeType==PCMU_CODE)
    {
        
    }
    else if (mcodeType==PCMA_CODE)
    {
        
    }
    else if (mcodeType==SPEEX_NBCODE)
    {
        if(CodeHandle)
        {
			//speex_encoder_destroy(CodeHandle);
			CodeHandle = NULL;
		}
		//speex_bits_destroy(&speexEncoderBits);
		 
    }
    else if (mcodeType==SILK_CODE)
    {
        if (CodeHandle){
            free(CodeHandle);
            CodeHandle=NULL;
        }
    }
    
    return 0;
}