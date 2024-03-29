//
//  TTEditListViewController.m
//  TTMedia
//
//  Created by liang on 1/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#import "TTEditListViewController.h"
#import "TTMovieListViewModel.h"
#import "TTMovieItemViewModel.h"
#import "TTEditViewController.h"
#import "TTEditImageViewController.h"


@interface TTEditListViewController ()

@end

@implementation TTEditListViewController

#pragma mark -- UITableViewDelegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    TTMovieItemViewModel *item = [self.viewModel itemAtIndex:indexPath.row];
    if (item.type == kItemTypeImage) {
        TTEditImageViewController *editVC = [[TTEditImageViewController alloc] initWithURLs:@[item.url]];
        [self.navigationController pushViewController:editVC animated:YES];
    } else {
        TTEditViewController *editVC = [[TTEditViewController alloc] initWithURLs:@[item.url]];
        [self.navigationController pushViewController:editVC animated:YES];
    }
}

@end
