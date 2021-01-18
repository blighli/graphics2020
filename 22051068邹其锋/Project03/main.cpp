
#include "ObjLoader.hpp"
#include <math.h>
using namespace std;

static int oldPosY = -1;
static int oldPosX = -1;
static bool mouseLeftDown;
static bool mouseRightDown;
static float times = 0.1;
static float cameraAngleX = 0.0;
static float cameraAngleY = 0.0;
static float cameraDistanceX = 0.0;
static float cameraDistanceY = 0.0;
bool l[] = {true, false, false, false, false};
static GLdouble axix = 0.0,axiy = 0.0,axiz = 6.0;
static GLdouble tarx = 0.0,tary = 0.0,tarz = 0.0;


// model path
string filePath = "monkey.obj";
ObjLoader objModel = ObjLoader(filePath);

void setLightRes() {
    
    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuseLignt[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat lightPos0[] = {0.0f, 0.0f, 10.0f, 0.0f};
    GLfloat lightPos1[] = {10.0f, 0.0f, 0.0f, 0.0f};
    GLfloat lightPos2[] = {-10.0f, 0.0f, 0.0f, 0.0f};
    GLfloat lightPos3[] = {0.0f, 10.0f, 0.0f, 0.0f};
    GLfloat lightPos4[] = {0.0f, -10.0f, 0.0f, 0.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLignt);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLignt);
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);
    glLightfv(GL_LIGHT3, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseLignt);
    glLightfv(GL_LIGHT3, GL_POSITION, lightPos3);
    glLightfv(GL_LIGHT4, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuseLignt);
    glLightfv(GL_LIGHT4, GL_POSITION, lightPos4);
    
    glEnable(GL_LIGHTING); //启用光源
}

//初始化
void init() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Project03");
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    setLightRes();
    glEnable(GL_DEPTH_TEST);
}

void display()
{
    glColor3f(1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    setLightRes();

    glLoadIdentity();
    gluLookAt (axix, axiy, axiz, tarx, tary, tarz, 0.0, 1.0, 0.0);
    glPushMatrix();
    glScalef(times, times, times);
    glTranslatef(cameraDistanceX, cameraDistanceY, -1000);
    glRotatef(cameraAngleX, 1, 0, 0);
    glRotatef(cameraAngleY, 0, 1, 0);
    // 绘制
    objModel.Draw();
    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (GLdouble)width / (GLdouble)height, 1.0f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
}


// 鼠标移动
void moseMove2(int button, int state, int x, int y)
{
    oldPosX = x; oldPosY = y;
    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }
    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
}

void changeViewPoint2(int x, int y)
{
    if(mouseLeftDown)
    {
        cameraAngleY += (x - oldPosX) * 0.5f;
        cameraAngleX += (y - oldPosY) * 0.5f;
        oldPosX = x;
        oldPosY = y;
    }
    if(mouseRightDown)
    {
        cameraDistanceX += (x - oldPosX) * 0.03f;
        cameraDistanceY -= (y - oldPosY) * 0.03f;
        oldPosY = y;
        oldPosX = x;
    }
}

// 键盘事件监听
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case '0':
            if(l[0]) {
                glDisable(GL_LIGHT0);
                l[0] = false;
            } else {
                glEnable(GL_LIGHT0);
                l[0] = true;
            }
            glutPostRedisplay();
            break;
        case '1':
            if(l[1]) {
                glDisable(GL_LIGHT1);
                l[1] = false;
            } else {
                glEnable(GL_LIGHT1);
                l[1] = true;
            }
            glutPostRedisplay();
            break;
        case '2':
            if(l[2]) {
                glDisable(GL_LIGHT2);
                l[2] = false;
            } else {
                glEnable(GL_LIGHT2);
                l[2] = true;
            }
            glutPostRedisplay();
            break;
        case '3':
            if(l[3]) {
                glDisable(GL_LIGHT3);
                l[3] = false;
            } else {
                glEnable(GL_LIGHT3);
                l[3] = true;
            }
            glutPostRedisplay();
            break;
        case '4':
            if(l[4]) {
                glDisable(GL_LIGHT4);
                l[4] = false;
            } else {
                glEnable(GL_LIGHT4);
                l[4] = true;
            }
            glutPostRedisplay();
            break;
            
        case 'd':
            axix += 0.3;
            tarx += 0.3;
            glutPostRedisplay();
            break;
        case 'a':
            axix -= 0.3;
            tarx -= 0.3;
            glutPostRedisplay();
            break;
        case 'w':
            axiz -= 0.3;
            tarz -= 0.3;
            glutPostRedisplay();
            break;
        case 's':
            axiz += 0.3;
            tarz += 0.3;
            glutPostRedisplay();
            break;
            
    }
}

void myIdle()
{
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(moseMove2);
    glutMotionFunc(changeViewPoint2);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(myIdle);
    glutMainLoop();
    return 0;
}
