//
//  YZView.m
//  001--GLSL
//
//  Created by Apple on 2020/9/9.
//  Copyright © 2020 张义镇. All rights reserved.
//

#import "YZView.h"
#import <OpenGLES/ES3/gl.h>
#import "GLESMath.h"
#import "GLESUtils.h"

@interface YZView()


@property (nonatomic, strong) CAEAGLLayer *myEagLayer;

@property (nonatomic, strong) EAGLContext *myContext;

@property (nonatomic, assign) GLuint myColorRenderBuffer;

@property (nonatomic, assign) GLuint myColorFrameBuffer;

@property (nonatomic, assign) GLuint myPrograme;

@property (nonatomic, assign) GLuint myVertices;

@end


@implementation YZView

{
    float xDegree;
    float yDegree;
    float zDegree;
    
    BOOL bX;
    BOOL bY;
    BOOL bZ;
    NSTimer *myTimer;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self setup];
    }
    return self;
}


- (void)setup {
    
    [self setupUI];
    
    [self setupLayer];
    
    [self setupContext];
    
    [self deleteRenderAndFrameBuffer];
    
    [self setupRenderBuffer];
    
    [self setupFrameBuffer];
    
    [self renderLayer];
}

- (void)setupUI {
    CGSize selfSize = self.bounds.size;
    static CGFloat buttonMargin = 30;
    CGSize buttonSize = CGSizeMake((selfSize.width - buttonMargin * 4) / 3, (selfSize.width - buttonMargin * 4) / 3);
    UIButton *buttonX = [[UIButton alloc] initWithFrame:CGRectMake(buttonMargin, selfSize.height - 150, buttonSize.width, buttonSize.height)];
    buttonX.backgroundColor = [UIColor greenColor];
    [buttonX setTitle:@"X" forState:(UIControlStateNormal)];
    [self addSubview:buttonX];
    [buttonX addTarget:self action:@selector(handleClickX) forControlEvents:(UIControlEventTouchUpInside)];
    
    UIButton *buttonY = [[UIButton alloc] initWithFrame:CGRectMake(buttonX.frame.origin.x + buttonMargin + buttonSize.width, buttonX.frame.origin.y, buttonSize.width, buttonSize.height)];
    buttonY.backgroundColor = [UIColor greenColor];
    [buttonY setTitle:@"Y" forState:(UIControlStateNormal)];
    [self addSubview:buttonY];
    [buttonY addTarget:self action:@selector(handleClickY) forControlEvents:(UIControlEventTouchUpInside)];
    
    UIButton *buttonZ = [[UIButton alloc] initWithFrame:CGRectMake( buttonY.frame.origin.x + buttonMargin + buttonSize.width, buttonY.frame.origin.y, buttonSize.width, buttonSize.height)];
    buttonZ.backgroundColor = [UIColor greenColor];
    [buttonZ setTitle:@"Z" forState:(UIControlStateNormal)];
    [self addSubview:buttonZ];
    [buttonZ addTarget:self action:@selector(handleClickZ) forControlEvents:(UIControlEventTouchUpInside)];
    
}


- (void)setupLayer {
    
    self.myEagLayer = (CAEAGLLayer *)self.layer;
    
    [self setContentScaleFactor:[[UIScreen mainScreen] scale]];
    
    self.myEagLayer.opaque = YES;
    self.myEagLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:@NO,kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
    
}

- (void)setupContext {
    
    self.myContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    [EAGLContext setCurrentContext:self.myContext];
    
    
}

- (void)deleteRenderAndFrameBuffer {
    
    glDeleteBuffers(1, &_myColorRenderBuffer);
    _myColorRenderBuffer = 0;
    glDeleteBuffers(1, &_myColorFrameBuffer);
    _myColorFrameBuffer = 0;
}

- (void)setupRenderBuffer {
    
    GLuint buffer;
    glGenBuffers(1, &buffer);
    
    self.myColorRenderBuffer = buffer;
    
    glBindRenderbuffer(GL_RENDERBUFFER, self.myColorRenderBuffer);
    [self.myContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:self.myEagLayer];
    
}

- (void)setupFrameBuffer {
    
    GLuint buffer;
    glGenBuffers(1, &buffer);
    
    self.myColorFrameBuffer = buffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, self.myColorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.myColorRenderBuffer);
}

- (void)renderLayer {
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    GLfloat scale = [[UIScreen mainScreen] scale];
    
    glViewport(self.frame.origin.x * scale, self.frame.origin.y * scale, self.frame.size.width * scale, self.frame.size.height * scale);
    
    NSString *verFile = [[NSBundle mainBundle] pathForResource:@"shaderv" ofType:@"vsh"];
    NSString *fragFile = [[NSBundle mainBundle] pathForResource:@"shaderf" ofType:@"fsh"];
    
    if (self.myPrograme) {
        glDeleteProgram(self.myPrograme);
        self.myPrograme = 0;
    }
    
    self.myPrograme = [self loadShader:verFile frag:fragFile];
    
    glLinkProgram(self.myPrograme);
    
    GLint linkSuccess;
    
    glGetProgramiv(self.myPrograme, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar message[256];
        glGetProgramInfoLog(self.myPrograme, sizeof(message), 0, &message[0]);
        NSString *messageString = [NSString stringWithUTF8String:message];
        NSLog(@"error: %@",messageString);
        
        return;
    }
    
    glUseProgram(self.myPrograme);
    
    GLfloat attrArr[] =
    {
        -0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 1.0f, //左上0
        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 1.0f, //右上1
        -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f, //左下2
        
        0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f, //右下3
        0.0f, 0.0f, 1.0f,       0.0f, 1.0f, 0.0f, //顶点4
    };
    
    // 索引数组
    GLuint indices[] =
    {
        0, 3, 2,
        0, 1, 3,
        0, 2, 4,
        0, 4, 1,
        2, 3, 4,
        1, 4, 3,
    };
    
    if (self.myVertices == 0) {
        glGenBuffers(1, &_myVertices);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, _myVertices);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(attrArr), attrArr, GL_DYNAMIC_DRAW);
    
    GLuint position = glGetAttribLocation(self.myPrograme, "position");
    
    glEnableVertexAttribArray(position);
    
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, NULL);
    
    
    GLuint positionColor = glGetAttribLocation(self.myPrograme, "positionColor");
    
    glEnableVertexAttribArray(positionColor);
    glVertexAttribPointer(positionColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (float *)NULL + 3);
    
    GLuint projectiionMatrixSlot = glGetUniformLocation(self.myPrograme, "projectionMatrix");
    GLuint modelViewMatrixSlot = glGetUniformLocation(self.myPrograme, "modelViewMatrix");
    
    float width = self.frame.size.width;
    float height = self.frame.size.height;
    
    
    KSMatrix4 _projectionMatrix;
    
    ksMatrixLoadIdentity(&_projectionMatrix);
    
    float aspect = width / height;
    
    ksPerspective(&_projectionMatrix, 30.0, aspect, 5.0 , 20.0);
    
    glUniformMatrix4fv(projectiionMatrixSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
    
    KSMatrix4 _modelViewMatrix;
    
    ksMatrixLoadIdentity(&_modelViewMatrix);
    
    ksTranslate(&_modelViewMatrix, 0, 0, -10);
    
    KSMatrix4 _rotationMatrix;
    
    ksMatrixLoadIdentity(&_rotationMatrix);
    
    ksRotate(&_rotationMatrix, xDegree, 1.0, 0.0, 0.0);
    ksRotate(&_rotationMatrix, yDegree, 0.0, 1.0, 0.0);
    ksRotate(&_rotationMatrix, zDegree, 0.0, 0.0, 1.0);
    
    ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
    
    glUniformMatrix4fv(modelViewMatrixSlot, 1, GL_FALSE, (GLfloat *)&_modelViewMatrix.m[0][0]);
    
    glEnable(GL_CULL_FACE);
    
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, indices);
    
    [self.myContext presentRenderbuffer:GL_RENDERBUFFER];
    
    
}

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (GLuint)loadShader:(NSString*)vertFile frag:(NSString *)fragFile {
    
    GLuint verShader, fragShader;
    
    GLuint program = glCreateProgram();
    
    [self compileShader:&verShader type:GL_VERTEX_SHADER file:vertFile];
    [self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragFile];
    
    glAttachShader(program, verShader);
    glAttachShader(program, fragShader);
    
    glDeleteShader(verShader);
    glDeleteShader(fragShader);
    
    
    return program;
}

-(void)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    NSString *content = [NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil];
    const GLchar *source = (GLchar *)[content UTF8String];
    
    *shader = glCreateShader(type);
    
    glShaderSource(*shader, 1, &source, NULL);
    
    glCompileShader(*shader);
    
    
}

- (void)handleClickX {
    
    if (myTimer == nil) {
        myTimer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(reDegree) userInfo:nil repeats:YES];
    }
    bX = !bX;
}

- (void)handleClickY {
    
    if (myTimer == nil) {
        myTimer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(reDegree) userInfo:nil repeats:YES];
    }
    bY = !bY;
}

- (void)handleClickZ {
    
    if (myTimer == nil) {
        myTimer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(reDegree) userInfo:nil repeats:YES];
    }
    bZ = !bZ;
}

- (void)reDegree {
    xDegree += bX * 5;
    yDegree += bY * 5;
    zDegree += bZ * 5;
    //重新渲染
    [self renderLayer];
}
@end
