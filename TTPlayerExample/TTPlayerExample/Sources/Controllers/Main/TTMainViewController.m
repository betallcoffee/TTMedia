//
//  TTMainViewController.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTMainViewController.h"

#import "ViewController.h"
#import "TTMovieListViewController.h"
#import "TTCaptureViewController.h"
#import "TTEditListViewController.h"

@interface TTMainViewController ()
{
    ViewController *_playerVC;
    TTMovieListViewController *_movieListVC;
    TTEditListViewController *_editListVC;
    TTCaptureViewController *_captureVC;
}

@end

@implementation TTMainViewController

- (instancetype)init {
    self = [super init];
    if (self) {
        
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

#pragma mark --
#pragma mark UI
- (void)setupUI {
    [super setupUI];
    
//    self.interactive = YES;
    
    _playerVC = [ViewController new];
    _playerVC.tabBarItem.tag = 0;
    _playerVC.tabBarItem.title = @"播放";
    _playerVC.tabBarItem.titlePositionAdjustment = UIOffsetMake(0, -10);
    [_playerVC.tabBarItem setTitleTextAttributes:@{NSFontAttributeName:[UIFont boldSystemFontOfSize:18]}
                                           forState:UIControlStateNormal];
    
    _movieListVC = [TTMovieListViewController new];
    _movieListVC.tabBarItem.tag = 1;
    _movieListVC.tabBarItem.title = @"视频";
    _movieListVC.tabBarItem.titlePositionAdjustment = UIOffsetMake(0, -10);
    [_movieListVC.tabBarItem setTitleTextAttributes:@{NSFontAttributeName:[UIFont boldSystemFontOfSize:18]}
                                           forState:UIControlStateNormal];
    
    _editListVC = [TTEditListViewController new];
    UINavigationController *editNavVC = [[UINavigationController alloc] initWithRootViewController:_editListVC];
    _editListVC.tabBarItem.tag = 2;
    _editListVC.tabBarItem.title = @"编辑";
    _editListVC.tabBarItem.titlePositionAdjustment = UIOffsetMake(0, -10);
    [_editListVC.tabBarItem setTitleTextAttributes:@{NSFontAttributeName:[UIFont boldSystemFontOfSize:18]}
                                           forState:UIControlStateNormal];
    
    _captureVC = [TTCaptureViewController new];
    _captureVC.tabBarItem.tag = 3;
    _captureVC.tabBarItem.title = @"录制";
    _captureVC.tabBarItem.titlePositionAdjustment = UIOffsetMake(0, -10);
    [_captureVC.tabBarItem setTitleTextAttributes:@{NSFontAttributeName:[UIFont boldSystemFontOfSize:18]}
                                           forState:UIControlStateNormal];
    
    self.tabBar.items = @[_playerVC.tabBarItem, _movieListVC.tabBarItem, _editListVC.tabBarItem, _captureVC.tabBarItem];
    self.tabBar.selectedItem = _playerVC.tabBarItem;
    self.tabBar.itemPositioning = UITabBarItemPositioningFill;
    
    self.viewControllers = @[_playerVC, _movieListVC, editNavVC, _captureVC];

}

@end
