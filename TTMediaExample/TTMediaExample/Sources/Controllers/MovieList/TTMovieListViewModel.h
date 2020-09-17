//
//  TTMovieListViewModel.h
//  TTMedia
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTBaseViewModel.h"

@class TTMovieItemViewModel;

@interface TTMovieListViewModel : TTBaseViewModel

@property (nonatomic, readonly) NSUInteger total;
@property (nonatomic, strong, readonly) NSArray<NSURL *> *movieList;

- (void)reload;

- (TTMovieItemViewModel *)itemAtIndex:(NSUInteger)index;

- (void)removeItemAtIndex:(NSUInteger)index;

@end
