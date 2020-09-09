//
//  ViewController.m
//  001--CubeImage
//
//  Created by Apple on 2020/9/9.
//  Copyright © 2020 张义镇. All rights reserved.
//

#import "ViewController.h"
#import <GLKit/GLKit.h>

typedef struct {
    GLKVector3 positionCoord;
    GLKVector2 textureCoord;
    GLKVector3 normal;
} CCVertex;

static NSInteger const kCoordCount = 36;

@interface ViewController ()<GLKViewDelegate>

@property (nonatomic, strong) GLKView *glkView;

@property (nonatomic, strong) GLKBaseEffect *baseEffect;

@property (nonatomic, assign) CCVertex *vertices;

@property (nonatomic, strong) CADisplayLink *displayLink;

@property (nonatomic, assign) NSInteger angle;

@property (nonatomic, assign) GLuint vertexBuffer;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor blackColor];
    
    [self commonInit];
    
    [self vertexDataSetup];
    
    [self addCADisplayLink];
}

- (void)commonInit {
    
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    [EAGLContext setCurrentContext:context];
    
    self.glkView = [[GLKView alloc] initWithFrame:self.view.bounds context:context];
    self.glkView.backgroundColor = [UIColor clearColor];
    self.glkView.delegate = self;
    [self.view addSubview:self.glkView];
    
    self.glkView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    
    NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"kunkun" ofType:@"jpg"];
    UIImage *image = [UIImage imageWithContentsOfFile:imagePath];
    
    NSDictionary *options = @{GLKTextureLoaderOriginBottomLeft:@YES};
    GLKTextureInfo *textureInfo = [GLKTextureLoader textureWithCGImage:image.CGImage options:options error:nil];
    
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.texture2d0.name = textureInfo.name;
    self.baseEffect.texture2d0.target = textureInfo.target;
    
    
    
    glEnable(GL_DEPTH_TEST);
}

-(void)vertexDataSetup {
    
    self.vertices = malloc(sizeof(CCVertex) * kCoordCount);
    self.vertices[0] = (CCVertex){{-0.5, 0.5, 0.5},  {0, 1}};
    self.vertices[1] = (CCVertex){{-0.5, -0.5, 0.5}, {0, 0}};
    self.vertices[2] = (CCVertex){{0.5, 0.5, 0.5},   {1, 1}};
    
    self.vertices[3] = (CCVertex){{-0.5, -0.5, 0.5}, {0, 0}};
    self.vertices[4] = (CCVertex){{0.5, 0.5, 0.5},   {1, 1}};
    self.vertices[5] = (CCVertex){{0.5, -0.5, 0.5},  {1, 0}};
    
    // 上面
    self.vertices[6] = (CCVertex){{0.5, 0.5, 0.5},    {1, 1}};
    self.vertices[7] = (CCVertex){{-0.5, 0.5, 0.5},   {0, 1}};
    self.vertices[8] = (CCVertex){{0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[9] = (CCVertex){{-0.5, 0.5, 0.5},   {0, 1}};
    self.vertices[10] = (CCVertex){{0.5, 0.5, -0.5},  {1, 0}};
    self.vertices[11] = (CCVertex){{-0.5, 0.5, -0.5}, {0, 0}};
    
    // 下面
    self.vertices[12] = (CCVertex){{0.5, -0.5, 0.5},    {1, 1}};
    self.vertices[13] = (CCVertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[14] = (CCVertex){{0.5, -0.5, -0.5},   {1, 0}};
    self.vertices[15] = (CCVertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[16] = (CCVertex){{0.5, -0.5, -0.5},   {1, 0}};
    self.vertices[17] = (CCVertex){{-0.5, -0.5, -0.5},  {0, 0}};
    
    // 左面
    self.vertices[18] = (CCVertex){{-0.5, 0.5, 0.5},    {1, 1}};
    self.vertices[19] = (CCVertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[20] = (CCVertex){{-0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[21] = (CCVertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[22] = (CCVertex){{-0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[23] = (CCVertex){{-0.5, -0.5, -0.5},  {0, 0}};
    
    // 右面
    self.vertices[24] = (CCVertex){{0.5, 0.5, 0.5},    {1, 1}};
    self.vertices[25] = (CCVertex){{0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[26] = (CCVertex){{0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[27] = (CCVertex){{0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[28] = (CCVertex){{0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[29] = (CCVertex){{0.5, -0.5, -0.5},  {0, 0}};
    
    // 后面
    self.vertices[30] = (CCVertex){{-0.5, 0.5, -0.5},   {0, 1}};
    self.vertices[31] = (CCVertex){{-0.5, -0.5, -0.5},  {0, 0}};
    self.vertices[32] = (CCVertex){{0.5, 0.5, -0.5},    {1, 1}};
    self.vertices[33] = (CCVertex){{-0.5, -0.5, -0.5},  {0, 0}};
    self.vertices[34] = (CCVertex){{0.5, 0.5, -0.5},    {1, 1}};
    self.vertices[35] = (CCVertex){{0.5, -0.5, -0.5},   {1, 0}};
    
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    GLsizeiptr bufferSizeBytes = sizeof(CCVertex) * kCoordCount;
    glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, self.vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(CCVertex), NULL + offsetof(CCVertex, positionCoord));
    
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(CCVertex), NULL + offsetof(CCVertex, textureCoord));
    
    
}

- (void)addCADisplayLink {
    self.angle = 0;
    
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateAngle)];
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

-(void)updateAngle {
    
    self.angle = (self.angle + 5) % 360;
    
    self.baseEffect.transform.modelviewMatrix = GLKMatrix4MakeRotation(GLKMathDegreesToRadians(self.angle), 0.3, 1, -0.7);
    [self.glkView display];
}

- (void)dealloc
{
    if ([EAGLContext currentContext] == self.glkView.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    if (_vertices) {
        free(_vertices);
        _vertices = nil;
    }
    
    if (_vertexBuffer) {
        glDeleteBuffers(1, &_vertexBuffer);
        _vertexBuffer = 0;
    }
    
    [self.displayLink invalidate];
}

#pragma GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    [self.baseEffect prepareToDraw];
    
    glDrawArrays(GL_TRIANGLES, 0, kCoordCount);
}


@end
