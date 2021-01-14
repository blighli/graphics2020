
#include <GL/glut.h>

#define _USE_MATH_DEFINES //需要放在math前,之后才可以使用M_PI等match定义参数
#include <math.h>

#include<GL/glaux.h>

#include<GL/glu.h>

#include"stdio.h"
#include"iostream"

using namespace std;
const GLint pickSize = 32;//拾取缓冲区的大小

int winWidth = 600, winHeight = 600;

// Lighting values
GLfloat  whiteLight[] = { 1.2f, 1.2f, 1.2f, 1.0f };
GLfloat  sourceLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
//光源位置
GLfloat  lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };


float light_angle = 0;//光源角度

float light_radius = 8.0;//光源半径



static float year = 0, month = 0, day = 0;

double aix_x = 0.0, aix_y = 1, aix_z = 0;

GLuint texName;//纹理名称

GLUquadricObj* qobj;//二次曲面声明类型

double sunx, suny, sunz;
double earthx, earthy, earthz;
double moonx, moony, moonz;




void IPosition(void);// 光源位置

void init(void);// 初始化，启动光源、材质、消隐

void material_sun(void);//太阳材质

void material_earth(void);// 地球材质

void material_moon(void);// 月球材质

void sun(void);//绘制太阳

void earth(void);//绘制地球

void moon(void);//绘制月球

void display(void);

void Rotate(void);   //更新旋转参数

void myidle(void);   //闲置调用函数

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

	glutCreateWindow("太阳地球月亮");

	init();

	glutDisplayFunc(display);   //调用显示回调函数

	glutReshapeFunc(reshape);   // 设置观察体范围

	glutKeyboardFunc(mykeyboard); //键盘回调函数

	glutMouseFunc(MousePlot);

	glutIdleFunc(myidle);       // 闲置 回调函数

	glutMainLoop();

	return 0;

}

//设置光源位置

void IPosition()

{

	float y, z;

	y = light_radius * cos(light_angle);

	z = light_radius * sin(light_angle);

	float light_position[] = { 3.0,y,z,0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

}



//启动光源、消隐、材料

void init(void)

{

	//IPosition(); //光源位置

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	//逆时针为多边形正面
	glFrontFace(GL_CCW);
	//启用剔除效果，多边形背面不用光照处理
	glEnable(GL_CULL_FACE);
	//启动灯光效果
	glEnable(GL_LIGHTING);
	//设置灯泡0
	//环境光颜色
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	//散射光强度
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	//光源位置
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	//开灯
	glEnable(GL_LIGHT0);

	//glEnable(GL_COLOR_MATERIAL);//使用颜色材质

	glClearDepth(1.0f);

	AUX_RGBImageRec *TextureImage[1];//加载位图

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //多边形的显示方式,模式将适用于物体的所有面采用填充形式

	glGenTextures(1, &texName);//glGenTextures (GLsizei n, GLuint *textures);在数组textures中返回n个当期未使用的值,表示纹理对象的名称 

	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//纹理过滤函数，必须写两遍

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	memset(TextureImage, 0, sizeof(void *) * 1);//作用是将某一块内存中的内容全部设置为指定的值

	if (TextureImage[0] = auxDIBImageLoadA("D:\\game\\CG\\earth2.bmp"))   //实现加载位图文件

	{

		//根据指定的参数，生成一个2D纹理（Texture）。

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
	if (mode == GL_SELECT)glPushName(1);//将名字1压入堆栈
	sun();
	if (mode == GL_SELECT)glPushName(2);//将名字2压入堆栈
	earth();
	if (mode == GL_SELECT)glPushName(3);//将名字3压入堆栈
	moon();
	glutSwapBuffers();

	
}

void display(void)

{

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawRect(GL_RENDER);//用渲染模式绘制图形
	glFlush();
}

//太阳材质

void material_sun(void)

{

	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //材质镜面反射颜色参数

	GLfloat mat_shininess[] = { 50.0 };                   // 镜面反射指数参数

	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //材质散射颜色	 

	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };

	GLfloat lmodel_ambient[] = { 1.0,0.0,0.0,1.0 };       //太阳颜色为红色     

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //材质镜面反射颜色

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //镜面反射指数

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //材质的散射颜色

	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //材质的环境颜色 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  整个场景的环境光的RGBA强度

}

//地球材质

void material_earth(void)

{

	glEnable(GL_COLOR_MATERIAL);

	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //材质镜面反射颜色参数

	GLfloat mat_shininess[] = { 50.0 };                   // 镜面反射指数参数

	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //材质散射颜色	 

	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };

	GLfloat lmodel_ambient[] = { 0.0,0.0,1.0,1.0 };       //地球颜色为蓝色     

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //材质镜面反射颜色

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //镜面反射指数

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //材质的散射颜色

	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //材质的环境颜色 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  整个场景的环境光的RGBA强度

}

//月球材质

void material_moon(void)

{

	glEnable(GL_COLOR_MATERIAL);

	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //材质镜面反射颜色参数

	GLfloat mat_shininess[] = { 50.0 };                   // 镜面反射指数参数

	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //材质散射颜色	 

	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };



	GLfloat lmodel_ambient[] = { 0.5,0.5,0.5,1.0 };       //月亮颜色为灰色     





	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //材质镜面反射颜色

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //镜面反射指数

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //材质的散射颜色

	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //材质的环境颜色 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  整个场景的环境光的RGBA强度

}

//绘制太阳

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

	glRotatef((GLfloat)year, 0.0, 1.0, 0.0);   //太阳自转	

	glutSolidSphere(0.3, 60, 60);                //画球体   

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//得到视图，投影，视口的矩阵
	glGetDoublev(GL_PROJECTION_MATRIX, mProj);
	glGetDoublev(GL_MODELVIEW_MATRIX, mView);
	glGetIntegerv(GL_VIEWPORT, mPort);
	//模拟投影，视图，模型变换
	gluProject(0, 0, 0, mView, mProj, mPort, &wx, &wy, &wz);
	sunx = wx;
	suny = wy;
	sunz = wz;


	glPopMatrix();

}

//绘制地球和月球

void earth()

{
	GLdouble mProj[16];
	GLdouble mView[16];
	GLint mPort[4];
	GLdouble wx, wy, wz;

	qobj = gluNewQuadric();//申请空间
	glPushMatrix();
	material_earth();
	glBindTexture(GL_TEXTURE_2D, texName);//允许建立一个绑定到目标纹理的有名称的纹理。

	glEnable(GL_TEXTURE_2D);//启用二维纹理

	gluQuadricTexture(qobj, GL_TRUE);//纹理函数



	glRotatef((GLfloat)day, aix_x, aix_y, aix_z);//公转

	glTranslatef(1.0, 0.0, 0.0);//平移

	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//自转

	//地球绕太阳的公转 设置//绘制地球

	//glutSolidSphere(0.1, 20, 16);
	gluSphere(qobj, 0.2, 20, 16); //二次曲面qobj

	glDisable(GL_TEXTURE_2D);//禁用二维纹理

	//得到视图，投影，视口的矩阵
	glGetDoublev(GL_PROJECTION_MATRIX, mProj);
	glGetDoublev(GL_MODELVIEW_MATRIX, mView);
	glGetIntegerv(GL_VIEWPORT, mPort);
	//模拟投影，视图，模型变换
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

	glRotatef((GLfloat)day, aix_x, aix_y, aix_z);//公转

	glTranslatef(1.0, 0.0, 0.0);//平移

	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//自转

	glRotatef((GLfloat)day, 0, 1, 0);//公转

	glTranslatef(0.3, 0, 0);

	glutSolidSphere(0.075, 20, 16);

	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//自转

	//得到视图，投影，视口的矩阵
	glGetDoublev(GL_PROJECTION_MATRIX, mProj);
	glGetDoublev(GL_MODELVIEW_MATRIX, mView);
	glGetIntegerv(GL_VIEWPORT, mPort);
	//模拟投影，视图，模型变换
	gluProject(0, 0, 0, mView, mProj, mPort, &wx, &wy, &wz);
	moonx = wx;
	moony = wy;
	moonz = wz;

	glPopMatrix();

}




void Rotate(void)          //设置各行星的公转周期

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
	printf("选中的数目为%d个\n", nPicks);
	ptr = pickBuffer;
	for (i = 0; i < nPicks; i++)
	{
		name = *ptr;//选中图元在堆栈中的位置
		ptr += 3;//跳过名字和深度信息
		ptr += name - 1;//根据位置信息获得选中的图元名字
		if (*ptr == 1)printf("你选择了太阳\n");
		if (*ptr == 2)printf("你选择了地球\n");
		if (*ptr == 3)printf("你选择了月亮\n");
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

		glGetIntegerv(GL_VIEWPORT, viewport); // 得到的是最后一个设置视口的参数
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glutPostRedisplay();


		winX = xMouse;

		winY = winHeight - yMouse;



		//获取像素对应的前裁剪面的点坐标

		bool bResult = gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &posX, &posY, &posZ);

		

		nearPointx = posX; nearPointy = posY; nearPointz = posZ;

		//获取像素对应的后裁剪面的点坐标

		bResult = gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &posX, &posY, &posZ);


		farPointx = posX; farPointy = posY; farPointz = posZ;

		if(pow(nearPointx,2)+ pow(nearPointy, 2)< pow(0.3, 2))
			printf("你选择了太阳\n");
		if (pow(nearPointx-cos(day*M_PI/180), 2) + pow(nearPointy, 2) < pow(0.2, 2))
			printf("你选择了地球\n");
		if (pow(nearPointx - cos(day*M_PI / 180) - 0.3*cos(month*M_PI / 180), 2) + pow(nearPointy, 2) < pow(0.075, 2))
			printf("你选择了月亮\n");

	}
}