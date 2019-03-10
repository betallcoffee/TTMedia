//
//  TTFileManager.h
//  TTPlayerExample
//
//  Created by liang on 7/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TTFileManager : NSObject

@property (nonatomic, strong, readonly) NSURL *movieDir;
@property (nonatomic, copy) NSArray<NSURL *> *movieURLList;

+ (instancetype)sharedInstance;

- (void)reloadMovieURLList;
- (NSURL *)newMovieURL;

@end
