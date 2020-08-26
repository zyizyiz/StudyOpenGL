#include <iostream>
#include <GLUT/GlUT.h>
#include "math3d.h"

void draw()
{
    /// 正方形
//    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//    glColor3f(1.0f, 0.0f, 0.0f);
//
//    glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
//
//    glBegin(GL_POLYGON);
//
//    glVertex3f(0.25f, 0.25f, 0.0f);
//    glVertex3f(0.75f, 0.25f, 0.0f);
//    glVertex3f(0.75f, 0.75f, 0.0f);
//    glVertex3f(0.25f, 0.75f, 0.0f);
//
//    glEnd();
//
//    glFlush();
    
    /// 圆形
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    
    const int n = 55;
    const GLfloat R = 0.5f;
    const GLfloat pi = 3.1415926f;
    for (int i = 0; i < n; i++) {
        glVertex2f(R * cos(2 * pi / n * i), R * sin(2 * pi / n * i));
    }
    
    glEnd();
    
    glFlush();
}

int main(int argc,const char *argv[])
{

    //1.初始化一个GLUT库
    glutInit(&argc, (char **)argv);
    
    //2.创建一个窗口并制定窗口名
    glutCreateWindow("CC_Window");
    
    //3.注册一个绘图函数，操作系统在必要时刻就会对窗体进行重绘制操作
    //它设置了一个显示回调（diplay callback），即GLUT在每次更新窗口内容的时候回自动调用该例程
    glutDisplayFunc(draw);
    
    //这是一个无限执行的循环，它会负责一直处理窗口和操作系统的用户输入等操作。（注意：不会执行在glutMainLoop()之后的所有命令。）
    glutMainLoop();
    
    return 0;
}
