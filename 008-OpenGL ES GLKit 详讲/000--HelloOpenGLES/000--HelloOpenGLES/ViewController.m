//
//  ViewController.m
//  000--HelloOpenGLES
//
//  Created by Apple on 2020/9/8.
//  Copyright © 2020 张义镇. All rights reserved.
//

#import "ViewController.h"
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

@interface ViewController ()
{
    EAGLContext *context;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    context = [[EAGLContext alloc] initWithAPI:(kEAGLRenderingAPIOpenGLES3)];
    
    if (!context) {
        NSLog(@"Create ES context Failed");
    }
    
    [EAGLContext setCurrentContext:context];
    
    GLKView *view = (GLKView *)self.view;
    view.context = context;
    
    glClearColor(1, 0, 0, 1);
}


- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClear(GL_COLOR_BUFFER_BIT);
}

@end
