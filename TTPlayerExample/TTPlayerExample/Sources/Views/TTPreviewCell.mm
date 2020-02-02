//
//  TTPreviewCell.m
//  TTPlayerExample
//
//  Created by liang on 18/2/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#import "Masonry.h"

#include "TTProcess.h"
#import "TTPreviewCell.h"

@interface TTPreviewCell()
{
    TTImageView *_imageView;
    std::shared_ptr<TT::Y420ToRGBFilter> _filterTexture;
}

@property (nonatomic, strong, readwrite) UILabel *label;
@end

@implementation TTPreviewCell

- (void)setupUI {
    self.backgroundColor = [UIColor redColor];
    if (_imageView == nil) {
        _imageView = [TTImageView new];
        _imageView.contentMode = UIViewContentModeScaleAspectFit;
        [self.contentView addSubview:_imageView];
        [_imageView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.edges.equalTo(_imageView.superview);
        }];
    }

    if (_filterTexture == nil) {
        _filterTexture = std::make_shared<TT::Y420ToRGBFilter>();
        _filterTexture->addFilter([_imageView filter]);
    }

    if (_label == nil) {
        _label = [UILabel new];
        _label.backgroundColor = UIColor.clearColor;
        _label.textColor = UIColor.blueColor;
        [self.contentView addSubview:_label];
        [_label mas_makeConstraints:^(MASConstraintMaker *make) {
            make.edges.equalTo(_label.superview);
        }];
    }
}

- (void)showFrame:(std::shared_ptr<TT::Frame>)frame {
//    _filterTexture->processFrame(frame);
}

@end
