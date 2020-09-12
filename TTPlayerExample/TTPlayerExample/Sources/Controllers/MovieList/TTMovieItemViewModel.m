//
//  TTMovieItemViewModel.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTMovieItemViewModel.h"

@interface TTMovieItemViewModel ()

@property (nonatomic, copy) NSURL *url;
@property (nonatomic, copy) NSDictionary *attr;

@end

@implementation TTMovieItemViewModel
@synthesize type = _type;

- (void)configURL:(NSURL *)url
{
    NSDictionary *attr = [[NSFileManager defaultManager] attributesOfItemAtPath:url.path error:nil];
    self.url = url;
    self.attr = attr;
    if ([self.url.pathExtension isEqualToString:@"png"]) {
        _type = kItemTypeImage;
    }
    NSLog(@"url: %@", url);
    NSLog(@"attr: %@", attr);
}

#pragma mark -
#pragma mark setter/getter

- (NSUInteger)height {
    return 44;
}

- (NSString *)name {
    return self.url.lastPathComponent;
}

- (NSURL *)url {
    return _url;
}

@end
