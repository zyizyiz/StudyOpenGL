//
//  ViewController.m
//  002-光照
//
//  Created by Apple on 2020/9/18.
//  Copyright © 2020 张义镇. All rights reserved.
//

#import "ViewController.h"
#import "AGLKVertexAttribArrayBuffer.h"
#import "sceneUtil.h"

@interface ViewController () {
    SceneTriangle triangles[NUM_FACES];
}

@property (nonatomic, strong) EAGLContext *myContext;

@property (nonatomic, strong) GLKBaseEffect *baseEffect;

@property (nonatomic, strong) GLKBaseEffect *extraEffect;

@property (nonatomic, strong) AGLKVertexAttribArrayBuffer *vertexBuffer;

@property (nonatomic, strong) AGLKVertexAttribArrayBuffer *extraBuffer;

@property (nonatomic, assign) GLfloat centexVertexHeight;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupES];
    
    [self setupEffect];
    
    [self setupBuffer];
    
}

- (void)setupES {
    
    self.myContext = [[EAGLContext alloc] initWithAPI:(kEAGLRenderingAPIOpenGLES2)];
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.myContext;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [EAGLContext setCurrentContext:self.myContext];
}

- (void)setupEffect {
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.light0.enabled = GL_TRUE;
    
    self.baseEffect.light0.diffuseColor = GLKVector4Make(0.7, 0.7, 0.7, 1.0);
    
    self.baseEffect.light0.position = GLKVector4Make(1.0, 1.0, 0.5, 0.0);
    
    self.extraEffect = [[GLKBaseEffect alloc] init];
    self.extraEffect.useConstantColor = GL_TRUE;
    
    if (true) {
        GLKMatrix4 modelViewMatrix = GLKMatrix4MakeRotation(GLKMathDegreesToRadians(-60), 1.0, 0.0, 0.0);
        modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, GLKMathDegreesToRadians(-30.0), 0, 0, 1.0);
        
        modelViewMatrix = GLKMatrix4Translate(modelViewMatrix, 0.0, 0.0, 0.25);
        
        self.baseEffect.transform.modelviewMatrix = modelViewMatrix;
        self.extraEffect.transform.modelviewMatrix = modelViewMatrix;
    }
}

- (void)setupBuffer {
    
    triangles[0] = SceneTriangleMake(vertexA, vertexB, vertexD);
    triangles[1] = SceneTriangleMake(vertexB, vertexC, vertexF);
    triangles[2] = SceneTriangleMake(vertexD, vertexB, vertexE);
    triangles[3] = SceneTriangleMake(vertexE, vertexB, vertexF);
    triangles[4] = SceneTriangleMake(vertexD, vertexE, vertexH);
    triangles[5] = SceneTriangleMake(vertexE, vertexF, vertexH);
    triangles[6] = SceneTriangleMake(vertexG, vertexD, vertexH);
    triangles[7] = SceneTriangleMake(vertexH, vertexF, vertexI);
    
    self.vertexBuffer = [[AGLKVertexAttribArrayBuffer alloc] initWithAttribStride:sizeof(SceneVertex) numberOfVertices:sizeof(triangles) / sizeof(SceneVertex) bytes:triangles usage:GL_DYNAMIC_DRAW];
    self.extraBuffer = [[AGLKVertexAttribArrayBuffer alloc] initWithAttribStride:sizeof(SceneVertex) numberOfVertices:0 bytes:NULL usage:GL_DYNAMIC_DRAW];
    
    self.centexVertexHeight = 0.0;
}

#pragma mark -- GLKView DrawRect
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    [self.baseEffect prepareToDraw];
    [self.vertexBuffer prepareToDrawWithAttrib:GLKVertexAttribPosition numberOfCoordinates:3 attribOffset:offsetof(SceneVertex, position) shouldEnable:YES];
    
    [self.vertexBuffer prepareToDrawWithAttrib:GLKVertexAttribNormal numberOfCoordinates:3 attribOffset:offsetof(SceneVertex, normal) shouldEnable:YES];
    [self.vertexBuffer drawArrayWithMode:GL_TRIANGLES startVertexIndex:0 numberOfVertices:sizeof(triangles)/sizeof(SceneVertex)];
    
}

- (void)setCentexVertexHeight:(GLfloat)centexVertexHeight {
    _centexVertexHeight = centexVertexHeight;
    
    SceneVertex newVertexE = vertexE;
    newVertexE.position.z = _centexVertexHeight;
    
    triangles[2] = SceneTriangleMake(vertexD, vertexB, newVertexE);
    triangles[3] = SceneTriangleMake(newVertexE, vertexB, vertexF);
    triangles[4] = SceneTriangleMake(vertexD, newVertexE, vertexH);
    triangles[5] = SceneTriangleMake(newVertexE, vertexF, vertexH);
    
    [self updateNormals];
}

-(void)updateNormals
{

    //更新每个点的平面法向量
    SceneTrianglesUpdateFaceNormals(triangles);
    
    [self.vertexBuffer reinitWithAttribStride:sizeof(SceneVertex) numberOfVertices:sizeof(triangles)/sizeof(SceneVertex) bytes:triangles];
    
    
}

- (IBAction)changeHeight:(UISlider *)sender {
    self.centexVertexHeight = sender.value;
}

@end
