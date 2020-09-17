//
//  TTEditImageViewController.m
//  TTMedia
//
//  Created by liang on 5/9/2020.
//  Copyright © 2020 tina. All rights reserved.
//

#import "easylogging++.h"

#import "TTDef.h"
#import "TTProcess.h"
#import "TTImageView.h"
#import "TTEdit.h"
#import "TTImageFilterController.h"
#import "TTPreviewCell.h"

#import "TTEditImageViewController.h"
#import "TTCGImagePicture.h"

static NSString *kFilterCellIdentifier = @"kFilterCellIdentifier";

@interface TTEditImageViewController ()
< UICollectionViewDelegate
, UICollectionViewDataSource
, UICollectionViewDelegateFlowLayout
>
{
    TT_SP(TT::EditSpace) _editGroup;
    TT_SP(TT::RGBTextureFilter) _startFilter;
}

@property (nonatomic, strong) TTCGImagePicture *imagePicture;
// 图片区域
@property (nonatomic, strong) TTImageView *imageView;
// 滤镜列表
@property (nonatomic, strong) UICollectionView *filterList;

@end

@implementation TTEditImageViewController

- (instancetype)initWithURLs:(NSArray<NSURL *> *)urls {
    self = [super init];
    if (self) {
        _editGroup = std::make_shared<TT::EditSpace>();
        _urls = [urls mutableCopy];
        _imagePicture = [TTCGImagePicture new];
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

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.hidden = NO;
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    self.navigationController.navigationBar.hidden = YES;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark -
#pragma mark UI
- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    self.view.layer.masksToBounds = YES;
    self.view.layer.cornerRadius = 5;
    
    self.imageView = [TTImageView new];
    self.imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:self.imageView];
    [self.imageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.imageView.superview);
    }];
    
    UICollectionViewFlowLayout *layout = [UICollectionViewFlowLayout new];
    layout.minimumLineSpacing = 0;
    layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    self.filterList = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:layout];
    self.filterList.layer.masksToBounds = YES;
    self.filterList.layer.cornerRadius = 5;
    self.filterList.backgroundColor = UIColor.yellowColor;
    self.filterList.frame = CGRectMake(40, 0, UIScreen.mainScreen.bounds.size.width - 60, 100);
    self.filterList.center = CGPointMake(UIScreen.mainScreen.bounds.size.width / 2, UIScreen.mainScreen.bounds.size.height - 120);
    self.filterList.delegate = self;
    self.filterList.dataSource = self;
    [self.filterList registerClass:TTPreviewCell.class forCellWithReuseIdentifier:kFilterCellIdentifier];
    [self.view addSubview:self.filterList];
}

- (void)setupFilter {
    _startFilter = TT_MK_SP(TT::RGBTextureFilter)();
}

#pragma mark target/action
- (void)onSaveButton:(UIButton *)button {

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
        [self reload];
    });
}

- (void)reload {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self showPicture];
    });
}

- (void)showPicture
{
    TT_SP(TT::Material) material = _editGroup->material(0);
    if(material) {
        TT_SP(TT::Frame) frame = material->frameAt(0);
        [self processFrame:frame];
    }
}

- (void)processFrame:(TT_SP(TT::Frame))frame
{
    _startFilter->processFrame(frame);
}

#pragma mark -- UICollectionViewDataSource
//定义展示的UICollectionViewCell的个数
-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return TT::ImageFilterFactory::filterCount();
}

//定义展示的Section的个数
-(NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}

//每个UICollectionView展示的内容
-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView
                 cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    
    TTPreviewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:kFilterCellIdentifier
                                                                    forIndexPath:indexPath];
    
    cell.backgroundColor = [UIColor colorWithRed:((10 * indexPath.row) / 255.0)
                                           green:((20 * indexPath.row)/255.0)
                                            blue:((30 * indexPath.row)/255.0)
                                           alpha:1.0f];
    
    [cell setupUI];
    
    int32_t index = (int32_t)indexPath.row;
    std::string fileName = TT::ImageFilterFactory::filterName(index);
    NSString *title = [NSString stringWithUTF8String:fileName.c_str()];
    [cell.label setText:title];
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

//UICollectionView被选中时调用的方法
-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    UICollectionViewCell *cell = (UICollectionViewCell *)[collectionView cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor whiteColor];
    
    int32_t index = (int32_t)indexPath.row;
    TT_SP(TT::Filter) filter = TT::ImageFilterFactory::createFilter(index);
    TT_SP(TT::Filter) output = [_imageView filter];
    filter->addOutput(output);
    _startFilter->addOutput(filter);
    
    [collectionView reloadData];
    [self reload];
}

@end
