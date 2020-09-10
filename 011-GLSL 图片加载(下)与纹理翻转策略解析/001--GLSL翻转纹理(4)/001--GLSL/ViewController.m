//
//  ViewController.m
//  001--GLSL
//
//  Created by Apple on 2020/9/9.
//  Copyright © 2020 张义镇. All rights reserved.
//

#import "ViewController.h"
#import "YZView.h"

@interface ViewController ()

@property (nonatomic, strong) YZView *myView;


@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.myView = [[YZView alloc] initWithFrame:self.view.bounds];
    self.myView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:self.myView];
}


@end
