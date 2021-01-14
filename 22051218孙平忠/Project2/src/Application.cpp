//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//
//#include <iostream>
//int main(void)
//{
//    GLFWwindow* window;
//
//    /* Initialize the library */
//    if (!glfwInit())
//        return -1;
//
//    
//
//    /* Create a windowed mode window and its OpenGL context */
//    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
//    if (!window)
//    {
//        glfwTerminate();
//        return -1;
//    }
//
//    /* Make the window's context current */
//    glfwMakeContextCurrent(window);
//
//    if (glewInit() != GLEW_OK)
//        std::cout << "ERROR!" << std::endl;
//
//    std::cout << glGetString(GL_VERSION) << std::endl;
//
//    float positions[6] = {
//        -0.5f, -0.5f,
//         0.0f,  0.5f,
//         0.5f, -0.5f
//    };
//    unsigned int buffer;
//    glGenBuffers(1, &buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, buffer);
//    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
//
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    /* Loop until the user closes the window */
//    while (!glfwWindowShouldClose(window))
//    {
//        /* Render here */
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//
//        /* Swap front and back buffers */
//        glfwSwapBuffers(window);
//
//        /* Poll for and process events */
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//}

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h>
#include <glaux.h>


using namespace std;
GLuint texName;//纹理名称
int year = 0, day = 0;//公转自转
int speed = 100;
float eye[] = { 5.5, 1.7, 1.7 };
float center[] = { 0, 0, 0 };
int wHeight = 0;
int wWidth = 0;
int state = 1;
GLUquadricObj* g_text;
void lPosition() {//太阳为光源
    GLfloat sun_light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //光源的位置在世界坐标系圆心，齐次坐标形式
    GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
    GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //RGBA模式的漫反射光，全白光
    GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  //RGBA模式下的镜面光 ，全白光
    glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
}
void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    lPosition();
    glShadeModel(GL_SMOOTH);
    //开启光照
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);//不然光源不亮了

    AUX_RGBImageRec* TextureImage[1];//加载位图
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //多边形的显示方式,模式将适用于物体的所有面采用填充形式
    glGenTextures(1, &texName);//glGenTextures (GLsizei n, GLuint *textures);在数组textures中返回n个当期未使用的值,表示纹理对象的名称 
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//纹理过滤函数，必须写两遍
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    memset(TextureImage, 0, sizeof(void*) * 1);//作用是将某一块内存中的内容全部设置为指定的值
    if (TextureImage[0] = auxDIBImageLoadA("tex.bmp")) {  //实现加载位图文件
        //根据指定的参数，生成一个2D纹理（Texture）。
        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
    }
    glMatrixMode(GL_PROJECTION);
    g_text = gluNewQuadric();
}

void sun() {
    glPushMatrix();
    GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat lmodel_ambient[] = { 1.0,0.0,0.0,1.0 };        //太阳颜色为红色      
    GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glRotatef((GLfloat)day, 0.0, 1.0, 0.0);
    gluSphere(g_text, 0.7, 32, 32);
    glPopMatrix();
}
void earth() {
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texName);//允许建立一个绑定到目标纹理的有名称的纹理。
    glEnable(GL_TEXTURE_2D);//启用二维纹理
    gluQuadricTexture(g_text, GL_TRUE);//纹理函数
    GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat lmodel_ambient[] = { 0.1,0.2,0.6,1.0 };
    GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glRotatef((GLfloat)year / 10, 0.0, 0.0, 1.0);
    glTranslatef(2.1, 0.0, 0.0);
    glRotatef((GLfloat)day / 10, 0.0, 0.0, 1.0);
    gluSphere(g_text, 0.25, 20, 16);
    glDisable(GL_TEXTURE_2D);
    GLfloat mat_specular1[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess1[] = { 50.0 };
    GLfloat lmodel_ambient1[] = { 1.0,1.0,0.0,1.0 };
    GLfloat white_light1[] = { 1.0, 1.0,1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient1);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient1);
    glRotatef((GLfloat)year / 10, 0.0f, 0.0f, 1.0f);
    glRotatef(day / 30.0 * 360.0, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.3f, 0.0f, 0.0f);
    gluSphere(g_text, 0.06, 20, 16);
    glPopMatrix();
}



void redraw(void) {//画模型
    lPosition();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(1.0, 1.0, 1.0);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    sun();
    earth();
    glutSwapBuffers();
}

void cPosition() {//相机位置
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2],
        center[0], center[1], center[2],
        0.0, 0.0, 1.0);

}

void updateView(int width, int height) {

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
    //glOrtho(-3, 3, -3, 3, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    cPosition();
}
void reshape(int width, int height) {
    wHeight = height;
    wWidth = width;
    updateView(wWidth, wHeight);

}

void move() {
    year = (year + 8) % 3600;
    day = (day + 30) % 3600;
}
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            eye[0] += 1;
            center[0] += 1;
            cPosition();
            break;
        case 'a':
            eye[1] += 1;
            center[1] += 1;
            cPosition();
            break;
        case 's':
            eye[0] -= 1;
            center[0] -= 1;
            cPosition();
            break;
        case 'd':
            eye[1] -= 1;
            center[1] -= 1;
            cPosition();
            break;
        case 'z':
            eye[2] -= 1;
            center[2] -= 1;
            cPosition();
            break;
        case 'c':
            eye[2] += 1;
            center[2] += 1;
            cPosition();
            break;
        case 'r'://回到中心位置
            eye[0] = 5.5;
            eye[1] = 1.7;
            eye[2] = 1.7;
            center[0] = 0;
            center[1] = 0;
            center[2] = 0;
            cPosition();
            break;
        case 27: //esc
            exit(0);
            break;
    }
}

void myIdle(void) {
    Sleep(speed);
    move();
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    cout << "WASDZC控制前后左右上下方向" << endl;
    cout << "R重置相机" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1080, 680);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("太阳系");
    init();
    glutDisplayFunc(redraw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(&myIdle);
    glutMainLoop();
    return 0;
}
