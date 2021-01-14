
#include <GL/glut.h>

#define _USE_MATH_DEFINES //��Ҫ����mathǰ,֮��ſ���ʹ��M_PI��match�������
#include <math.h>

#include<GL/glaux.h>

#include<GL/glu.h>

#include"stdio.h"
#include"iostream"

using namespace std;
const GLint pickSize = 32;//ʰȡ�������Ĵ�С

int winWidth = 600, winHeight = 600;

// Lighting values
GLfloat  whiteLight[] = { 1.2f, 1.2f, 1.2f, 1.0f };
GLfloat  sourceLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
//��Դλ��
GLfloat  lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };


float light_angle = 0;//��Դ�Ƕ�

float light_radius = 8.0;//��Դ�뾶



static float year = 0, month = 0, day = 0;

double aix_x = 0.0, aix_y = 1, aix_z = 0;

GLuint texName;//��������

GLUquadricObj* qobj;//����������������

double sunx, suny, sunz;
double earthx, earthy, earthz;
double moonx, moony, moonz;




void IPosition(void);// ��Դλ��

void init(void);// ��ʼ����������Դ�����ʡ�����

void material_sun(void);//̫������

void material_earth(void);// �������

void material_moon(void);// �������

void sun(void);//����̫��

void earth(void);//���Ƶ���

void moon(void);//��������

void display(void);

void Rotate(void);   //������ת����

void myidle(void);   //���õ��ú���

void reshape(int w, int h);

void mykeyboard(unsigned char key, int x, int y);

void ProcessPicks(GLint nPicks, GLuint pickBuffer[]);

void MousePlot(GLint button, GLint action, GLint xMouse, GLint yMouse);


int main(int argc, char** argv)

{

	glutInit(&argc, argv);

	glutInitWindowSize(600, 600);

	glutInitWindowPosition(100, 100);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("̫����������");

	init();

	glutDisplayFunc(display);   //������ʾ�ص�����

	glutReshapeFunc(reshape);   // ���ù۲��巶Χ

	glutKeyboardFunc(mykeyboard); //���̻ص�����

	glutMouseFunc(MousePlot);

	glutIdleFunc(myidle);       // ���� �ص�����

	glutMainLoop();

	return 0;

}

//���ù�Դλ��

void IPosition()

{

	float y, z;

	y = light_radius * cos(light_angle);

	z = light_radius * sin(light_angle);

	float light_position[] = { 3.0,y,z,0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

}



//������Դ������������

void init(void)

{

	//IPosition(); //��Դλ��

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	//��ʱ��Ϊ���������
	glFrontFace(GL_CCW);
	//�����޳�Ч��������α��治�ù��մ���
	glEnable(GL_CULL_FACE);
	//�����ƹ�Ч��
	glEnable(GL_LIGHTING);
	//���õ���0
	//��������ɫ
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	//ɢ���ǿ��
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	//��Դλ��
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	//����
	glEnable(GL_LIGHT0);

	//glEnable(GL_COLOR_MATERIAL);//ʹ����ɫ����

	glClearDepth(1.0f);

	AUX_RGBImageRec *TextureImage[1];//����λͼ

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //����ε���ʾ��ʽ,ģʽ���������������������������ʽ

	glGenTextures(1, &texName);//glGenTextures (GLsizei n, GLuint *textures);������textures�з���n������δʹ�õ�ֵ,��ʾ������������ 

	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//������˺���������д����

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	memset(TextureImage, 0, sizeof(void *) * 1);//�����ǽ�ĳһ���ڴ��е�����ȫ������Ϊָ����ֵ

	if (TextureImage[0] = auxDIBImageLoadA("D:\\game\\CG\\earth2.bmp"))   //ʵ�ּ���λͼ�ļ�

	{

		//����ָ���Ĳ���������һ��2D����Texture����

		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

	}

	glMatrixMode(GL_PROJECTION);

}

void myidle(void)

{

	glutPostRedisplay();

}

void DrawRect(GLenum mode)
{
	IPosition();
	Rotate();
	if (mode == GL_SELECT)glPushName(1);//������1ѹ���ջ
	sun();
	if (mode == GL_SELECT)glPushName(2);//������2ѹ���ջ
	earth();
	if (mode == GL_SELECT)glPushName(3);//������3ѹ���ջ
	moon();
	glutSwapBuffers();

	
}

void display(void)

{

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawRect(GL_RENDER);//����Ⱦģʽ����ͼ��
	glFlush();
}

//̫������

void material_sun(void)

{

	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //���ʾ��淴����ɫ����

	GLfloat mat_shininess[] = { 50.0 };                   // ���淴��ָ������

	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //����ɢ����ɫ	 

	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };

	GLfloat lmodel_ambient[] = { 1.0,0.0,0.0,1.0 };       //̫����ɫΪ��ɫ     

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //���ʾ��淴����ɫ

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //���淴��ָ��

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //���ʵ�ɢ����ɫ

	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //���ʵĻ�����ɫ 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  ���������Ļ������RGBAǿ��

}

//�������

void material_earth(void)

{

	glEnable(GL_COLOR_MATERIAL);

	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //���ʾ��淴����ɫ����

	GLfloat mat_shininess[] = { 50.0 };                   // ���淴��ָ������

	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //����ɢ����ɫ	 

	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };

	GLfloat lmodel_ambient[] = { 0.0,0.0,1.0,1.0 };       //������ɫΪ��ɫ     

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //���ʾ��淴����ɫ

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //���淴��ָ��

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //���ʵ�ɢ����ɫ

	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //���ʵĻ�����ɫ 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  ���������Ļ������RGBAǿ��

}

//�������

void material_moon(void)

{

	glEnable(GL_COLOR_MATERIAL);

	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //���ʾ��淴����ɫ����

	GLfloat mat_shininess[] = { 50.0 };                   // ���淴��ָ������

	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //����ɢ����ɫ	 

	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };



	GLfloat lmodel_ambient[] = { 0.5,0.5,0.5,1.0 };       //������ɫΪ��ɫ     





	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //���ʾ��淴����ɫ

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //���淴��ָ��

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //���ʵ�ɢ����ɫ

	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //���ʵĻ�����ɫ 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  ���������Ļ������RGBAǿ��

}

//����̫��

void sun()

{
	GLdouble mProj[16];
	GLdouble mView[16];
	GLint mPort[4];
	GLdouble wx, wy, wz;
	
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glColor3ub(255, 97, 3);
	material_sun();

	glRotatef((GLfloat)year, 0.0, 1.0, 0.0);   //̫����ת	

	glutSolidSphere(0.3, 60, 60);                //������   

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//�õ���ͼ��ͶӰ���ӿڵľ���
	glGetDoublev(GL_PROJECTION_MATRIX, mProj);
	glGetDoublev(GL_MODELVIEW_MATRIX, mView);
	glGetIntegerv(GL_VIEWPORT, mPort);
	//ģ��ͶӰ����ͼ��ģ�ͱ任
	gluProject(0, 0, 0, mView, mProj, mPort, &wx, &wy, &wz);
	sunx = wx;
	suny = wy;
	sunz = wz;


	glPopMatrix();

}

//���Ƶ��������

void earth()

{
	GLdouble mProj[16];
	GLdouble mView[16];
	GLint mPort[4];
	GLdouble wx, wy, wz;

	qobj = gluNewQuadric();//����ռ�
	glPushMatrix();
	material_earth();
	glBindTexture(GL_TEXTURE_2D, texName);//������һ���󶨵�Ŀ������������Ƶ�����

	glEnable(GL_TEXTURE_2D);//���ö�ά����

	gluQuadricTexture(qobj, GL_TRUE);//������



	glRotatef((GLfloat)day, aix_x, aix_y, aix_z);//��ת

	glTranslatef(1.0, 0.0, 0.0);//ƽ��

	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//��ת

	//������̫���Ĺ�ת ����//���Ƶ���

	//glutSolidSphere(0.1, 20, 16);
	gluSphere(qobj, 0.2, 20, 16); //��������qobj

	glDisable(GL_TEXTURE_2D);//���ö�ά����

	//�õ���ͼ��ͶӰ���ӿڵľ���
	glGetDoublev(GL_PROJECTION_MATRIX, mProj);
	glGetDoublev(GL_MODELVIEW_MATRIX, mView);
	glGetIntegerv(GL_VIEWPORT, mPort);
	//ģ��ͶӰ����ͼ��ģ�ͱ任
	gluProject(0, 0, 0, mView, mProj, mPort, &wx, &wy, &wz);
	earthx = wx;
	earthy = wy;
	earthz = wz;


	glPopMatrix();

}

void moon()

{
	GLdouble mProj[16];
	GLdouble mView[16];
	GLint mPort[4];
	GLdouble wx, wy, wz;

	glPushMatrix();

	material_moon();

	glRotatef((GLfloat)day, aix_x, aix_y, aix_z);//��ת

	glTranslatef(1.0, 0.0, 0.0);//ƽ��

	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//��ת

	glRotatef((GLfloat)day, 0, 1, 0);//��ת

	glTranslatef(0.3, 0, 0);

	glutSolidSphere(0.075, 20, 16);

	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//��ת

	//�õ���ͼ��ͶӰ���ӿڵľ���
	glGetDoublev(GL_PROJECTION_MATRIX, mProj);
	glGetDoublev(GL_MODELVIEW_MATRIX, mView);
	glGetIntegerv(GL_VIEWPORT, mPort);
	//ģ��ͶӰ����ͼ��ģ�ͱ任
	gluProject(0, 0, 0, mView, mProj, mPort, &wx, &wy, &wz);
	moonx = wx;
	moony = wy;
	moonz = wz;

	glPopMatrix();

}




void Rotate(void)          //���ø����ǵĹ�ת����

{

	year = year + 0.2;

	if (year > 360.0)

	{

		year = year - 360.0;

	}

	day = day + 0.2;

	if (day > 360.0)

	{

		day = day - 360.0;

	}

	month = month + 0.5;

	if (month > 360)

		month -= 360;

	glutPostRedisplay();



}

void reshape(int w, int h)

{

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);

	//glOrtho(-30.0, 30.0, -30.0, 30.0, -30.0, 30.0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

}

void mykeyboard(unsigned char key, int x, int y)

{

	switch (key)

	{

	case 'W':

	case 'w':

		aix_y -= 0.01;

		aix_z += 0.01;

		break;

	case 'S':

	case 's':

		aix_y += 0.01;

		aix_z -= 0.01;

		break;

	case 'A':

	case 'a':

		light_angle -= 1.0;

		break;

	case 'D':

	case 'd':

		light_angle += 1.0;

		break;

	default:

		break;

	}

	glutPostRedisplay();

}

void ProcessPicks(GLint nPicks, GLuint pickBuffer[])
{
	GLint i;
	GLuint name, *ptr;
	printf("ѡ�е���ĿΪ%d��\n", nPicks);
	ptr = pickBuffer;
	for (i = 0; i < nPicks; i++)
	{
		name = *ptr;//ѡ��ͼԪ�ڶ�ջ�е�λ��
		ptr += 3;//�������ֺ������Ϣ
		ptr += name - 1;//����λ����Ϣ���ѡ�е�ͼԪ����
		if (*ptr == 1)printf("��ѡ����̫��\n");
		if (*ptr == 2)printf("��ѡ���˵���\n");
		if (*ptr == 3)printf("��ѡ��������\n");
		ptr++;
	}
	printf("\n\n");
}

void MousePlot(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	GLint    viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	int winX;
	int winY;
	GLdouble posX, posY, posZ;
	float nearPointx, nearPointy, nearPointz;
	float farPointx, farPointy, farPointz;

	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN)
	{

		glGetIntegerv(GL_VIEWPORT, viewport); // �õ��������һ�������ӿڵĲ���
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glutPostRedisplay();


		winX = xMouse;

		winY = winHeight - yMouse;



		//��ȡ���ض�Ӧ��ǰ�ü���ĵ�����

		bool bResult = gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &posX, &posY, &posZ);

		

		nearPointx = posX; nearPointy = posY; nearPointz = posZ;

		//��ȡ���ض�Ӧ�ĺ�ü���ĵ�����

		bResult = gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &posX, &posY, &posZ);


		farPointx = posX; farPointy = posY; farPointz = posZ;

		if(pow(nearPointx,2)+ pow(nearPointy, 2)< pow(0.3, 2))
			printf("��ѡ����̫��\n");
		if (pow(nearPointx-cos(day*M_PI/180), 2) + pow(nearPointy, 2) < pow(0.2, 2))
			printf("��ѡ���˵���\n");
		if (pow(nearPointx - cos(day*M_PI / 180) - 0.3*cos(month*M_PI / 180), 2) + pow(nearPointy, 2) < pow(0.075, 2))
			printf("��ѡ��������\n");

	}
}