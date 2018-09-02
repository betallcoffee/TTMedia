//
//  TTMovieListViewController.h
//  TTPlayerExample
//
//  Created by liang on 7/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTBaseViewController.h"

@class TTMovieListViewModel;

@interface TTMovieListViewController : TTBaseViewController

@property (nonatomic, strong, readonly) TTMovieListViewModel *viewModel;

@end
