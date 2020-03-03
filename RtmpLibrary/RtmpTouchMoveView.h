#import <UIKit/UIKit.h>

@interface RtmpTouchMoveView : UIImageView
{
 
    
}
@property(nonatomic,assign)  bool nBeautifyFrontCamera ;
- (void)stopPreview;
- (void)startPreview;
- (void)reOrientation;
- (void)closeBeautify;
- (void)openBeautify;
- (void)sWitchCamera;
- (void)ChangeFrame;
@end
