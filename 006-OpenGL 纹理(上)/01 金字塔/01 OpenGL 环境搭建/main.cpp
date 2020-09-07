#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

/*
 GLMatrixStack 变化管线使用矩阵堆栈
 
 GLMatrixStack 构造函数允许指定堆栈的最大深度、默认的堆栈深度为64.这个矩阵堆在初始化时已经在堆栈中包含了单位矩阵。
 GLMatrixStack::GLMatrixStack(int iStackDepth = 64);
 
 //通过调用顶部载入这个单位矩阵
 void GLMatrixStack::LoadIndentiy(void);
 
 //在堆栈顶部载入任何矩阵
 void GLMatrixStack::LoadMatrix(const M3DMatrix44f m);
 */
// 各种需要的类
GLShaderManager		shaderManager;
GLMatrixStack		modelViewMatrix;
GLMatrixStack		projectionMatrix;
GLFrame				cameraFrame;
GLFrame             objectFrame;
//投影矩阵
GLFrustum			viewFrustum;

//容器类（7种不同的图元对应7种容器对象）
GLBatch				pyramidBatch;

GLuint              textureID;

GLGeometryTransform transformPipeline;
M3DMatrix44f        shadowMatrix;

void MakePyramid(GLBatch& pyramidBatch) {
    pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    
    M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
    M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
    M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
    M3DVector3f vBackLeft = { -1.0f,  -1.0f, -1.0f };
    M3DVector3f vBackRight = { 1.0f,  -1.0f, -1.0f };
    M3DVector3f n;
    
    m3dFindNormal(n, vBackLeft, vBackRight, vFrontRight);
    
    //vBackLeft
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
     pyramidBatch.Vertex3fv(vBackLeft);
     
     //vBackRight
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
     pyramidBatch.Vertex3fv(vBackRight);
     
     //vFrontRight
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
     pyramidBatch.Vertex3fv(vFrontRight);
     
     
     //三角形Y =(vFrontLeft,vBackLeft,vFrontRight)
    
     //1.找到三角形X 法线
     m3dFindNormal(n, vFrontLeft, vBackLeft, vFrontRight);
     
     //vFrontLeft
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
     pyramidBatch.Vertex3fv(vFrontLeft);
     
     //vBackLeft
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
     pyramidBatch.Vertex3fv(vBackLeft);
     
     //vFrontRight
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
     pyramidBatch.Vertex3fv(vFrontRight);

     
     // 金字塔前面
     //三角形：（Apex，vFrontLeft，vFrontRight）
     m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
    
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
     pyramidBatch.Vertex3fv(vApex);
     
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
     pyramidBatch.Vertex3fv(vFrontLeft);

     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
     pyramidBatch.Vertex3fv(vFrontRight);
     
     //金字塔左边
     //三角形：（vApex, vBackLeft, vFrontLeft）
     m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
     pyramidBatch.Vertex3fv(vApex);
     
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
     pyramidBatch.Vertex3fv(vBackLeft);
     
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
     pyramidBatch.Vertex3fv(vFrontLeft);
     
     //金字塔右边
     //三角形：（vApex, vFrontRight, vBackRight）
     m3dFindNormal(n, vApex, vFrontRight, vBackRight);
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
     pyramidBatch.Vertex3fv(vApex);
     
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
     pyramidBatch.Vertex3fv(vFrontRight);
     
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
     pyramidBatch.Vertex3fv(vBackRight);
     
     //金字塔后边
     //三角形：（vApex, vBackRight, vBackLeft）
     m3dFindNormal(n, vApex, vBackRight, vBackLeft);
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
     pyramidBatch.Vertex3fv(vApex);
     
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
     pyramidBatch.Vertex3fv(vBackRight);
     
     pyramidBatch.Normal3fv(n);
     pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
     pyramidBatch.Vertex3fv(vBackLeft);
     
     //结束批次设置
     pyramidBatch.End();
    
}


bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    
    if (pBits == NULL) {
        return false;
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
    
    free(pBits);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    return true;
    
}

// 此函数在呈现上下文中进行任何必要的初始化。.
// 这是第一次做任何与opengl相关的任务。
void SetupRC()
{
  
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    shaderManager.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);
    
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    LoadTGATexture("stone.tga", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    MakePyramid(pyramidBatch);
    
    cameraFrame.MoveForward(-10);
}



void ShutdownRC(void) {
    glDeleteTextures(1, &textureID);
}

// 召唤场景
void RenderScene(void)
{
    
    static GLfloat vLightPos [] = { 1.0f, 1.0f, 0.0f };
    static GLfloat vWhite [] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    modelViewMatrix.PushMatrix();
    
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    
    modelViewMatrix.MultMatrix(mCamera);
    
    
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vWhite, 0);
    
    pyramidBatch.Draw();
    
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

//特殊键位处理（上、下、左、右移动）
void SpecialKeys(int key, int x, int y)
{
  
    if (key == GLUT_KEY_UP) {
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}


// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
  
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    modelViewMatrix.LoadIdentity();
    
    
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    //申请一个颜色缓存区、深度缓存区、双缓存区、模板缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置window 的尺寸
    glutInitWindowSize(800, 600);
    //创建window的名称
    glutCreateWindow("Pyramid");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    //特殊键位函数（上下左右）
    glutSpecialFunc(SpecialKeys);
    //显示函数
    glutDisplayFunc(RenderScene);
    
    //判断一下是否能初始化glew库，确保项目能正常使用OpenGL 框架
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    //绘制
    SetupRC();
    
    //runloop运行循环
    glutMainLoop();
    
    ShutdownRC();
    return 0;
}
