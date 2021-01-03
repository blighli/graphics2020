#include <gl/GLUT.h>
#include <iostream>

static const GLfloat vertex_list[][3] = {
-0.5f, -0.5f, -0.5f,
0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, -0.5f,
0.5f, 0.5f, -0.5f,
-0.5f, -0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
-0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
};

static const GLint index_list[][4] = {
    0,2,3,1,
    0,4,6,2,
    0,1,5,4,
    4,5,7,6,
    1,3,7,5,
    2,6,7,3,
};
GLfloat xMov = 0.0f;
GLfloat winW = 0.0f;
GLfloat winH = 0.0f;
GLfloat zoom = 1.0f;
GLint rotFlag = 0;
GLint oldx;
GLint oldy;
float a = 0;
float b = 0;
float c = 0;
float cx=0, cy=0, cz=3;
float lx = 0, ly = 0, lz = 0;
float ux = 0, uy = 0, uz = 0;
float scale = 1.0f;

//显示函数
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除深度缓存
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    //设置光源
    GLfloat  lightcolor[] = { 0.8,0.7,0.2,1.0 };
    GLfloat  lightpos[] = { 5,5,8,1};
    glLightfv(GL_LIGHT0,GL_POSITION,lightpos);
    glLightfv(GL_LIGHT0,GL_AMBIENT,lightcolor);
    glEnable(GL_LIGHT0);

    //绘制茶壶
    //glColor3f(1.0,1.0,1.0);
    glScalef(scale, scale, scale);
    glutSolidTeapot(2);

    //绘制坐标系
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);  //x r
    glVertex3d(0, 0, 0);
    glVertex3d(100, 0, 0);
    glColor3f(0, 1, 0);//y g
    glVertex3d(0, 0, 0);
    glVertex3d(0, 100, 0);
    glColor3f(0, 0, 1);//z b
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, 100);
    glEnd();
    
    glFlush();
}

//窗口回调
void onReshape(int w ,int h) 
{
    GLfloat aspectRatio = (GLfloat)w / (GLfloat)h;
    winW = w;
    winH = h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-5.0f , 5.0f , -5.0f, 5.0f , -100.0f , 100.0f );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutSwapBuffers();
}
//像素
void onMouse(int button,int state,int x, int y) 
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float x, y, z;
        x = 3 * sin(a * 3.14 / 180);
        y = 3;
        z = 1;
        a += 5;
        while (a > 360) {
            a -= 360;
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(x,y,z, 0,0,1,0,0,1 );
        glutPostRedisplay();
    }
}
/*
键盘按键
x,y,z：分别绕对应轴旋转
w：放大
s：缩小
*/
void keyboard(unsigned char key, int m,int n) 
{
    switch (key)
    {
    case'x':
    case'X':
        cx = 1;
        cy = 3 * cos(a * 3.14 / 180);
        cz = 3 * sin(a * 3.14 / 180);
        a += 5;
        while (a > 360) {
            a -= 360;
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cx, cy, cz, 0, 0, 0, 0.1, 0,0 );
        break;
    case'y':
    case'Y':
        cx = 3 * sin(b * 3.14 / 180);
        cy = 1;
        cz = 3 * cos(b * 3.14 / 180);
        b += 5;
        while (b > 360) {
            b -= 360;
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cx, cy, cz, 0, 0, 0, 0, 0.1, 0);
        break;
    case'z':
    case'Z':
        cx = 3 * sin(c * 3.14 / 180);
        cy = 3 * cos(c * 3.14 / 180);
        cz = 1;
        c += 5;
        while (c > 360) {
            c -= 360;
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cx, cy, cz, 0, 0, 0, 0,0,0.1);
        break;
    case'w':
        scale += 0.1;
        if (scale >= 2) scale = 1.9f;
        std::cout << scale << "!" << std::endl;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        break;
    case's':
        scale -= 0.1;
        if (scale <= 0.2) scale = 0.2f;
        std::cout << scale << "." << std::endl;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        break;
    default:
        break;
    }
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);//单缓冲和RGB模式
    glutInitWindowSize(600,600);
    glutInitWindowPosition(300,300);
    glutCreateWindow("pengqi project01");

    glutDisplayFunc(display);   //设置绘制回调函数
    glutReshapeFunc(onReshape); //设置窗口大小改变时的回调函数

    //glutMouseFunc(onMouse);     //绑定鼠标点击回调
    glutKeyboardFunc(&keyboard);//绑定键盘响应回调
  
    glutMainLoop();
    return 0;
}
