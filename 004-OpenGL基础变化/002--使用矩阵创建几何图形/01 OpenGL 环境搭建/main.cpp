// Objects.cpp
// OpenGL SuperBible, Chapter 4
// Demonstrates GLTools built-in objects
// Program by Richard S. Wright Jr.

#include "GLTools.h"	// OpenGL toolkit
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager		shaderManager;
GLMatrixStack		modelViewMatrix;
GLMatrixStack		projectionMatrix;
//观察者位置
GLFrame				cameraFrame;
//世界坐标位置
GLFrame             objectFrame;

//视景体，用来构造投影矩阵
GLFrustum			viewFrustum;

//三角形批次类
GLTriangleBatch     CC_Triangle;

//球
GLTriangleBatch     sphereBatch;
//环
GLTriangleBatch     torusBatch;
//圆柱
GLTriangleBatch     cylinderBatch;
//锥
GLTriangleBatch     coneBatch;
//磁盘
GLTriangleBatch     diskBatch;

GLGeometryTransform	transformPipeline;
M3DMatrix44f		shadowMatrix;

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

int nStep = 0;

// 将上下文中，进行必要的初始化
void SetupRC()
{
    glClearColor(0.7, 0.7, 0.7, 1.0);
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    
    objectFrame.MoveForward(15);
    
    gltMakeSphere(sphereBatch, 3.0, 10, 20);
    
    gltMakeTorus(torusBatch, 3.0, 0.75, 15, 15);
    
    gltMakeCylinder(cylinderBatch, 2.0, 2.0, 3.0, 15, 2);
    
    gltMakeCylinder(coneBatch, 2.0, 0.0, 3.0, 13, 2);
    
    gltMakeDisk(diskBatch, 1.5, 3.0, 13, 3);
    
}



void DrawWireFramedBatch(GLTriangleBatch* pBatch)
{
 
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    
    pBatch->Draw();
    
    
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glPolygonOffset(-1, -1);
    
    glLineWidth(2.5);
    
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    
    pBatch->Draw();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glDisable(GL_BLEND);
    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
    
}

//召唤场景
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    modelViewMatrix.PushMatrix(objectFrame);
    
    
    switch(nStep) {
        case 0:
            DrawWireFramedBatch(&sphereBatch);
            break;
        case 1:
            DrawWireFramedBatch(&torusBatch);
            break;
        case 2:
            DrawWireFramedBatch(&cylinderBatch);
            break;
        case 3:
            DrawWireFramedBatch(&coneBatch);
            break;
        case 4:
            DrawWireFramedBatch(&diskBatch);
            break;
    }
    
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

//上下左右，移动图形
void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        //移动世界坐标系，而不是去移动物体。
        //将世界坐标系在X方向移动-5.0
    objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
    glutPostRedisplay();
}




//点击空格，切换渲染图形
void KeyPressFunc(unsigned char key, int x, int y)
{
    if(key == 32)
    {
        nStep++;
        
        if(nStep > 4)
            nStep = 0;
    }
    
    switch(nStep)
    {
        case 0:
            glutSetWindowTitle("Sphere");
            break;
        case 1:
            glutSetWindowTitle("Torus");
            break;
        case 2:
            glutSetWindowTitle("Cylinder");
            break;
        case 3:
            glutSetWindowTitle("Cone");
            break;
        case 4:
            glutSetWindowTitle("Disk");
            break;
    }
    
    glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
    //1.视口
    glViewport(0, 0, w, h);
    
    //2.透视投影
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
    //projectionMatrix 矩阵堆栈 加载透视投影矩阵
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //3.modelViewMatrix 矩阵堆栈 加载单元矩阵
    modelViewMatrix.LoadIdentity();
    
    //4.通过GLGeometryTransform管理矩阵堆栈
    //使用transformPipeline 管道管理模型视图矩阵堆栈 和 投影矩阵堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Sphere");
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(KeyPressFunc);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    
    SetupRC();
    
    glutMainLoop();
    return 0;
}
