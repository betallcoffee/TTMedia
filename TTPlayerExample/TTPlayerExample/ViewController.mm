//
//  ViewController.m
//  TTPlayerExample
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#include <memory>

#import "TTPlayer_ios.h"
#include "TTProcess.h"
#import "TTCapture.h"

#import "TTAVPlayerView.h"
#import "ViewController.h"
#import "TTFileManager.h"

using namespace TT;

@interface ViewController ()
{
    Player *_player;
    
    std::shared_ptr<FilterGroup> _filterGroup;
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
    
    [self setUpFilter];
    [self setUpPlayer];
    [self setUpUI];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark -
#pragma mark setUp

- (void)setUpUI {
    self.playButton.bounds = CGRectMake(0, 0, 44, 44);
    self.playButton.backgroundColor = [UIColor blueColor];
    self.playButton.center = self.view.center;
    [self.playButton addTarget:self action:@selector(onClickPlay:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.playButton];
}

- (void)setUpFilter {
    _filterGroup = std::make_shared<FilterGroup>();
    
    _imageView = [TTImageView new];
    CGRect bounds = self.view.bounds;
    _imageView.frame = CGRectMake(0, 0, bounds.size.width, bounds.size.height - 64);
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:_imageView];
    _filterGroup->addFilter([_imageView filter]);
}

- (void)setUpPlayer {
    _player = createPlayer_ios();
    _player->bindFilter(_filterGroup);
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
//        const char *cFilePath = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
//        const char *cFilePath = "http://1251845595.vod2.myqcloud.com/9d67116cvodgzp1251845595/eb800eaf9031868223333695199/playlist.m3u8";
        const char *cFilePath = "http://down.ttdtweb.com/test/MTV.mp4";
        std::shared_ptr<URL> url = std::make_shared<URL>(cFilePath);
//        std::shared_ptr<URL> url = std::make_shared<URL>("rtmp://live.hkstv.hk.lxdns.com/live/hks");
        
        _player->play(url);
    }
}

@end
