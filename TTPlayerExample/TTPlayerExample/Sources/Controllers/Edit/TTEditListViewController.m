//
//  TTEditListViewController.m
//  TTPlayerExample
//
//  Created by liang on 1/7/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#import "TTMovieListViewModel.h"
#import "TTMovieItemViewModel.h"

#import "TTEditViewController.h"
#import "TTEditListViewController.h"

@interface TTEditListViewController ()

@end

@implementation TTEditListViewController

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    TTMovieItemViewModel *item = [self.viewModel itemAtIndex:indexPath.row];
    TTEditViewController *editVC = [[TTEditViewController alloc] initWithURLs:@[item.url]];
    [self.navigationController pushViewController:editVC animated:YES];
}

@end
