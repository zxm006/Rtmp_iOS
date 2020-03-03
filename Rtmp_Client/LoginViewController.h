#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <AVFoundation/AVAudioPlayer.h>

@interface LoginViewController : UIViewController<AVAudioPlayerDelegate>
{
    UITextField *textip;
    unsigned long           m_ulLocalVideoID ;			//VideoID
	unsigned long           m_ulLocalAudioID ;			//AudioID
	unsigned long           m_ulPeerVideoID ;			//VideoID
	unsigned long           m_ulPeerAudioID ;			//AudioID
    unsigned long           m_ulPeerUserID;
 
}
@property (nonatomic, strong)AVAudioPlayer *AUDOIplayer;
@property(retain,nonatomic)IBOutlet UITextField   *textip;
@property(retain,nonatomic)NSString               *ipserver;

@property(retain,nonatomic)IBOutlet UIView   *pwVideo;
@property(retain,nonatomic)IBOutlet UITextField   *liveurl;

@property (strong, nonatomic) CADisplayLink *mdisplayLink;
@property (strong, nonatomic)AVAudioPlayer *movePlayer;
 
@end
