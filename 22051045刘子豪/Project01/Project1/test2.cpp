#include <stdlib.h>  
#include <GL/glut.h>  
#include<iostream>
//#pragma comment(lib, "glut32.lib")  
using namespace std;
GLfloat roate = 0.0;//������ת����
GLfloat rote = 0.0;//������ת�Ƕ�
GLfloat anglex = 0.0;//X ����ת
GLfloat angley = 0.0;//Y ����ת
GLfloat anglez = 0.0;//Z ����ת
GLint WinW = 400;
GLint WinH = 400;
GLfloat oldx;//���������ʱ��¼�������  
GLfloat oldy;
GLfloat oldx1;//���Ҽ�����ʱ��¼�������  
GLfloat oldy1;
GLfloat cameraDistance = 0;
int flag = -1;
int flag1 = -1;
void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0); //������ɫ  
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.5, 1.0); //������ɫ
	glLoadIdentity();  //���ص�λ����  
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
			oldx = x;//���������ʱ��¼�������  
			oldy = y;
			flag1 = 0;
			cout << "left" << endl;
		}

	}
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			oldx1 = x;//���Ҽ�����ʱ��¼�������  
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
		anglex += 360 * (GLfloat)deltax / (GLfloat)WinW;//������Ļ����껬���ľ�����������ת�ĽǶ�  
		angley += 360 * (GLfloat)deltay / (GLfloat)WinH;
		anglez += 360 * (GLfloat)deltay / (GLfloat)WinH;
		oldx = x;//��¼��ʱ��������꣬�����������  
		oldy = y;//����û����������䣬��������ת���ò��ɿ�  
	}
	else
	{
		cameraDistance += (oldy1 - y)*0.2f; //������Ļ����껬���ľ���������ƽ�Ƶľ���
		oldx1 = x;//��¼��ʱ��������꣬����������� 
		oldy1 = y;
	}
	
	glutPostRedisplay();

}
int main(int argc, char** argv)
{
	cout << "����0�����߿���������1����ʵ�Ĳ��:" << endl;
	cin >> flag;
	while (flag != 0 && flag != 1)
	{
		cout << "���������룺" << endl;
		cin >> flag;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("�������");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(display);
	glutMainLoop();
	return 0;
}
