#include <stdlib.h>  
#include <GL/glut.h>  
#include<iostream>
//#pragma comment(lib, "glut32.lib")  
using namespace std;
GLfloat roate = 0.0;//设置旋转速率
GLfloat rote = 0.0;//设置旋转角度
GLfloat anglex = 0.0;//X 轴旋转
GLfloat angley = 0.0;//Y 轴旋转
GLfloat anglez = 0.0;//Z 轴旋转
GLint WinW = 400;
GLint WinH = 400;
GLfloat oldx;//当左键按下时记录鼠标坐标  
GLfloat oldy;
GLfloat oldx1;//当右键按下时记录鼠标坐标  
GLfloat oldy1;
GLfloat cameraDistance = 0;
int flag = -1;
int flag1 = -1;
void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0); //背景黑色  
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.5, 1.0); //画笔蓝色
	glLoadIdentity();  //加载单位矩阵  
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glTranslatef(0, 0, cameraDistance);
	glRotatef(rote, 0.0f, 1.0f, 0.0f);
	glRotatef(anglex, 1.0, 0.0, 0.0);
	glRotatef(angley, 0.0, 1.0, 0.0);
	glRotatef(anglez, 0.0, 0.0, 1.0);
	if(flag)
		glutSolidTeapot(2);
	else
		glutWireTeapot(2);
	rote += roate;
	//glRotatef(angle, 0.0, 1.0, 0.0);
	//angle += 1.0f;
	glutSwapBuffers();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			roate = 0;
			rote = 0;
			oldx = x;//当左键按下时记录鼠标坐标  
			oldy = y;
			flag1 = 0;
			cout << "left" << endl;
		}

	}
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			oldx1 = x;//当右键按下时记录鼠标坐标  
			oldy1 = y;
			flag1 = 1;
			cout << "right" << endl;
		}

	}

}
void motion(int x, int y)
{
	if (!flag1) 
	{
		GLint deltax = oldx - x;
		GLint deltay = oldy - y;
		anglex += 360 * (GLfloat)deltax / (GLfloat)WinW;//根据屏幕上鼠标滑动的距离来设置旋转的角度  
		angley += 360 * (GLfloat)deltay / (GLfloat)WinH;
		anglez += 360 * (GLfloat)deltay / (GLfloat)WinH;
		oldx = x;//记录此时的鼠标坐标，更新鼠标坐标  
		oldy = y;//若是没有这两句语句，滑动是旋转会变得不可控  
	}
	else
	{
		cameraDistance += (oldy1 - y)*0.2f; //根据屏幕上鼠标滑动的距离来设置平移的距离
		oldx1 = x;//记录此时的鼠标坐标，更新鼠标坐标 
		oldy1 = y;
	}
	
	glutPostRedisplay();

}
int main(int argc, char** argv)
{
	cout << "输入0绘制线框茶壶，输入1绘制实心茶壶:" << endl;
	cin >> flag;
	while (flag != 0 && flag != 1)
	{
		cout << "请重新输入：" << endl;
		cin >> flag;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("茶壶绘制");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(display);
	glutMainLoop();
	return 0;
}
