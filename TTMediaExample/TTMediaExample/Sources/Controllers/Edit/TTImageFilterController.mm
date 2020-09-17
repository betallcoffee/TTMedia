//
//  TTImageFilterController.m
//  TTMedia
//
//  Created by liang on 5/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#include "TTProcess.h"

#import "TTImageFilterController.h"
#import "TTPreviewCell.h"

static NSString *kPreviewCellIdentifier = @"ImageFilterPreviewCell";

@interface TTImageFilterController ()
< UICollectionViewDelegate
, UICollectionViewDataSource
, UICollectionViewDelegateFlowLayout
>
{
    TT_SP(TT::Frame) _frame;
}

@end

@implementation TTImageFilterController

#pragma mark -- UICollectionViewDataSource
//定义展示的UICollectionViewCell的个数
-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return 1;
}

//定义展示的Section的个数
-(NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}

//每个UICollectionView展示的内容
-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView
                 cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    TTPreviewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:kPreviewCellIdentifier
                                                                    forIndexPath:indexPath];
    
    cell.backgroundColor = [UIColor colorWithRed:((10 * indexPath.row) / 255.0)
                                           green:((20 * indexPath.row)/255.0)
                                            blue:((30 * indexPath.row)/255.0)
                                           alpha:1.0f];
    
    [cell setupUI];
    [cell showFrame:_frame];
    
    return cell;
}

#pragma mark --UICollectionViewDelegateFlowLayout
//定义每个UICollectionView 的大小
- (CGSize)collectionView:(UICollectionView *)collectionView
                  layout:(UICollectionViewLayout*)collectionViewLayout
  sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    return CGSizeMake(80, 80);
}

//定义每个UICollectionView 的 margin
-(UIEdgeInsets)collectionView:(UICollectionView *)collectionView
                       layout:(UICollectionViewLayout *)collectionViewLayout
       insetForSectionAtIndex:(NSInteger)section {
    return UIEdgeInsetsMake(5, 5, 5, 5);
}

#pragma mark --UICollectionViewDelegate
//返回这个UICollectionView是否可以被选择
-(BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

@end
