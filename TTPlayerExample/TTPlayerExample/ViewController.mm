//
//  ViewController.m
//  TTPlayerExample
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#include <memory>

#import "TTPlayer_ios.h"
#include "TTAudio.h"
#include "TTProcess.h"
#import "TTCapture.h"

#import "TTAVPlayerView.h"
#import "ViewController.h"
#import "TTMovieFilesManager.h"

using namespace TT;

@interface ViewController ()
{
    std::shared_ptr<Player> _player;
    std::shared_ptr<IOAudioUnit_ios> _audio;
    
    std::shared_ptr<Filter> _filter;
    std::shared_ptr<ContrastFilter> _contrast;
    
    TTImageView *_imageView;
}

@property (nonatomic, strong) TTOpenGLView *glView;
@property (nonatomic, strong) TTAVPlayerView *avplayerView;
@property (nonatomic, strong) UIButton *playButton;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    /**
     Session默认行为
     
     可以进行播放，但是不能进行录制。
     当用户将手机上的静音拨片拨到“静音”状态时，此时如果正在播放音频，那么播放内容会被静音。
     当用户按了手机的锁屏键或者手机自动锁屏了，此时如果正在播放音频，那么播放会静音并被暂停。
     如果你的App在开始播放的时候，此时QQ音乐等其他App正在播放，那么其他播放器会被静音并暂停。
     
     默认的行为相当于设置了Category为“AVAudioSessionCategorySoloAmbient”
     AVAudioSessionCategoryPlayAndRecord    否    默认不引起    既可以录音也可以播放
     AVAudioSessionCategoryPlayback    否    默认引起    只用于播放
     AVAudioSessionCategoryRecord    否    是    只用于录音
     */
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    
    [self setupFilter];
    [self setupPlayer];
    [self setupAudio];
    
    [self setupUI];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark -
#pragma mark setUp

- (void)setupUI {
    self.playButton.bounds = CGRectMake(0, 0, 44, 44);
    self.playButton.backgroundColor = [UIColor blueColor];
    self.playButton.center = self.view.center;
    [self.playButton addTarget:self action:@selector(onClickPlay:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.playButton];
}

- (void)setupFilter {
    _filter = std::make_shared<Filter>();
    
    _imageView = [TTImageView new];
    CGRect bounds = self.view.bounds;
    _imageView.frame = CGRectMake(0, 0, bounds.size.width, bounds.size.height - 64);
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:_imageView];
    _filter->addOutput([_imageView filter]);
}

- (void)setupPlayer {
    _player = createPlayer_ios();
    _player->bindFilter(_filter);
    //    bindGLView_ios(_player, self.glView);
    //    self.glView.frame = self.view.bounds;
    //    [self.view addSubview:self.glView];
    
    //    NSURL *fileURL = [[NSBundle mainBundle] URLForResource:@"video" withExtension:@"h264"];
    
    //    [self.view addSubview:self.playerView];
    //    self.avplayerView.frame = CGRectMake(0, 64, 320, 100);
    //    [self.avplayerView loadAssetFromFile:fileURL];
    //    for (int i = 0; i < 5; i++) {
    //        TTAVPlayerView *playerView = [[TTAVPlayerView alloc] init];
    //        [self.view addSubview:playerView];
    //        playerView.frame = CGRectMake(0, 100 * (i + 1), 320, 100);
    //        [playerView loadAssetFromFile:fileURL];
    //    }
}

- (void)setupAudio {
    _audio = std::make_shared<IOAudioUnit_ios>();
    _audio->setInputEnable(true);
    _audio->setup();
}

#pragma mark -
#pragma mark getter/setter

- (TTOpenGLView *)glView {
    if (_glView == nil) {
        _glView = [TTOpenGLView new];
    }
    return _glView;
}

- (TTAVPlayerView *)playerView {
    if (_avplayerView == nil) {
        _avplayerView = [TTAVPlayerView new];
    }
    return _avplayerView;
}

- (UIButton *)playButton {
    if (_playButton == nil) {
        _playButton = [UIButton new];
    }
    return _playButton;
}

#pragma mark -
#pragma mark button selector

- (void)onClickPlay:(UIButton *)button {
    if (_audio->start()) {
        return;
    }

    if (_player == nullptr) {
        return;
    }
    if (button.selected) {
        button.selected = NO;
        _player->stop();
    } else {
        button.selected = YES;
        NSString *filePath = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"mp4"];
//        NSString *filePath = [[NSBundle mainBundle] pathForResource:@"audio_HEv2" ofType:@"flv"];
        NSURL *movieDir = [TTMovieFilesManager sharedInstance].movieDir;
        NSURL *fileURL = [movieDir URLByAppendingPathComponent:@"liangliang3.mp4"];
        filePath = [fileURL absoluteString];
        const char *cFilePath = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
//        const char *cFilePath = "http://1251845595.vod2.myqcloud.com/9d67116cvodgzp1251845595/eb800eaf9031868223333695199/playlist.m3u8";
//        const char *cFilePath = "http://down.ttdtweb.com/test/MTV.mp4";
//        const char *cFilePath = "http://1251316161.vod2.myqcloud.com/918777bavodgzp1251316161/dd2ad6e85285890784107131626/cAAaxsH3F8UA.mp4";
        std::shared_ptr<URL> url = std::make_shared<URL>(cFilePath);
//        std::shared_ptr<URL> url = std::make_shared<URL>("rtmp://live.hkstv.hk.lxdns.com/live/hks1");
        _player->play(url);
    }
}

@end
