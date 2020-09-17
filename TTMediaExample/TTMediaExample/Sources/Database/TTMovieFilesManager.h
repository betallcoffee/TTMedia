//
//  TTMovieFilesManager.H
//  TTMedia
//
//  Created by liang on 7/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TTMovieFilesManager : NSObject

@property (nonatomic, strong, readonly) NSURL *movieDir;
@property (nonatomic, copy, readonly) NSArray<NSURL *> *movieURLList;

+ (instancetype)sharedInstance;

- (void)reloadMovieURLList;
- (NSURL *)newMovieURL;

@end
