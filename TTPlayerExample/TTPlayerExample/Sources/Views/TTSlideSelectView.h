//
//  TTSlideSelectView.h
//  TTPlayerExample
//
//  Created by liang on 1/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TTSlideSelectView;

@protocol TTSlideSelectViewDelegate <NSObject>

@optional
- (void)moveSlideSelectView:(TTSlideSelectView *)slideSelectView offset:(CGFloat)offset;
- (void)moveLeftInSlideSelectView:(TTSlideSelectView *)slideSelectView;
- (void)moveRightInSlideSelectView:(TTSlideSelectView *)slideSelectView;
- (void)endMoveInSlideSelectView:(TTSlideSelectView *)slideSelectView;

@end

@interface TTSlideSelectView : UIView

@property (nonatomic, weak) id<TTSlideSelectViewDelegate> delegate;
@property (nonatomic, strong, readonly) UICollectionView *previewBar;

@property (nonatomic, assign, readonly) NSUInteger leftIndex;
@property (nonatomic, assign, readonly) NSUInteger rightIndex;

@end
