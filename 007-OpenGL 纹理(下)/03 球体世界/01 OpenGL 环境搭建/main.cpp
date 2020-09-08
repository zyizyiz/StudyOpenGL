#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

//**4、添加附加随机球
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

GLShaderManager		shaderManager;			// 着色器管理器
GLMatrixStack		modelViewMatrix;		// 模型视图矩阵
GLMatrixStack		projectionMatrix;		// 投影矩阵
GLFrustum			viewFrustum;			// 视景体
GLGeometryTransform	transformPipeline;		// 几何图形变换管道

GLTriangleBatch		torusBatch;             // 花托批处理
GLBatch				floorBatch;             // 地板批处理

//**2、定义公转球的批处理（公转自转）**
GLTriangleBatch     sphereBatch;            //球批处理

//**3、角色帧 照相机角色帧（全局照相机实例）
GLFrame             cameraFrame;

//**5、添加纹理
//纹理标记数组
GLuint uiTextures[3];

bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{

    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    //1.读取纹理数据
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if(pBits == NULL)
        return false;
    
    //2、设置纹理参数
    //参数1：纹理维度
    //参数2：为S/T坐标设置模式
    //参数3：wrapMode,环绕模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    //参数1：纹理维度
    //参数2：线性过滤
    //参数3：wrapMode,环绕模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    //3.载入纹理
    //参数1：纹理维度
    //参数2：mip贴图层次
    //参数3：纹理单元存储的颜色成分（从读取像素图是获得）-将内部参数nComponents改为了通用压缩纹理格式GL_COMPRESSED_RGB
    //参数4：加载纹理宽
    //参数5：加载纹理高
    //参数6：加载纹理的深度
    //参数7：像素数据的数据类型（GL_UNSIGNED_BYTE，每个颜色分量都是一个8位无符号整数）
    //参数8：指向纹理图像数据的指针
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, nWidth, nHeight, 0,
                 eFormat, GL_UNSIGNED_BYTE, pBits);
    
    //使用完毕释放pBits
    free(pBits);
  
    //4.设置Mip贴图,纹理生成所有的Mip层
    //参数：GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    return true;
}



void SetupRC()
{
  
    glClearColor(0.0, 0.0, 0.0, 1.0);
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    gltMakeSphere(torusBatch, 0.4, 40, 80);
    
    gltMakeSphere(sphereBatch, 0.1, 26, 13);
    
    GLfloat texSize = 10.0;
    floorBatch.Begin(GL_TRIANGLE_FAN, 4,1);
   floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
   floorBatch.Vertex3f(-20.f, -0.41f, 20.0f);
   
   floorBatch.MultiTexCoord2f(0, texSize, 0.0f);
   floorBatch.Vertex3f(20.0f, -0.41f, 20.f);
   
   floorBatch.MultiTexCoord2f(0, texSize, texSize);
   floorBatch.Vertex3f(20.0f, -0.41f, -20.0f);
   
   floorBatch.MultiTexCoord2f(0, 0.0f, texSize);
   floorBatch.Vertex3f(-20.0f, -0.41f, -20.0f);
   floorBatch.End();
    
    
    //7.随机小球球顶点坐标数据
    for (int i = 0; i < NUM_SPHERES; i++) {
        
        //y轴不变，X,Z产生随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        //在y方向，将球体设置为0.0的位置，这使得它们看起来是飘浮在眼睛的高度
        //对spheres数组中的每一个顶点，设置顶点数据
        spheres[i].SetOrigin(x, 0.0f, z);
    }
    
    glGenTextures(3, uiTextures);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    LoadTGATexture("marble.tag", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    LoadTGATexture("marslike.tga", GL_LINEAR_MIPMAP_LINEAR,
                   GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    LoadTGATexture("moonlike.tga", GL_LINEAR_MIPMAP_LINEAR,
                   GL_LINEAR, GL_CLAMP_TO_EDGE);
    
       
   
}

void drawSomething(GLfloat yRot)
{
    static GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static GLfloat vLightPos[] = { 0.0f, 3.0f, 0.0f, 1.0f };
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    for (int i = 0; i < NUM_SPHERES; i++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, modelViewMatrix.GetMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vWhite, 0);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    
    modelViewMatrix.Translate(0.0, 0.2, -2.5);
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot, 0.0, 1.0, 0.0);
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, modelViewMatrix.GetMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vWhite, 0);
    torusBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot * -2.0, 0.0, 1.0, 0.0);
    modelViewMatrix.Translate(0.8, 0.0, 0.0);
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, modelViewMatrix.GetMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vWhite, 0);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix();
  
}

//进行调用以绘制场景
void RenderScene(void)
{

    static GLfloat vFloorColor[] = { 1.0f, 1.0f, 0.0f, 0.75f};
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    modelViewMatrix.PushMatrix();
    
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);
    
    modelViewMatrix.PushMatrix();
    
    modelViewMatrix.Scale(1.0, -1.0, 1.0);
    
    modelViewMatrix.Translate(0.0, 0.8, 0.0);
    
    glFrontFace(GL_CW);
    drawSomething(yRot);
    glFrontFace(GL_CCW);
    
    modelViewMatrix.PopMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, transformPipeline.GetModelViewProjectionMatrix(), vFloorColor, 0);
    floorBatch.Draw();
    
    glDisable(GL_BLEND);
    drawSomething(yRot);
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}


void SpeacialKeys(int key,int x,int y)
{
    
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));
    
    if (key == GLUT_KEY_UP) {
        
        //MoveForward 平移
        cameraFrame.MoveForward(linear);
    }
    
    if (key == GLUT_KEY_DOWN) {
        cameraFrame.MoveForward(-linear);
    }
    
    if (key == GLUT_KEY_LEFT) {
        //RotateWorld 旋转
        cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_RIGHT) {
        cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
    }
    
}


// 屏幕更改大小或已初始化
void ChangeSize(int nWidth, int nHeight)
{
    //1.设置视口
    glViewport(0, 0, nWidth, nHeight);
    
    //2.设置投影方式
    viewFrustum.SetPerspective(35.0f, float(nWidth)/float(nHeight), 1.0f, 100.0f);
    
    //3.将投影矩阵加载到投影矩阵堆栈,
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelViewMatrix.LoadIdentity();
    
    //4.将投影矩阵堆栈和模型视图矩阵对象设置到管道中
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
   
    
}

//删除纹理
void ShutdownRC(void)
{
    glDeleteTextures(3, uiTextures);
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    
    glutCreateWindow("OpenGL SphereWorld");
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpeacialKeys);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    
    SetupRC();
    glutMainLoop();
    ShutdownRC();
    return 0;
}
