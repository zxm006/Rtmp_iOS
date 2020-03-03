
#import "liveFFmpegdecode.h"
#include <pthread.h>
extern "C"
{
#import "libavformat/avformat.h"
    //#import "libswscale/swscale.h"
#include  "libavutil/imgutils.h"
}

#import <mach/mach_time.h>

@implementation liveFFmpegdecode

#define INBUF_OTHERVIDEOSIZE 0

- (id)init
{
    self = [super init];
    if (self)
    {
        
        _m_pUIImageView = nil;
        m_IsSetVidoeFrame = NO;
        
    }
    return self;
}

// Allocate RGB picture
-(UIImage *) imageFromAVPicture:(AVFrame *)pict  width:(int) width   height:(int )height;

{
    CGBitmapInfo bitmapInfo =kCGBitmapByteOrderDefault;
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,pict->data[0],pict->linesize[0]*height,kCFAllocatorNull);
    CGDataProviderRef provider =CGDataProviderCreateWithCFData(data);
    CGColorSpaceRef colorSpace =CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImage = CGImageCreate(width,height,
                                       8,24,pict->linesize[0],
                                       colorSpace,bitmapInfo,provider,NULL,NO,kCGRenderingIntentDefault);
    CGColorSpaceRelease(colorSpace);
    
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    CGImageRelease(cgImage);
    CGDataProviderRelease(provider);
    CFRelease(data);
    
    return image;
}

-(int) Beginmpeg_decode_h264
{
 av_register_all();
    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        return -1;
    }
    
    m_CodecContext= avcodec_alloc_context3(codec);
    if (!m_CodecContext) {
        
        return -1;
    }
    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        m_CodecContext->flags|= CODEC_FLAG_TRUNCATED;
    if (avcodec_open2(m_CodecContext, codec,0) < 0) {
        avcodec_close(m_CodecContext);
        avcodec_free_context(&m_CodecContext);
        
        return -1;
    }
    m_IsDisplayVideo = true;
    m_IsSetVidoeFrame = NO;
    return 0;
}

-(int) Endinmpeg_decode_h264
{
    m_IsDisplayVideo = false;
    m_IsSetVidoeFrame = false;
    if (m_CodecContext) {
        avcodec_close(m_CodecContext);
        avcodec_free_context(&m_CodecContext);
        m_CodecContext=NULL;
    }
    dispatch_sync(dispatch_get_main_queue(), ^{
        // 更UI
        if(_mivideoPlayView)
        {
            [_mivideoPlayView removeFromSuperview];
            [_mivideoPlayView release];
            _mivideoPlayView=nil;
        }
        
    });
    
    return 0;
}

-(bool)StartDisplayVideo
{
    m_IsDisplayVideo = true;
    return true;
}

-(void)StopDisplayVideo
{
    m_IsDisplayVideo = false;
}

double MachTimeToSecsdec(uint64_t time)

{
    mach_timebase_info_data_t timebase;
    
    mach_timebase_info(&timebase);
    
    return (double)time * (double)timebase.numer /
    
    (double)timebase.denom /1e9;
    
}

- (void)reSetVidoeFrame
{
//    dispatch_sync(dispatch_get_main_queue(), ^{
        if (_mivideoPlayView)
        {
            [_mivideoPlayView removeFromSuperview];
            [_mivideoPlayView release];
            _mivideoPlayView=nil;
        }
//    });
    
    m_IsSetVidoeFrame = NO;
    
}

-(int) ffmpeg_decode_h264:(unsigned char*) ucbuffer Len:(int) buffer_size  iCount:(int) iCount
{
   if(!m_IsDisplayVideo)
        return 0;
    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = ucbuffer ;
    avpkt.size = buffer_size;
  
    if(m_CodecContext==NULL)
    {
        av_packet_unref(&avpkt);
        return 0;
    }
    AVFrame* pictureFrame= av_frame_alloc();
    
    int got_picture = 0;
    
    if (avcodec_send_packet(m_CodecContext,&avpkt)==0)
    {
        
        got_picture= avcodec_receive_frame(m_CodecContext, pictureFrame);
    }
    else
    {
        if (pictureFrame) {
            av_frame_free(&pictureFrame);
            pictureFrame=NULL;
            
        }
        
        av_packet_unref(&avpkt);
        return 0;
    }
    if (got_picture==0)
    {
            int height = m_CodecContext->height;
            int width = m_CodecContext->width;
            
            if(width<=0||height<=0)
            {
                if (pictureFrame) {
                    av_frame_free(&pictureFrame);
                    pictureFrame=NULL;
                    
                }
                
                av_packet_unref(&avpkt);
                return 0;
            }
            
            int bufsize =     height *width * 3 / 2;
            
            char* buf =  (char*)malloc(bufsize);
            memset(buf, 0,bufsize);
            int a = 0, i;
            for (i = 0; i<height; i++)
            {
                memcpy(buf + a,pictureFrame->data[0] + i *pictureFrame->linesize[0], width);
                a += width;
            }
            for (i = 0; i<height / 2; i++)
            {
                
                memcpy(buf + a,pictureFrame->data[1] + i *pictureFrame->linesize[1], width / 2);
                a += width / 2;
            }
            for (i = 0; i<height / 2; i++)
            {
                
                memcpy(buf + a,pictureFrame->data[2] + i *pictureFrame->linesize[2], width / 2);
                a += width / 2;
            }
            
        
        
            if (!_mivideoPlayView&&!m_IsSetVidoeFrame)
            {
                dispatch_sync(dispatch_get_main_queue(), ^{
                    if (_mivideoPlayView)
                    {
                        [_mivideoPlayView removeFromSuperview];
                        [_mivideoPlayView release];
                        _mivideoPlayView=nil;
                    }
                    UIView *subview=(UIView*)_m_pUIImageView ;
                    if([subview isMemberOfClass:[UIView class]]||[subview isKindOfClass:[UIView class]])
                    {
                        UIWebView * tmpsubView=nil;
                        
                        for (UIView *tsubView in subview.subviews)
                        {
                            if (tsubView&&[tsubView isKindOfClass:[UIWebView class]])
                            {
                                tmpsubView = (UIWebView*)tsubView;
                                break;
                            }
                            usleep(0);
                        }
                        
                        CGRect frame = subview.frame;
                        
                        if(width>=frame.size.width)
                        {
                            int  w =  subview.bounds.size.height/height*width;
                            int oirx =(subview.bounds.origin.x+subview.bounds.size.width - w)/2;
                            frame.size.width = w;
                            frame.origin.x = oirx;
                            
                            [_mivideoPlayView setFrame:frame];
                        }
                        else
                        {
                            int  h =  subview.bounds.size.width/width*height;
                            int oiry =(subview.bounds.origin.y+subview.bounds.size.height - h)/2;
      
                            frame.size.height = h;
                            frame.origin.y = oiry;
                            [_mivideoPlayView setFrame:frame];
                            
                        }
                    
                        NSLog(@"subview width %f",subview.bounds.size.width);
                        _mivideoPlayView=[[iOSGLView alloc]initWithFrame:frame];
                        [_mivideoPlayView setBackgroundColor:[UIColor blackColor]];
                        _mivideoPlayView.userInteractionEnabled = NO;
                        if (!tmpsubView)
                        {
                            [subview insertSubview:_mivideoPlayView atIndex:0];
                            
                            NSLog(@"tmpsubView =nil ");
                        }
                        else
                        {
                            NSLog(@"tmpsubView !=nil ");
                            [subview insertSubview:_mivideoPlayView belowSubview:tmpsubView];
                        }
                        
                        [_mivideoPlayView startAnimation];
                    }
                });
                
                m_IsSetVidoeFrame = YES;
            }
            
            if(m_IsDisplayVideo&&_mivideoPlayView&&width>0&&height>0)
            {
              
                
                [_mivideoPlayView setBufferYUV:(const uint8_t*)buf width:width height:height];
                free(buf);
                buf=NULL;
            }
            
        }
   
    if (pictureFrame) {
        av_frame_free(&pictureFrame);
        pictureFrame=NULL;
        
    }
    av_packet_unref(&avpkt);
    return 0;
}



-(void)SetLocalVideoWindow:( void  *)pUIImageView
{
    _m_pUIImageView = (UIView*)pUIImageView;
    
}

UIImage* imageWithImage(UIImage* image ,CGSize newSize)
{
    // Create a graphics image context
    UIGraphicsBeginImageContext(newSize);
    // Tell the old image to draw in this new context, with the desired
    // new size
    [image drawInRect:CGRectMake(0,0,newSize.width,newSize.height)];
    // Get the new image from the context
    UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
    // End the context
    UIGraphicsEndImageContext();
    // Return the new image.
    return newImage;
}

-(void)dealloc
{
    
    
    self.m_pUIImageView = nil;
    
    //    if (picture_rgb) {
    //        av_frame_free(&picture_rgb);
    //        picture_rgb=NULL;
    //
    //    }
    //
    //    if (picture_rgb_buf) {
    //        free(picture_rgb_buf);
    //        picture_rgb_buf=NULL;
    //
    //    }
    //
    //
    //    if(m_pSwsCtxDst)
    //    {
    //        sws_freeContext(m_pSwsCtxDst);
    //        m_pSwsCtxDst=NULL;
    //    }
    
    usleep(500000);
    
    [super dealloc];
}

@end

