#include <stdlib.h>
#include "glut.h"
#include<iostream>
using namespace std;

void Draw_Scene()
{
	// 绘制茶壶
	glPushMatrix();
	glTranslatef(-1, -5, 0);
	glRotatef(90, 1, 0, 0); 
	glutSolidTeapot(1);
	glPopMatrix();

	// 绘制立方体
	glPushMatrix();
	glTranslatef(0 , 0 , 0);
	glScalef(3, 3, 3);
	glutSolidCube(1.0);
	glPopMatrix();

	// 绘制球体
	glPushMatrix();
	glTranslatef(-5, 0, 0);
	glScalef(1, 1, 1);
	glutSolidSphere(1, 20,30);
	glPopMatrix();

}

float rr = 0;  //相机运动所在球体的 '经度'
float rt = 0;  //相机运动所在球体的 '纬度'
float camera2obj_len  = 10; //  初始化相机离物体的距离为10个单位长度
float eye[]     = { 0.0, 0.0, 10.0 };  // 初始化相机位置
float center[]  = { 0.0, 0.0, 0.0 };   // 固定相机朝向坐标系的中心，也就是物体所摆放的位置

void key(unsigned char k, int x, int y)
{
	switch (k)
	{
			//WA SD ZC：控制相机上下左右前后移动
			case 'd': {   //相机绕物体向右转，可查看物体的右侧
				rr += 0.05;
				eye[0] = camera2obj_len * sin(rr) * cos(rt);
				eye[2] = camera2obj_len * cos(rr) * cos(rt);
				eye[1] = camera2obj_len * sin(rt);
				break;
			}
			case 'a': {  //相机绕物体向左转，可查看物体的左侧
				rr -= 0.05;
				eye[0] = camera2obj_len * sin(rr) * cos(rt);
				eye[2] = camera2obj_len * cos(rr) * cos(rt);
				eye[1] = camera2obj_len * sin(rt);
				break;
			}
			case 'w': {  //相机绕物体向上转，可查看物体的上侧
				if (rt < 1.5) rt += 0.035;
				eye[0] = camera2obj_len * sin(rr)* cos(rt);
				eye[2] = camera2obj_len * cos(rr)* cos(rt);
				eye[1] = camera2obj_len * sin(rt);
				break;
			}
			case 's': { //相机绕物体向下转，可查看物体的下方
				if (rt > -1.5) rt -= 0.035;
				eye[0] = camera2obj_len * sin(rr) * cos(rt);
				eye[2] = camera2obj_len * cos(rr) * cos(rt);
				eye[1] = camera2obj_len * sin(rt);
				break;
			}
			case 'c': {  //相机远离物体观察
				camera2obj_len += 1;
				eye[0] = camera2obj_len * sin(rr) * cos(rt);
				eye[2] = camera2obj_len * cos(rr) * cos(rt);
				eye[1] = camera2obj_len * sin(rt);
				break;
			}
			case 'z': { //相机靠向物体观察
				camera2obj_len -= 1;
				eye[0] = camera2obj_len * sin(rr) * cos(rt);
				eye[2] = camera2obj_len * cos(rr) * cos(rt);
				eye[1] = camera2obj_len * sin(rt);
				break;
			}
			
			// 退出观察，关闭窗口
			case 'q': {exit(0); break; }
	}

}


void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 1.0, 1.0, 0.0, 1.0 };
	GLfloat light_pos[] = { 5,5,7,1 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);

	glRotatef(0 , 0, 1.0f, 0);			
	glRotatef(-90, 1, 0, 0);
	glScalef(0.2, 0.2, 0.2);

	Draw_Scene();						
	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}


int wHeight = 0;
int wWidth = 0;


void updateView(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float whRatio = (GLfloat)width / (GLfloat)height;
	gluPerspective(30, 1, 1, -5);
	glMatrixMode(GL_MODELVIEW);
}

void reshape(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	wHeight = height;
	wWidth = width;
	updateView(wHeight, wWidth);
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1000, 1000);
	int windowHandle = glutCreateWindow("Simple OpenGL APP");

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(key);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}






