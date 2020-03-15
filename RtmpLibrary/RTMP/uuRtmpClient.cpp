#include "uuRtmpClient.h"
#include <unistd.h>
#import <librtmp/rtmp.h>
#include <librtmp/log.h>
#include "AutoLock.h"
#include <stdlib.h>
#include <stdio.h>

#define FLV_CODECID_H264 7

char * put_byte( char *output, uint8_t nVal )
{
    output[0] = nVal;
    return output+1;
}
char * put_be16(char *output, uint16_t nVal )
{
    output[1] = nVal & 0xff;
    output[0] = nVal >> 8;
    return output+2;
}
char * put_be24(char *output,uint32_t nVal )
{
    output[2] = nVal & 0xff;
    output[1] = nVal >> 8;
    output[0] = nVal >> 16;
    return output+3;
}
char * put_be32(char *output, uint32_t nVal )
{
    output[3] = nVal & 0xff;
    output[2] = nVal >> 8;
    output[1] = nVal >> 16;
    output[0] = nVal >> 24;
    return output+4;
}
char * put_be64( char *output, uint64_t nVal )
{
    output=put_be32( output, nVal >> 32 );
    output=put_be32( output, nVal );
    return output;
}

char * put_amf_string( char *c, const char *str )
{
    uint16_t len = strlen( str );
    c=put_be16( c, len );
    memcpy(c,str,len);
    return c+len;
}

char * put_amf_double( char *c, double d )
{
    *c++ = AMF_NUMBER; /* type: Number */
    {
        unsigned char *ci, *co;
        ci = (unsigned char *)&d;
        co = (unsigned char *)c;
        co[0] = ci[7];
        co[1] = ci[6];
        co[2] = ci[5];
        co[3] = ci[4];
        co[4] = ci[3];
        co[5] = ci[2];
        co[6] = ci[1];
        co[7] = ci[0];
    }
    return c+8;
}

CRtmpClient::CRtmpClient(CRtmpPlayCall* pRtmpPlayCall)
:m_pRtmp(NULL)
,m_isSendSpsInfo(false)
,m_isSendSpecInfo(false)
,m_bIsPlay(false)
,m_ThreadRun(false)
,m_bIsStop(false)
,m_pRtmpPlayCall(pRtmpPlayCall)
{
    
}

CRtmpClient::~CRtmpClient()
{
    
}

BOOL CRtmpClient::InitSockets()
{
#ifdef WIN32
    WORD version;
    WSADATA wsaData;
    version = MAKEWORD(1, 1);
    return (WSAStartup(version, &wsaData) == 0);
#else
    return TRUE;
#endif
}

void CRtmpClient::CleanupSockets()
{
#ifdef WIN32
    WSACleanup();
#endif
}

BOOL CRtmpClient::ConnectServer(const char* url, bool bIsPlay)
{
    printf("ConnectServer");
    KAutoLock l(m_mKCritSec);
    m_bIsPlay = bIsPlay;
    
    InitSockets();
    RTMP_LogSetLevel(RTMP_LOGINFO);
    m_pRtmp = RTMP_Alloc();
    if(!m_pRtmp)
    {
        printf("m_pRtmp");
        return FALSE;
    }
    
    
    RTMP_Init(m_pRtmp);
    m_pRtmp->Link.timeout=15;
    
    RTMP_SetupURL(m_pRtmp,(char*)url);
    if(!m_bIsPlay)
        RTMP_EnableWrite(m_pRtmp);
    
    m_pRtmp->Link.lFlags|=RTMP_LF_LIVE;
    
    if(m_bIsPlay)
        RTMP_SetBufferMS(m_pRtmp, 7200*1000);
    
    if (!RTMP_Connect(m_pRtmp,NULL)){
        printf("RTMP_Connect FALSE");
        return FALSE;
    }
    
    if (!RTMP_ConnectStream(m_pRtmp, 0)){
        printf("RTMP_ConnectStream FALSE");
        return FALSE;
    }
    
    if(m_bIsPlay){
        StartThread();
        m_ThreadRun = true;
    }
    
    m_isSendSpecInfo = true;
    m_isSendSpsInfo = true;
    printf("RTMP_ConnectStream YES");
    return TRUE;
}

void CRtmpClient::DisconnectServer()
{
    if(m_bIsPlay){
        m_ThreadRun = false;
        printf("m_ThreadRun");
        WaitForStop();
        printf("WaitForStop");
    }
    if(m_pRtmp)
    {
//        KAutoLock l(m_mKCritSec);
        RTMP_DeleteStream(m_pRtmp);
        printf("RTMP_DeleteStream");
        
        RTMP_Close(m_pRtmp);
        RTMP_Free(m_pRtmp);
        m_pRtmp = NULL;
    }
    CleanupSockets();
}

double CRtmpClient::GetPlayTime()
{
    if(m_pRtmp)
    {
        return RTMP_GetDuration(m_pRtmp);
    }
    return 0.0;
}

BOOL CRtmpClient::SetPlaySeek(int nTime)
{
    if(m_pRtmp)
    {
        return RTMP_SendSeek(m_pRtmp, nTime);
    }
    return TRUE;
}

BOOL CRtmpClient::SetPlayAndStop(bool bPaly)
{
    if(m_pRtmp)
    {
        //m_bIsStop = !bPaly;
        RTMP_Pause(m_pRtmp, !bPaly);
        return TRUE;
    }
    return FALSE;
}

int
DumpMetaData(AMFObject *obj)
{
    AMFObjectProperty *prop;
    int n, len;
    for (n = 0; n < obj->o_num; n++)
    {
        char str[256] = "";
        prop = AMF_GetProp(obj, NULL, n);
        switch (prop->p_type)
        {
            case AMF_OBJECT:
            case AMF_ECMA_ARRAY:
            case AMF_STRICT_ARRAY:
                if (prop->p_name.av_len)
                    RTMP_Log(RTMP_LOGINFO, "%.*s:", prop->p_name.av_len, prop->p_name.av_val);
                DumpMetaData(&prop->p_vu.p_object);
                break;
            case AMF_NUMBER:
                snprintf(str, 255, "%.2f", prop->p_vu.p_number);
                break;
            case AMF_BOOLEAN:
                snprintf(str, 255, "%s",
                         prop->p_vu.p_number != 0. ? "TRUE" : "FALSE");
                break;
            case AMF_STRING:
                len = snprintf(str, 255, "%.*s", prop->p_vu.p_aval.av_len,
                               prop->p_vu.p_aval.av_val);
                if (len >= 1 && str[len-1] == '\n')
                    str[len-1] = '\0';
                break;
            case AMF_DATE:
                snprintf(str, 255, "timestamp:%.2f", prop->p_vu.p_number);
                break;
            default:
                snprintf(str, 255, "INVALID TYPE 0x%02x",
                         (unsigned char)prop->p_type);
        }
        if (str[0] && prop->p_name.av_len)
        {
            RTMP_Log(RTMP_LOGINFO, "  %-22.*s%s", prop->p_name.av_len,
                     prop->p_name.av_val, str);
        }
    }
    return FALSE;
}


int
HandleMetadata(RTMP *r, char *body, unsigned int len)
{
    /* allright we get some info here, so parse it and print it */
    /* also keep duration or filesize to make a nice progress bar */
    AMFObject obj;
    AVal metastring;
    int ret = FALSE;
    
    int nRes = AMF_Decode(&obj, body, len, FALSE);
    if (nRes < 0)
    {
        RTMP_Log(RTMP_LOGERROR, "%s, error decoding meta data packet", __FUNCTION__);
        return FALSE;
    }
    
    AMF_Dump(&obj);
    AMFProp_GetString(AMF_GetProp(&obj, NULL, 0), &metastring);
    
//    if (AVMATCH(&metastring, &av_onMetaData))
//    {
        AMFObjectProperty prop;
        /* Show metadata */
        RTMP_Log(RTMP_LOGINFO, "Metadata:");
        DumpMetaData(&obj);
    static const AVal av_duration = AVC("duration");
    
        if (RTMP_FindFirstMatchingProperty(&obj, &av_duration, &prop))
        {
            r->m_fDuration = prop.p_vu.p_number;
            /*RTMP_Log(RTMP_LOGDEBUG, "Set duration: %.2f", m_fDuration); */
        }
 
        ret = TRUE;
//    }
    AMF_Reset(&obj);
    return ret;
}


int first =0;


void CRtmpClient::ThreadProcMain(void)
{
   
    int sun =0;
    
     while (m_ThreadRun)
    {
        {
            KAutoLock l(m_mKCritSec);
            if(RTMP_IsConnected(m_pRtmp))
            {
                RTMPPacket packet = {0};
                int uRe = RTMP_GetNextMediaPacket(m_pRtmp, &packet);
//                    printf("RTMP_GetNextMediaPacket == %d \n\r",uRe);
                if(uRe == 0)
                {
                    sun ++;
                    
                    if (sun >2)
                    {
                        if(m_pRtmpPlayCall)
                        {
                            m_pRtmpPlayCall->Rtmp_PlayStop();
                            printf("m_pRtmpPlayCall->Rtmp_PlayStop() \n\r");
                            return;
                        }
                    }
                
                }
                
                if(uRe > 0)
                {
                    if(uRe == 2)
                    {
                        printf("Got Play.Complete or Play.Stop from server. "
                               "Assuming stream is complete");
                        if(m_pRtmpPlayCall)
                        {
                            m_pRtmpPlayCall->Rtmp_PlayStop();
                            return;
                        }
                    }
                    
                    sun = 0;
                    
                    if(packet.m_packetType == RTMP_PACKET_TYPE_INFO)
                    {
                    }
                    
                    else if(packet.m_packetType == RTMP_PACKET_TYPE_AUDIO)
                    {
                        char *body = packet.m_body;
                        if(m_pRtmpPlayCall && packet.m_body && packet.m_nBodySize > 0)
                        {
                            unsigned char uH1= (unsigned char)body[0];
                            unsigned char uH2 = (unsigned char)body[1];
                            if(uH1 == 0xAF && uH2 == 0x00)
                                m_pRtmpPlayCall->Rtmp_GetAudioData((unsigned char *)packet.m_body + 4, packet.m_nBodySize - 4, packet.m_nTimeStamp);
                            if(uH1 == 0xAF && uH2 == 0x01)
                                m_pRtmpPlayCall->Rtmp_GetAudioData((unsigned char *)packet.m_body + 2, packet.m_nBodySize - 2, packet.m_nTimeStamp);
                        }
                    }
                    else if(packet.m_packetType == RTMP_PACKET_TYPE_VIDEO)
                    {
                          m_pRtmpPlayCall->Rtmp_GetVideoData((unsigned char *)packet.m_body, packet.m_nBodySize, packet.m_nTimeStamp);
 
                    }
                    else if(packet.m_packetType == RTMP_PACKET_TYPE_FLASH_VIDEO)
                    {
                        if(m_pRtmpPlayCall)
                            m_pRtmpPlayCall->Rtmp_GetVideoData((unsigned char *)packet.m_body, packet.m_nBodySize, packet.m_nTimeStamp);
                    }
                    else
                    {
                        printf("RTMP Data .... \n");
                    }
                          RTMPPacket_Free(&packet);  
                }
            }
    
        }
        
        while(m_bIsStop && m_ThreadRun && m_bIsPlay)
        {
            usleep(1000);
        }
        
        usleep(1000);
    }
}


BOOL CRtmpClient::SendH264VideoPacket(unsigned char *pData, unsigned int nSize, unsigned int nWidth, unsigned int nHeight, unsigned int nFrameRate, bool bIsKeyFrame, unsigned int nTimeStamp)
{
    KAutoLock l(m_mKCritSec);
    if(!pData || nSize<=0 || m_pRtmp == NULL)
    {
        return FALSE;
    }
    
    if(m_isSendSpsInfo)
    {
        unsigned char *spsData = pData;
        unsigned int uDataSize = nSize;
        
        RTMPMetadata metaData;
        memset(&metaData,0,sizeof(RTMPMetadata));
        
        NaluUnit naluUnit;
        naluUnit.data = NULL;
        naluUnit.size = 0;
        naluUnit.type = 0;
        
        BOOL bre = GetNaluInfo(naluUnit, nSize, spsData);
        if(!bre)
            return FALSE;
        
        if(naluUnit.data == NULL || naluUnit.size <= 0 || naluUnit.size > 1024)
        {
            printf("SendH264VideoPacket  naluUnit.size=%d error",naluUnit.size);
            return FALSE;
        }
        
        metaData.nSpsLen = naluUnit.size;
        memcpy(metaData.Sps,naluUnit.data,naluUnit.size);
        
        metaData.nPpsLen = naluUnit.size;
        memcpy(metaData.Pps,naluUnit.data,naluUnit.size);
        
        metaData.nWidth = nWidth;
        metaData.nHeight = nHeight;
        metaData.nFrameRate = nFrameRate;
        
        SendH264Metadata(&metaData);
        m_isSendSpsInfo = false;
    }
    
    unsigned char *body = new unsigned char[nSize+9];
    
    int i = 0;
    if(bIsKeyFrame)
    {
        body[i++] = 0x17;// 1:Iframe 7:AVC
    }
    else
    {
        body[i++] = 0x27;// 2:Pframe 7:AVC
    }
    
    body[i++] = 0x01;// AVC NALU
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    
    // NALU size
    body[i++] = nSize>>24;
    body[i++] = nSize>>16;
    body[i++] = nSize>>8;
    body[i++] = nSize&0xff;
    
    // NALU data
    memcpy(&body[i],pData,nSize);
    
    bool bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+nSize,nTimeStamp);
    
    delete[] body;
    
    return bRet;
}

BOOL CRtmpClient::SendH264AudioPacket(unsigned char *pData, unsigned int nSize, unsigned int nTimeStamp)
{
    KAutoLock l(m_mKCritSec);
    if(!pData || nSize<=0 || m_pRtmp == NULL)
    {
        return FALSE;
    }
    
    if(m_isSendSpecInfo)
    {
        unsigned char *body = new unsigned char[nSize+9];
        int i = 0;
        /*AF 00 + AAC RAW data*/
        body[i++] = 0xAF;
        body[i++] = 0x00;
        body[i++] = 0x12;
        body[i++] = 0x08;
        
        memcpy(&body[i],pData,nSize);
        bool bRet = SendPacket(RTMP_PACKET_TYPE_AUDIO,body,i+nSize,0);
        delete[] body;
        m_isSendSpecInfo = false;
        return TRUE;
    }
    
    unsigned char *body = new unsigned char[nSize+9];
    int i = 0;
    /*AF 00 + AAC RAW data*/
    body[i++] = 0xAF;
    body[i++] = 0x01;
    
    memcpy(&body[i],pData,nSize);
    bool bRet = SendPacket(RTMP_PACKET_TYPE_AUDIO,body,i+nSize,nTimeStamp);
    delete[] body;
    
    return TRUE;
}

int CRtmpClient::SendPacket(unsigned int nPacketType,unsigned char *pData,long nSize,unsigned int nTimestamp)
{
    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet,nSize);
    
    packet.m_packetType = nPacketType;
    packet.m_nChannel = 0x04;
    packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet.m_nTimeStamp = nTimestamp;
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;
    packet.m_nBodySize = nSize;
    
    
    memcpy(packet.m_body,pData,nSize);
    
    int nRet = 0;
    if(RTMP_IsConnected(m_pRtmp))
    {
        nRet = RTMP_SendPacket(m_pRtmp,&packet,true);
    }
    RTMPPacket_Free(&packet);
    
    return nRet;
}

BOOL CRtmpClient::SendHAudioMetadata(LPRTMPMetadata lpMetaData)
{
    if(!lpMetaData){
        return FALSE;
    }
    
    char body[1024] = {0};
    char * p = (char *)body;
    
    p =put_amf_string( p, "audiodatarate");
    p =put_amf_double( p, lpMetaData->nAudioDatarate);
    p =put_amf_string( p, "audiosamplerate");
    p =put_amf_double( p, lpMetaData->nAudioSampleRate);
    
    p =put_amf_string( p, "audiosamplesize");
    p =put_amf_double( p, lpMetaData->nAudioSampleSize);
    
    p =put_amf_string( p, "stereo");
    p =put_amf_double( p, lpMetaData->nAudioChannels);
    
    p =put_amf_string( p, "audiocodecid");
    p =put_amf_double( p, lpMetaData->nAudioFmt);
    
    p =put_amf_string( p, "");
    p =put_byte( p, AMF_OBJECT_END);
    
    int index = p-body;
    return SendPacket(RTMP_PACKET_TYPE_INFO,(unsigned char*)body,p-body,0);
}

BOOL CRtmpClient::getH264Metadata(LPRTMPMetadata lpMetaData)
{
    return FALSE;
}

BOOL CRtmpClient::SendH264Metadata(LPRTMPMetadata lpMetaData)
{
    if(!lpMetaData){
        return FALSE;
    }
    
    char body[1024] = {0};
    
    char * p = (char *)body;
    p = put_byte(p, AMF_STRING );
    p = put_amf_string(p , "@setDataFrame" );
    
    p = put_byte( p, AMF_STRING );
    p = put_amf_string( p, "onMetaData" );
    
    p = put_byte(p, AMF_OBJECT );
    p = put_amf_string( p, "copyright" );
    p = put_byte(p, AMF_STRING );
    p = put_amf_string( p, "firehood" );
    
    p =put_amf_string( p, "width");
    p =put_amf_double( p, lpMetaData->nWidth);
    
    p =put_amf_string( p, "height");
    p =put_amf_double( p, lpMetaData->nHeight);
    
    p =put_amf_string( p, "framerate" );
    p =put_amf_double( p, lpMetaData->nFrameRate);
    
    p =put_amf_string( p, "videocodecid" );
    p =put_amf_double( p, 7);
    
    p =put_amf_string( p, "" );
    p =put_byte( p, AMF_OBJECT_END );
    
    int index = p-body;
    
    SendPacket(RTMP_PACKET_TYPE_INFO,(unsigned char*)body,p-body,0);
    
    int i = 0;
    body[i++] = 0x17; // 1:keyframe 7:AVC
    body[i++] = 0x00; // AVC sequence header
    
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00; // fill in 0;
    
    // AVCDecoderConfigurationRecord.
    body[i++] = 0x01; // configurationVersion
    body[i++] = lpMetaData->Sps[1]; // AVCProfileIndication
    body[i++] = lpMetaData->Sps[2]; // profile_compatibility
    body[i++] = lpMetaData->Sps[3]; // AVCLevelIndication
    body[i++] = 0xff; // lengthSizeMinusOne
    
    // sps nums
    body[i++] = 0xE1; //&0x1f
    // sps data length
    body[i++] = lpMetaData->nSpsLen>>8;
    body[i++] = lpMetaData->nSpsLen&0xff;
    // sps data
    memcpy(&body[i],lpMetaData->Sps,lpMetaData->nSpsLen);
    i= i+lpMetaData->nSpsLen;
    
    // pps nums
    body[i++] = 0x01; //&0x1f
    // pps data length
    body[i++] = lpMetaData->nPpsLen>>8;
    body[i++] = lpMetaData->nPpsLen&0xff;
    // sps data
    memcpy(&body[i],lpMetaData->Pps,lpMetaData->nPpsLen);
    i= i+lpMetaData->nPpsLen;
    
    return SendPacket(RTMP_PACKET_TYPE_VIDEO,(unsigned char*)body,i,0);
}


BOOL CRtmpClient::GetNaluInfo(NaluUnit &nalu , unsigned int nSize, unsigned char *pData)
{
    int i = 0;
    while(i<nSize-4)
    {
        if(pData[i++] == 0x00 &&
           pData[i++] == 0x00 &&
           pData[i++] == 0x00 &&
           pData[i++] == 0x01
           )
        {
            int pos = i;
            while (pos<nSize-4)
            {
                if(pData[pos++] == 0x00 &&
                   pData[pos++] == 0x00 &&
                   pData[pos++] == 0x00 &&
                   pData[pos++] == 0x01
                   )
                {
                    break;
                }
            }
            nalu.type = pData[i]&0x1f;
            nalu.size = (pos-4)-i;
            nalu.data = &pData[i];
            return TRUE;
        }
    }
    return FALSE;
}
