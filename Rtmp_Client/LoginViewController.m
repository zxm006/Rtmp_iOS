
#import "LoginViewController.h"
#import <RtmpLibrary/RtmpLibrary.h>
//#import <VoiceTalk/AiYouVoiceManage.h>

//#import <LinkMic/LinkMic.h>

#import "TouchMovePanel.h"
TouchMovePanel *m_TouchMovelAIYOUPanel;

@implementation LoginViewController
@synthesize textip;
@synthesize ipserver;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
//    m_pOpenRtmpRemoteUser = NULL;
    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [textip resignFirstResponder];
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{  
    [super viewDidLoad];
    [UIApplication sharedApplication].idleTimerDisabled=YES;
//    self.AUDOIplayer= [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"star" ofType:@"mp3"]] error:nil];
//    self.AUDOIplayer.delegate = self;
//    //设置初始音量大小
//    // avAudioPlayer.volume = 1;
//    
//    //设置音乐播放次数  -1为一直循环
//    self.AUDOIplayer.numberOfLoops = -1;
//    
//    //预播放
//    [self.AUDOIplayer prepareToPlay];
//    [self.AUDOIplayer play];
    
    // Do any additional setup after loading the view from its nib.192.168.113.185
    
    [super viewDidLoad];
    
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    
  
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(handleDeviceOrientationDidChange:)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:nil
     ];

    
    [textip setText:@"rtmp://10.1.1.178/live/flytest"];
}


- (void)handleDeviceOrientationDidChange:(UIInterfaceOrientation)interfaceOrientation
{
    //1.获取 当前设备 实例
    UIDevice *device = [UIDevice currentDevice];
    
    /**
     *  2.取得当前Device的方向，Device的方向类型为Integer
     *
     *  必须调用beginGeneratingDeviceOrientationNotifications方法后，此orientation属性才有效，否则一直是0。orientation用于判断设备的朝向，与应用UI方向无关
     *
     *  @param device.orientation
     *
     */
        RtmpReOrientationMoveCamera();
    RtmpReOrientation();
    
    switch (device.orientation) {
        case UIDeviceOrientationFaceUp:
            NSLog(@"屏幕朝上平躺");
            break;
            
        case UIDeviceOrientationFaceDown:
            NSLog(@"屏幕朝下平躺");
            break;
            
            //系統無法判斷目前Device的方向，有可能是斜置
        case UIDeviceOrientationUnknown:
            NSLog(@"未知方向");
            break;
            
        case UIDeviceOrientationLandscapeLeft:
            NSLog(@"屏幕向左横置");
            break;
            
        case UIDeviceOrientationLandscapeRight:
            NSLog(@"屏幕向右橫置");
            break;
            
        case UIDeviceOrientationPortrait:
            NSLog(@"屏幕直立");
            break;
            
        case UIDeviceOrientationPortraitUpsideDown:
            NSLog(@"屏幕直立，上下顛倒");
            break;
            
        default:
            NSLog(@"无法辨识");
            break;
    }
    
}

void OpenTouchMovePanel(int x,int y,int width,int hight)
{
    UIViewController *rootvc = nil;
    
    UIWindow * window = [[UIApplication sharedApplication] keyWindow];
    if (window.windowLevel != UIWindowLevelNormal)
    {
        NSArray *windows = [[UIApplication sharedApplication] windows];
        for(UIWindow * tmpWin in windows)
        {
            if (tmpWin.windowLevel == UIWindowLevelNormal)
            {
                window = tmpWin;
                break;
            }
        }
    }
    
    UIView *frontView = [[window subviews] objectAtIndex:0];
    id nextResponder = [frontView nextResponder];
    
    if ([nextResponder isKindOfClass:[UIViewController class]])
        rootvc = nextResponder;
    else
        rootvc = window.rootViewController;
    
    if (m_TouchMovelAIYOUPanel) {
        [m_TouchMovelAIYOUPanel removeFromSuperview];
        m_TouchMovelAIYOUPanel = nil;
    }
    
    NSArray* nibView =  [[NSBundle mainBundle] loadNibNamed:@"TouchMovePanel" owner:nil options:nil];
    m_TouchMovelAIYOUPanel  =[nibView objectAtIndex:0];
    [m_TouchMovelAIYOUPanel setFrame:CGRectMake(x, y, width, hight)];
    
    [m_TouchMovelAIYOUPanel setBackgroundColor:[UIColor greenColor]];
    [ rootvc.view addSubview:m_TouchMovelAIYOUPanel];
    [m_TouchMovelAIYOUPanel setDisplay];
 
}

-(void) ainmation
{
}

-(IBAction)openBeautify:(id)Sender
{
    RtmpOpenBeautify();
}

-(IBAction)closeBeautify:(id)Sender
{
    RtmpCloseBeautify();
}

-(IBAction)btstartLive:(id)Sender
{
    InitRtmpLiveClass("","","");
    OpenTouchMovePanel(100,100,240,160 );
    initRTMPCallback(NULL,self);
     RtmpStartScreenLive("rtmp://192.168.1.8:1935/rtmplive/demo");
}
bool isfront =YES;

-(IBAction)btstopLive:(id)Sender
{
    RtmpStopWatch();
//    RtmpSwitchCamera();
}

-(IBAction)btLogin:(id)Sender
{
     InitRtmpLiveClass("","","");
      OpenTouchMovePanel(100,100,148,132 );
    RtmpOpenMoveCameraWithView(m_TouchMovelAIYOUPanel.cloudView);
   initRTMPCallback(NULL,self);

}

-(IBAction)btclose:(id)Sende
{
  RtmpStopCameraLive();
  RtmpStopWatch();
 
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES;
}
- (BOOL)shouldAutorotate
{
    RtmpReOrientationMoveCamera();
    
    return YES;
}
- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}


-(void)Alert:(NSString*) result message:(NSString*) message
{
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:result message:message
                                                   delegate:self cancelButtonTitle:@"确定" otherButtonTitles: nil];
    [alert show];
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}
@end
