//
//  TTMovieFilterController.m
//  TTMedia
//
//  Created by liang on 5/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#import "easylogging++.h"
#import "TTEdit.h"
#import "TTFrame.hpp"
#import "TTRGBTextureFilter.hpp"

#import "TTMovieFilterController.h"
#import "TTPreviewCell.h"
#import "TTCGImagePicture.h"

static NSString *kPreviewCellIdentifier = @"MoviePreviewCell";

@interface TTMovieFilterController ()
< UICollectionViewDelegate
, UICollectionViewDataSource
, UICollectionViewDelegateFlowLayout
>
{
    TT_SP(TT::EditSpace) _editGroup;
    TT_SP(TT::RGBTextureFilter) _startFilter;
}

@end

@implementation TTMovieFilterController

#pragma mark UI
- (void)setupUI
{
    self.selectView.previewBar.delegate = self;
    self.selectView.previewBar.dataSource = self;
    [self.selectView.previewBar registerClass:[TTPreviewCell class] forCellWithReuseIdentifier:kPreviewCellIdentifier];
}

#pragma mark property
- (TTSlideSelectView *)selectView {
    if (_selectView == nil) {
        CGRect frame = CGRectMake(40, 0, UIScreen.mainScreen.bounds.size.width - 60, 100);
        _selectView = [[TTSlideSelectView alloc] initWithFrame:frame];
    }
    return _selectView;
}

#pragma mark Edit Material
- (void)addMaterial:(NSURL *)url {
    const char *str = [url.absoluteString cStringUsingEncoding:NSUTF8StringEncoding];
    TT_SP(TT::URL) ttUrl = TT_MK_SP(TT::URL)(str);
    TTCGImagePicture *picture = [TTCGImagePicture new];
    picture.picture->seturl(ttUrl);
    _editGroup->addMaterial(picture.picture);
    [self loadMoreForMaterial:picture.picture];
}

- (void)loadMoreForMaterial:(std::shared_ptr<TT::Material>)material {
    _editGroup->loadMoreForMaterial(material, [=](){
        [self reloadPreviewBar];
    });
}

- (void)reloadPreviewBar {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.selectView.previewBar reloadData];
    });
}

- (void)processFrame:(TT_SP(TT::Frame))frame
{
    _startFilter->processFrame(frame);
}

#pragma mark -- UICollectionViewDataSource
//定义展示的UICollectionViewCell的个数
-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    std::shared_ptr<TT::Material> material = _editGroup->material(static_cast<int>(section));
    if (material) {
        return material->frameCount();
    }
    return 0;
}

//定义展示的Section的个数
-(NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return _editGroup->materialCount();
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
    cell.label.text = [NSString stringWithFormat:@"%d", static_cast<int>(indexPath.row)];
    
    std::shared_ptr<TT::Material> material = _editGroup->material(static_cast<int>(indexPath.section));
    if(material) {
        [cell setupUI];
        
        std::shared_ptr<TT::Frame> frame = material->frameAt(static_cast<int>(indexPath.row));
        if (frame) {
            [cell showFrame:frame];
        }
        
        int count = material->frameCount();
        if (indexPath.row + 1 >= count
            && !material->isEnd()) {
            [self loadMoreForMaterial:material];
        }
    }
    
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
- (void)scrollViewDidScroll:(UIScrollView *)scrollView {
//    LOG(DEBUG) << "scrolling: " << scrollView.contentOffset.x;
    NSArray<NSIndexPath *> *indexPaths = self.selectView.previewBar.indexPathsForVisibleItems;
    if (indexPaths.count > 0) {
        NSIndexPath *indexPath = indexPaths.firstObject;
        std::shared_ptr<TT::Material> material = _editGroup->material(static_cast<int>(indexPath.section));
        if(material) {
            std::shared_ptr<TT::Frame> frame = material->frameAt(static_cast<int>(indexPath.row));
            [self processFrame:frame];
        }
    }
}

//UICollectionView被选中时调用的方法
-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    UICollectionViewCell *cell = (UICollectionViewCell *)[collectionView cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor whiteColor];
    
    std::shared_ptr<TT::Material> material = _editGroup->material(static_cast<int>(indexPath.section));
    if(material) {
        std::shared_ptr<TT::Frame> frame = material->frameAt(static_cast<int>(indexPath.row));
        [self processFrame:frame];
    }
    [collectionView reloadData];
}

//返回这个UICollectionView是否可以被选择
-(BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}


@end
