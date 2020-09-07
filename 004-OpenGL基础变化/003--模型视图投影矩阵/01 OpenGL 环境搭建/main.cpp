
#include "GLTools.h"	
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "GLBatch.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLFrustum           viewFrustum;
GLShaderManager     shaderManager;
GLTriangleBatch     torusBatch;
GLGeometryTransform transformPipeline;

// 设置视口和投影矩阵
void ChangeSize(int w, int h)
{
    //防止除以零
    if(h == 0)
        h = 1;
    
    //将视口设置为窗口尺寸
    glViewport(0, 0, w, h);
    
    //设置透视投影
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 1000.0f);
}


//召唤场景
void RenderScene(void)
{
   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    M3DMatrix44f mTranslate, mRotate, mModelview, mModelViewProjection;
    
    m3dTranslationMatrix44(mTranslate, 0.0, 0.0, -2.5);
    
    m3dRotationMatrix44(mRotate, m3dDegToRad(yRot), 0.0, 1.0, 0.0);
    
    m3dMatrixMultiply44(mModelview, mTranslate, mRotate);
    
    m3dMatrixMultiply44(mModelViewProjection, viewFrustum.GetProjectionMatrix(), mModelview);
    
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT, mModelViewProjection, vBlack);
    torusBatch.Draw();
    
    glutSwapBuffers();
    
    glutPostRedisplay();
}


void SetupRC()
{
   
    glClearColor(0.8, 0.8, 0.8, 1.0);
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    
    gltMakeSphere(torusBatch, 0.4, 10, 20);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}



int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("ModelViewProjection Example");
    glutReshapeFunc(ChangeSize);
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
