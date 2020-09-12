//
//  TTMovieListViewModel.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTMovieListViewModel.h"
#import "TTMovieItemViewModel.h"
#import "TTMovieFilesManager.h"

@interface TTMovieListViewModel ()
{
    NSMutableArray<NSURL *> *_movieList;
}

@end

@implementation TTMovieListViewModel

- (void)reload {
    _movieList = nil;
    [[TTMovieFilesManager sharedInstance] reloadMovieURLList];
}

- (TTMovieItemViewModel *)itemAtIndex:(NSUInteger)index {
    TTMovieItemViewModel *item = [TTMovieItemViewModel new];
    NSURL *url = [self.movieList objectAtIndex:index];
    NSLog(@"url: %@", url);
    [item configURL:url];
    return item;
}

- (void)removeItemAtIndex:(NSUInteger)index {
    NSLog(@"remove item at index: %u", index);
    NSURL *url = [self.movieList objectAtIndex:index];
    NSDictionary *attr = [[NSFileManager defaultManager] attributesOfItemAtPath:url.path error:nil];
    NSLog(@"url: %@", url);
    NSLog(@"attr: %@", attr);
    [[NSFileManager defaultManager] removeItemAtURL:url error:nil];
    [_movieList removeObjectAtIndex:index];
}

#pragma mark -
#pragma setter/getter

- (NSUInteger)total {
    return self.movieList.count;
}

- (NSArray<NSURL *> *)movieList {
    if (_movieList == nil) {
        _movieList = [NSMutableArray new];
        [_movieList addObjectsFromArray:[TTMovieFilesManager sharedInstance].movieURLList];
    }
    return _movieList;
}

@end
