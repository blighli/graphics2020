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

#include <iostream>
#include <stdlib.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <glut.h>
#include <math.h>
#include <Windows.h>
#include <string>
#define  GLUT_WHEEL_UP 3     
#define  GLUT_WHEEL_DOWN 4
using namespace std;
typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

//鼠标交互有关的
int mousetate = 0; //鼠标当前的状态
GLfloat Oldx = 0.0; // 点击之前的位置
GLfloat Oldy = 0.0;
GLuint texture;
//与实现角度大小相关的参数，只需要两个就可以完成
float xRotate = 0.0f;   //旋转
float yRotate = 0.0f;
float ty = 0.0f;
float tx = 0.0f;
float scale = 0.004;

//文件读取有关的
MyMesh mesh;  //mesh把文件读取了，封装在mesh对象中
//"dinosaur.obj";
const string  file = "";
const string file_1 = file + "cube.obj";
const string file_2 = file + "bunny.obj";
const string file_3 = file + "Eight.obj";

int currentfile = 1;

GLuint showFaceList, showWireList;
int showstate = 1;
bool showFace = true;
bool showWire = false;
bool showFlatlines = false;

void setLightRes() {
    GLfloat lightPosition[] = { 0.0f, 1.0f, 0.0f, 0.0f }; // 平行光源, GL_POSITION属性的最后一个参数为0
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHTING); //启用光源
    glEnable(GL_LIGHT0);   //使用指定灯光
}
void SetupRC()
{
    //当你想剔除背面的时候，你只需要调用glEnable(GL_CULL_FACE)就可以了，OPENGL状态机会自动按照默认值进行CULL_FACE，
    //默认是glFrontFace（GL_CCW）  GL_CCW逆时针为正，GL_CW顺时针
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    // 启用光照计算
    glEnable(GL_LIGHTING);
    // 指定环境光强度（RGBA）  此时可以控制模型的显示颜色
    GLfloat ambientLight[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    // 设置光照模型，将ambientLight所指定的RGBA强度值应用到环境光
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    // 启用颜色追踪
    //GL_COLOR_MATERIAL使我们可以用颜色来贴物体。如果没有这行代码，纹理将始终保持原来的颜色，glColor3f(r,g,b)就没有用了
    glEnable(GL_COLOR_MATERIAL);
    // 设置多边形正面的环境光和散射光材料属性，追踪glColor
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

//初始化顶点和面   
void initGL()
{
    //实用显示列表
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(2.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST); //用来开启深度缓冲区的功能，启动后OPengl就可以跟踪Z轴上的像素，那么它只有在前面没有东西的情况下才会绘制这个像素，在绘制3d时，最好启用，视觉效果会比较真实
    
    setLightRes();//启用指定光源
    SetupRC();//设置环境光
    showFaceList = glGenLists(1);//创建分配一个显示列表
    showWireList = glGenLists(1);
    //int temp = mesh.n_edges();

    //下面都是绘制编译  等待 callList调用显示列表来进行绘制显示
    // 绘制 wire 
    glNewList(showWireList, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(0.5f, 0.5f, 0.5f);//灰色
    glBegin(GL_LINES);
    //边线三维图绘制
    for (MyMesh::HalfedgeIter he_it = mesh.halfedges_begin(); he_it != mesh.halfedges_end(); ++he_it) {
        //链接这个有向边的起点和终点
        glVertex3fv(mesh.point(mesh.from_vertex_handle(*he_it)).data());
        glVertex3fv(mesh.point(mesh.to_vertex_handle(*he_it)).data());
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEndList();

    // 绘制flat
    glNewList(showFaceList, GL_COMPILE);
    //三角面图绘制
    for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        glBegin(GL_TRIANGLES);
        for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
            glNormal3fv(mesh.normal(*fv_it).data());  //为每个顶点都指定法线向量，使得成像更立体
            glVertex3fv(mesh.point(*fv_it).data());
        }
        glEnd();
    }
    glEndList();
}




// 当窗体改变大小的时候，改变窗口大小时保持图形比例
void myReshape(GLint w, GLint h)
{
    glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));//跨平台进行强制类型转换 在c++下无需转换
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-static_cast<GLdouble>(w) / h, static_cast<GLdouble>(w) / h, -1.0, 1.0, -100.0, 100.0);
    else
        glOrtho(-1.0, 1.0, -static_cast<GLdouble>(h) / w, static_cast<GLdouble>(h) / w, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// 读取文件的函数
void readfile(string file) {
    // 请求顶点法线 vertex normals
    mesh.request_vertex_normals();
    //如果不存在顶点法线，则报错 
    if (!mesh.has_vertex_normals())
    {
        cout << "错误：标准定点属性 “法线”不存在" << endl;
        return;
    }
    // 如果有顶点法线则读取文件，读入到mesh对象中
    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(mesh, file, opt))
    {
        cout << "无法读取文件:" << file << endl;
        return;
    }
    else cout << "成功读取文件:" << file << endl;
    cout << endl; // 为了ui显示好看一些
    
    if (!opt.check(OpenMesh::IO::Options::VertexNormal))
    {
        // 通过面法线计算顶点法线
        mesh.request_face_normals();
        // mesh计算出顶点法线
        mesh.update_normals();
        // 释放面法线
        mesh.release_face_normals();
    }
}

//  键盘交互 1. 切换文件 2.切换显示
void mySpecial(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        cout << "读取文件:" << file_1 << " 中......" << endl;
        readfile(file_1);
        scale = 0.004;
        currentfile = 1;
        initGL();
        break;
    case GLUT_KEY_F2:
        cout << "读取文件:" << file_2 << " 中......" << endl;
        readfile(file_2);
        scale = 0.003;
        currentfile = 2;
        initGL();
        break;
    case GLUT_KEY_F3:
        cout << "读取文件:" << file_3 << " 中......" << endl;
        readfile(file_3);
        scale = 0.02;
        currentfile = 3;
        initGL();
        break;
    
    case GLUT_KEY_INSERT:
        if (showFace == true) {
            showFace = false;
            showWire = true;

            cout << "切换显示模式为：WireFrame" << endl;
        }
        else if (showWire == true)
        {
            showWire = false;
            showFlatlines = true;
            cout << "切换显示模式为：Flatlines" << endl;
        }
        else if (showFlatlines == true) {
            showFlatlines = false;
            showFace = true;
            //DisplaySphere(0.5, "G:\\source\\OpenGL\\Cow\\checkerboard3.bmp");
            cout << "切换显示模式为：Flat" << endl;
        }
        break;
    case GLUT_KEY_LEFT:
        tx -= 0.01;
        break;
    case GLUT_KEY_RIGHT:
        tx += 0.01;
        break;
    case GLUT_KEY_UP:
        ty += 0.01;
        break;
    case GLUT_KEY_DOWN:
        ty -= 0.01;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// 鼠标交互
void myMouse(int button, int state, int x, int y)
{    //鼠标左键按下或者松开
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mousetate = 1;
        Oldx = x;
        Oldy = y;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        mousetate = 0;
    
    if (state == GLUT_UP && button == GLUT_WHEEL_UP) {
        //cout << "hello" << endl; 
        //滑轮向上滑，则scale减小
        if (currentfile == 1)
            scale += 0.0005;
        if (currentfile == 2)
            scale += 0.001;
        if (currentfile == 5) {
            scale += 0.1;
        }
        else
            scale += 0.001;
    }
    if (state == GLUT_UP && button == GLUT_WHEEL_DOWN) {
        //cout << "good" << endl;
        //滑轮向下滑，则scale增加
        if (currentfile == 1)
            scale -= 0.0005;
        if (currentfile == 2)
            scale -= 0.001;
        if (currentfile == 5) {
            scale -= 0.1;
        }
        else
            scale -= 0.001;
    }
    glutPostRedisplay();//促使主程序尽快的重绘窗口
}

// 鼠标运动时
void onMouseMove(int x, int y) {
    //当鼠标状态为按下时进入后续判断
    if (mousetate) {
        //x对应y是因为对应的是法向量
        yRotate += x - Oldx;
        glutPostRedisplay();//标记当前窗口需要重新绘制
        Oldx = x;
        xRotate += y - Oldy;
        glutPostRedisplay();
        Oldy = y;
    }
}

void myDisplay()
{
    //要清除之前的深度缓存
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //与显示相关的函数
    glRotatef(xRotate, 1.0f, 0.0f, 0.0f); // 让物体旋转的函数 第一个参数是角度大小，后面的参数是旋转的法向量
    glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
    //glTranslatef(0.0f, 0.0f, ty);
    glTranslatef(tx, ty, 0);        //移动
    glScalef(scale, scale, scale); // 缩放 x,y,z分别乘以scale

                                   
    if (showFace)
        glCallList(showFaceList);
    if (showFlatlines) {
        glCallList(showFaceList);
        glCallList(showWireList);
    }
    if (showWire)
        glCallList(showWireList);

    glutSwapBuffers(); //这是Opengl中用于实现双缓存技术的一个重要函数
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // GLUT_Double 表示使用双缓存而非单缓存
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 500);
    glutCreateWindow("Mesh Viewer");
    //一开始默认读取文件1
    //readfile(file_1);
    //initGL();
    glutMouseFunc(myMouse);        //鼠标点击处理函数
    glutMotionFunc(onMouseMove); // 鼠标移动的时候的函数
    glutSpecialFunc(&mySpecial);//键盘上下左右响应
    glutReshapeFunc(&myReshape);//自适应窗口大小的改变 使得模型不会太宽 或者太高 若要精确显示模型不建议此操作
    glutDisplayFunc(&myDisplay);

    glutMainLoop();
    return 0;
}