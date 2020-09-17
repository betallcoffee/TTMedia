//
//  TTMovieItemViewModel.h
//  TTMedia
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTBaseViewModel.h"

typedef enum : NSUInteger {
    kItemTypeNone,
    kItemTypeImage,
    kItemTypeMovie,
} ItemType;

@interface TTMovieItemViewModel : TTBaseViewModel

@property (nonatomic, assign, readonly) ItemType type;
@property (nonatomic, assign, readonly) NSUInteger height;

@property (nonatomic, copy, readonly) NSString *name;
@property (nonatomic, copy, readonly) NSURL *url;
@property (nonatomic, assign, readonly) NSNumber *size;

- (void)configURL:(NSURL *)url;

@end
