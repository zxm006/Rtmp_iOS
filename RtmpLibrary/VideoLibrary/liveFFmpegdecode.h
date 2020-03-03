#ifndef _FFMPEGDECODE_H_
#define _FFMPEGDECODE_H_
#import <UIKit/UIKit.h>
extern "C"
{
#import "libavformat/avformat.h"
#import "libavformat/avio.h"
}

#import "iOSGLView.h"
@protocol inoutDataDelegate;
 
@interface liveFFmpegdecode:NSObject 
{
 
    AVCodecContext *m_CodecContext;
   bool         m_IsDisplayVideo;
    iOSGLView *_mivideoPlayView;
     bool         m_IsSetVidoeFrame;
} 
 

@property(assign,nonatomic) UIView* m_pUIImageView;
-(int)Beginmpeg_decode_h264;
-(int)ffmpeg_decode_h264:(unsigned char*) ucbuffer Len:(int) buffer_size iCount:(int) iCount;
-(int)Endinmpeg_decode_h264;
-(UIImage *)imageFromAVPicture:(AVFrame *)pict  width:(int) width   height:(int )height;

-(void)reSetVidoeFrame;

-(void)SetLocalVideoWindow:(void *)pUIImageView;

-(bool)StartDisplayVideo;
-(void)StopDisplayVideo;

@end
 
#endif
