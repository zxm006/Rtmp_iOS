#import <UIKit/UIKit.h>

@interface TouchMovePanel : UIView
{
    
}

@property(strong,nonatomic)NSMutableArray  *mbuttons;
@property(strong,nonatomic)IBOutlet UIImageView *cloudView;
@property(strong,nonatomic)IBOutlet UIView *topCloudView;
@property(strong,nonatomic)IBOutlet UIView *buttonView;
-(void)setDisplay;
- (void)setbuttons:(NSArray*)array;
@end
