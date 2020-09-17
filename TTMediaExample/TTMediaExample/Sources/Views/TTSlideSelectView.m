//
//  TTSlideSelectView.m
//  TTMedia
//
//  Created by liang on 1/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#import "Masonry.h"

#import "TTSlideSelectView.h"

@interface TTSlideSelectView()

@property (nonatomic, strong) UICollectionView *previewBar;

@property (nonatomic, strong) UIView *leftView;
@property (nonatomic, strong) UIView *rightView;
@property (nonatomic, strong) UIView *centerView;

@property (nonatomic, strong) UIPanGestureRecognizer *panGesture;
@property (nonatomic, assign) CGPoint startCenter;
@property (nonatomic, assign) BOOL isLeft;
@property (nonatomic, assign) BOOL isRight;

@end

@implementation TTSlideSelectView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        _isLeft = NO;
        _isRight = NO;
        [self setupUI];
        [self createGesture];
        [self addObservers];
    }
    return self;
}

- (void)dealloc {
    [self removeObservers];
    [self destroyGesture];
}

- (void)setupUI {
    self.layer.borderColor = UIColor.whiteColor.CGColor;
    self.layer.borderWidth = 2;
    self.layer.cornerRadius = 5;
    self.layer.masksToBounds = YES;
    
    UICollectionViewFlowLayout *layout = [UICollectionViewFlowLayout new];
    layout.minimumLineSpacing = 0;
    layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    self.previewBar = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:layout];
    [self addSubview:self.previewBar];
    [self.previewBar mas_makeConstraints:^(MASConstraintMaker *make) {
        make.right.equalTo(self.previewBar.superview);
        make.left.equalTo(self.previewBar.superview);
        make.bottom.equalTo(self.previewBar.superview);
        make.top.equalTo(self.previewBar.superview);
    }];
    
    self.leftView.backgroundColor = UIColor.yellowColor;
    [self addSubview:self.leftView];
    self.leftView.frame = CGRectMake(0, 0, 20, self.frame.size.height);
    
    self.rightView.backgroundColor = UIColor.yellowColor;
    [self addSubview:self.rightView];
    self.rightView.frame = CGRectMake(self.frame.size.width - 20, 0, 20, self.frame.size.height);
    
    self.centerView.backgroundColor = UIColor.whiteColor;
    [self addSubview:self.centerView];
    [self.centerView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.center.top.bottom.equalTo(self.centerView.superview);
        make.width.equalTo(@(2));
    }];
}

- (void)layoutSubviews {
    [super layoutSubviews];
    self.leftView.bounds = CGRectMake(0, 0, 20, self.frame.size.height);
    self.rightView.bounds = CGRectMake(0, 0, 20, self.frame.size.height);
}

- (void)createGesture {
    self.panGesture = [UIPanGestureRecognizer new];
    [self.panGesture addTarget:self action:@selector(onPanGesture:)];
    [self addGestureRecognizer:self.panGesture];
}

- (void)destroyGesture {
    [self removeGestureRecognizer:self.panGesture];
}

#pragma mark -- observer
- (void)addObservers {
    [self.previewBar addObserver:self forKeyPath:@"contentOffset"
                         options:NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld
                         context:nil];
}

- (void)removeObservers {
    [self.previewBar removeObserver:self forKeyPath:@"contentOffset"];
}

#pragma mark -- override
- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey,id> *)change
                       context:(void *)context {
    if ([keyPath isEqualToString:@"contentOffset"]) {
        NSValue *newValue = [change objectForKey:NSKeyValueChangeNewKey];
        CGPoint newContentOffset = newValue.CGPointValue;
        NSValue *oldValue = [change objectForKey:NSKeyValueChangeOldKey];
        CGPoint oldContentOffset = oldValue.CGPointValue;
        CGFloat offset = oldContentOffset.x - newContentOffset.x;
        self.leftView.center = CGPointMake(self.leftView.center.x + offset, self.leftView.center.y);
        self.rightView.center = CGPointMake(self.rightView.center.x + offset, self.rightView.center.y);
        if (self.delegate && [self.delegate respondsToSelector:@selector(moveSlideSelectView:offset:)]) {
            [self.delegate moveSlideSelectView:self offset:offset];
        }
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

#pragma mark -- target/action
- (void)onPanGesture:(UIPanGestureRecognizer *)gesture {
    if (UIGestureRecognizerStateBegan == gesture.state) {
        CGPoint location = [gesture locationInView:self];
        CGRect leftFrame = self.leftView.frame;
        leftFrame.size.width *= 2;
        CGRect rightFrame = self.rightView.frame;
        rightFrame.origin.x -= rightFrame.size.width;
        rightFrame.size.width *= 2;
        if (CGRectContainsPoint(leftFrame, location)) {
            _isLeft = YES;
            _isRight = NO;
            self.startCenter = self.leftView.center;
        } else if (CGRectContainsPoint(rightFrame, location)) {
            _isLeft = NO;
            _isRight = YES;
            self.startCenter = self.rightView.center;
        } else {
            _isLeft = NO;
            _isRight = NO;
            [gesture cancelsTouchesInView];
        }
    } else if (UIGestureRecognizerStateChanged == gesture.state) {
        CGPoint translation = [gesture translationInView:self];
        if (_isLeft && !_isRight) {
            CGFloat centerX = self.startCenter.x + translation.x;
            if (centerX < self.previewBar.frame.origin.x) {
                return;
            }
            self.leftView.center = CGPointMake(centerX, self.leftView.center.y);
            if (self.delegate && [self.delegate respondsToSelector:@selector(moveLeftInSlideSelectView:)]) {
                [self.delegate moveLeftInSlideSelectView:self];
            }
        } else if (!_isLeft && _isRight) {
            CGFloat centerX = self.startCenter.x + translation.x;
            self.rightView.center = CGPointMake(centerX, self.rightView.center.y);
            if (self.delegate && [self.delegate respondsToSelector:@selector(moveRightInSlideSelectView:)]) {
                [self.delegate moveRightInSlideSelectView:self];
            }
        } else {
            if (self.delegate && [self.delegate respondsToSelector:@selector(moveSlideSelectView:offset:)]) {
                [self.delegate moveSlideSelectView:self offset:translation.x];
            }
        }
    } else {
        _isLeft = NO;
        _isRight = NO;
        if (self.delegate && [self.delegate respondsToSelector:@selector(endMoveInSlideSelectView:)]) {
            [self.delegate endMoveInSlideSelectView:self];
        }
    }
    
}

#pragma mark -- getter/setter
- (UIView *)leftView {
    if (nil == _leftView) {
        _leftView = [UIView new];
    }
    return _leftView;
}

- (UIView *)rightView {
    if (nil == _rightView) {
        _rightView = [UIView new];
    }
    return _rightView;
}

- (UIView *)centerView {
    if (nil == _centerView) {
        _centerView = [UIView new];
    }
    return _centerView;
}

- (NSUInteger)leftIndex {
    CGPoint point = [self convertPoint:self.leftView.center toView:self.previewBar];
    NSIndexPath *indexPath = [self.previewBar indexPathForItemAtPoint:point];
    return indexPath.row;
}

- (NSUInteger)rightIndex {
    CGPoint point = [self convertPoint:self.rightView.center toView:self.previewBar];
    NSIndexPath *indexPath = [self.previewBar indexPathForItemAtPoint:point];
    return indexPath.row;
}

@end
