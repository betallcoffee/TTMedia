//
//  TTPreviewCell.h
//  TTPlayerExample
//
//  Created by liang on 18/2/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TTPreviewCell : UICollectionViewCell

@property (nonatomic, readonly, strong) UILabel *label;

- (void)setupUI;

- (void)showFrame:(std::shared_ptr<TT::Frame>)frame;

@end
