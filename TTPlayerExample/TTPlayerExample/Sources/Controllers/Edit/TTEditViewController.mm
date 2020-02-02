//
//  TTEditViewController.m
//  TTPlayerExample
//
//  Created by liang on 21/1/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"
#include "TTEdit.h"
#include "TTProcess.h"

#import "TTCapture.h"

#import "TTEditViewController.h"
#import "TTPreviewCell.h"
#import "TTSlideSelectView.h"

static NSString *kPreviewCellIdentifier = @"previewCell";

@interface TTEditViewController ()
< UICollectionViewDelegate
, UICollectionViewDataSource
, UICollectionViewDelegateFlowLayout
>
{
    std::shared_ptr<TT::EditSpace> _editGroup;
    
    std::shared_ptr<TT::FilterGroup> _filterGroup;
    std::shared_ptr<TT::Y420ToRGBFilter> _filterTexture;
}

@property (nonatomic, strong) TTSlideSelectView *selectView;
@property (nonatomic, strong) UICollectionView *previewBar;
@property (nonatomic, strong) TTImageView *imageView;

@end

@implementation TTEditViewController

- (instancetype)initWithURLs:(NSArray<NSURL *> *)urls {
    self = [super init];
    if (self) {
        _editGroup = std::make_shared<TT::EditSpace>();
        _urls = [urls mutableCopy];
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self setupFilter];
    
    [_urls enumerateObjectsUsingBlock:^(NSURL * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        [self addMaterial:obj];
    }];
    
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSave target:self action:@selector(onSaveButton:)];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark -
#pragma mark UI
- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    
    self.imageView = [TTImageView new];
    self.imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:self.imageView];
    [self.imageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.imageView.superview);
    }];
    
    self.selectView = [[TTSlideSelectView alloc] initWithFrame:CGRectMake(40, 0, UIScreen.mainScreen.bounds.size.width - 60, 100)];
    self.selectView.center = CGPointMake(UIScreen.mainScreen.bounds.size.width / 2, UIScreen.mainScreen.bounds.size.height - 140);
    [self.view addSubview:self.selectView];
    self.selectView.previewBar.delegate = self;
    self.selectView.previewBar.dataSource = self;
    [self.selectView.previewBar registerClass:[TTPreviewCell class] forCellWithReuseIdentifier:kPreviewCellIdentifier];
}

- (void)setupFilter {
    _filterGroup = TT_MK_SP(TT::FilterGroup)();
    _filterGroup->addFilter([_imageView filter]);
    
    TT_SP(TT::Filter) sepiaFilter = TT_MK_SP(TT::SepiaFilter)();
    sepiaFilter->addFilter(_filterGroup);

    TT_SP(TT::Filter) gaussianBlurFilter = TT_MK_SP(TT::GaussianBlurFilter)();
    gaussianBlurFilter->addFilter(_filterGroup);
    
    TT_SP(TT::Filter) twoPassFilter = TT_MK_SP(TT::TwoPassFilter)();
    twoPassFilter->addFilter(_filterGroup);
    
    _filterTexture = TT_MK_SP(TT::Y420ToRGBFilter)();
    _filterTexture->addFilter(gaussianBlurFilter);
}

#pragma mark target/action
- (void)onSaveButton:(UIButton *)button {
    NSUInteger startIndex = self.selectView.leftIndex;
    NSUInteger endIndex = self.selectView.rightIndex;
    LOG(INFO) << "on save button:" << startIndex << " to " << endIndex;
    
    NSUInteger count = _editGroup->materialCount();
    for (int i = 0; i < count; i++) {
        std::shared_ptr<TT::Material> material = _editGroup->material(i);
        if (material) {
            TT_SP(TT::Clip) clip = TT_MK_SP(TT::Clip)();
            clip->setmaterial(material);
            int frameCount = material->frameCount();
            if (frameCount > startIndex) {
                clip->setsrcStartIndex(static_cast<int>(startIndex));
                if (frameCount > endIndex) {
                    clip->setsrcEndIndex(static_cast<int>(endIndex));
                    break;
                } else {
                    clip->setsrcEndIndex(frameCount);
                    endIndex -= (frameCount - startIndex);
                    startIndex = 0;
                }
            } else {
                startIndex -= frameCount;
                endIndex -= frameCount;
            }
        }
    }
    NSURL *url = [self.urls objectAtIndex:0];
    url = [url URLByDeletingLastPathComponent];
    url = [url URLByAppendingPathComponent:@"liangliang3.mp4"];
    const char *str = [url.absoluteString cStringUsingEncoding:NSUTF8StringEncoding];
    std::shared_ptr<TT::URL> _saveUrl = std::make_shared<TT::URL>(str);
    _editGroup->exportFile(_saveUrl);
}

#pragma mark Edit Material
- (void)addMaterial:(NSURL *)url {
    const char *str = [url.absoluteString cStringUsingEncoding:NSUTF8StringEncoding];
    std::shared_ptr<TT::Media> video = std::make_shared<TT::Media>(std::make_shared<TT::URL>(str));
    _editGroup->addMaterial(video);
    [self loadMoreForMaterial:video];
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
        
        std::shared_ptr<TT::Frame> frame = material->frame(static_cast<int>(indexPath.row));
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
    NSArray<NSIndexPath *> *indexPaths = _previewBar.indexPathsForVisibleItems;
    if (indexPaths.count > 0) {
        NSIndexPath *indexPath = indexPaths.firstObject;
        std::shared_ptr<TT::Material> material = _editGroup->material(static_cast<int>(indexPath.section));
        if(material) {
            std::shared_ptr<TT::Frame> frame = material->frame(static_cast<int>(indexPath.row));
            _filterTexture->processFrame(frame);
        }
    }
}

//UICollectionView被选中时调用的方法
-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    UICollectionViewCell *cell = (UICollectionViewCell *)[collectionView cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor whiteColor];
    
    std::shared_ptr<TT::Material> material = _editGroup->material(static_cast<int>(indexPath.section));
    if(material) {
        std::shared_ptr<TT::Frame> frame = material->frame(static_cast<int>(indexPath.row));
        _filterTexture->processFrame(frame);
    }
    [collectionView reloadData];
}

//返回这个UICollectionView是否可以被选择
-(BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

@end
