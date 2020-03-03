#import "TouchMovePanel.h"
#import <AVFoundation/AVFoundation.h>

#define kScreenWidth [[UIScreen mainScreen] bounds].size.width
#define kScreenHeight [[UIScreen mainScreen] bounds].size.height



@interface TouchMovePanel ()<UIGestureRecognizerDelegate>
{
    int kWidth;
    int kHeight;
}
@property(strong,nonatomic)IBOutlet UIView *reddotView;
@property(strong,nonatomic)IBOutlet UILabel *reddotLabel;
@end

@implementation TouchMovePanel

-(void)setDisplay
{
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(click:)];
    [self.topCloudView addGestureRecognizer:tap];
    kWidth = self.frame.size.width;
    kHeight = self.frame.size.height;
}

- (id)init
{
    if(self = [super init])
    {
          kWidth = 60;
          kHeight = 60;
        
    }
    return self;
}


- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event
{
    
}

- (void)setbuttons:(NSArray*)array
{
    if (!array) {
        return;
    }
      self.mbuttons =[[NSMutableArray alloc]init];
    
    [self.mbuttons addObjectsFromArray:array];
    
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event
{
    CGPoint center = self.center;
    
    if ( center.x<kWidth/2) {
        center.x =kWidth/2;
    }
    if ( center.y<kHeight/2) {
        center.y =kHeight/2;
    }
    
    if ( center.x>kScreenWidth-kWidth/2) {
        center.x =kScreenWidth-kWidth/2;
    }
    if ( center.y>kScreenHeight-kHeight/2) {
        center.y =kScreenHeight-kHeight/2;
    }
    
    self.center = center;
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event
{
    // 当前触摸点
    CGPoint center = self.center;
    
    if ( center.x<kWidth/2) {
        center.x =kWidth/2;
    }
    if ( center.y<kHeight/2) {
        center.y =kHeight/2;
    }
    
    if ( center.x>kScreenWidth-kWidth/2) {
        center.x =kScreenWidth-kWidth/2;
    }
    if ( center.y>kScreenHeight-kHeight/2) {
        center.y =kScreenHeight-kHeight/2;
    }
    
    self.center = center;
    
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    
    // 当前触摸点
    CGPoint currentPoint = [touch locationInView:self.superview];
    // 上一个触摸点
    CGPoint previousPoint = [touch previousLocationInView:self.superview];
    
    // 当前view的中点
    CGPoint center = self.center;
    
    center.x += (currentPoint.x - previousPoint.x);
    center.y += (currentPoint.y - previousPoint.y);
    
    // 修改当前view的中点(中点改变view的位置就会改变)
    self.center = center;
}


- (void) setAIYOUPanelNumber:(int) buttontype  number:(int) number
{
   
    if (number>0)
    {
        [self.reddotView  setHidden:NO];
      [self.reddotLabel setText:[NSString stringWithFormat:@"%d",number]  ];
    }
    else
    {
        [self.reddotView  setHidden:YES];
       [self.reddotLabel setText:@""  ];
    }
}


- (void) showPanel:(NSArray *)btarrays
{
    
   
    return  ;
    
    
}

 

-(void)changeColor
{
    [UIView animateWithDuration:2.0 animations:^{
        self.alpha = 0.3;
    }];
}

-(void)click:(UITapGestureRecognizer*)t
{
    [self.buttonView setHidden:!self.buttonView.hidden];

}

- (void)clean
{
    [self removeFromSuperview];
}

@end
