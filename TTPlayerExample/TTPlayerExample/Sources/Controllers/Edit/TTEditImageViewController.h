//
//  TTEditImageViewController.h
//  TTPlayerExample
//
//  Created by liang on 5/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "TTBaseViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface TTEditImageViewController : TTBaseViewController

@property (nonatomic, copy, readonly) NSArray<NSURL *> *urls;

- (instancetype)initWithURLs:(NSArray<NSURL *> *)urls;

@end

NS_ASSUME_NONNULL_END
