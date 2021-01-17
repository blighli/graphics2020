/// ADD begin
#include <iostream>
using namespace std;
/// ADD end


#include <GL/glut.h>
#include <math.h>


/// ADD begin
GLUquadricObj* quadric;
unsigned int texture_id[3];
enum OBJECT_ID {
	SUN,
	EARTH,
	MOON
};
unsigned int LoadTex(string Image, unsigned int& Texture)
{
	FILE* img = NULL;
	img = fopen(Image.c_str(), "rb");

	unsigned long bWidth = 0;
	unsigned long bHeight = 0;
	unsigned long size = 0;

	fseek(img, 18, SEEK_SET);
	fread(&bWidth, 4, 1, img);
	fread(&bHeight, 4, 1, img);
	fseek(img, 0, SEEK_END);
	size = ftell(img) - 54;

	unsigned char* data = (unsigned char*)malloc(size);

	fseek(img, 54, SEEK_SET);    // image data
	fread(data, size, 1, img);

	fclose(img);

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bWidth, bHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	if (data)
		free(data);

	return Texture;
}
/// ADD end

float seta = 200;
float mx = 0, my = 0, mz = 50, rx = 0, ry = 0, rz = 0;//平移和旋转
float mspeed = 5, rspeed = 1;
static int oldrx = 0, oldry = 0;//旋转
static int oldmx = 0, oldmy = 0;//平移
static int oldpx = 0, oldpy = 0;
bool a = false, b = false;
int specialKey;
void myDisplay(void);

void Mouse(int button, int state, int x, int y) //处理鼠标点击
{

	if (state == GLUT_DOWN) //第一次鼠标按下时,记录鼠标在窗口中的初始坐标
	{
		if (button == GLUT_RIGHT_BUTTON) {
			oldrx = x, oldry = y;
			b = true;
		}
		else if (button == GLUT_LEFT_BUTTON) {
			oldmx = x, oldmy = y;
			a = true;
		}
	}
	else if (state == GLUT_UP)//鼠标松开
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			a = false;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			b = false;
		}
	}
}
void onMouseMove(int x, int y) //处理鼠标拖动
{
	if ((a == true) && (b == true))//左右键
	{
		mz += (y - oldmy) * mspeed / 10;
		oldmx = x, oldmy = y;
	}
	else if (a == true)//左键
	{
		my += -(y - oldmy) * mspeed;
		mx += (x - oldmx) * mspeed;
		oldmx = x, oldmy = y;
	}
	else if (b == true)//右键
	{
		ry += -(x - oldrx);
		rx += -(y - oldry);
		oldrx = x, oldry = y;
	}

}

void myTimerFunc(int val)
{
	++seta;
	if (seta >= 360)
		seta = 0;//每年为360天，超过360从0再开始
	myDisplay();
	glutTimerFunc(1, myTimerFunc, 0);
}
void myKeyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'W':
		my += mspeed;
		break;
	case 'S':
		my -= mspeed;
		break;
	case 'A':
		mx -= mspeed;
		break;
	case 'D':
		mx += mspeed;
		break;
	case 'Q':
		mz += mspeed;
		break;
	case 'E':
		mz -= mspeed;
		break;
	case 'I':
		rx += rspeed;
		break;
	case 'K':
		rx -= rspeed;
		break;
	case 'J':
		ry += rspeed;
		break;
	case 'L':
		ry -= rspeed;
		break;
	case '1':
		mspeed += 2;
		break;
	case '2':
		mspeed -= 2;
		break;
	case '3':
		rspeed += 0.5;
		break;
	case '4':
		rspeed -= 0.5;
		break;
	}
}
void SetRC()
{
	glShadeModel(GL_FLAT);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float amb[4] = { 0.8,0.0,0.0,1 };					//红色环境光
	float dif[4] = { 1.0,1.0,1.0,1 };					//白色漫射光和反射光
	float spe[4] = { 1.0,1.0,1.0,1 };                   //黑色镜面反射
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, amb);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_BACK, GL_LINE);

	/// ADD begin
	// 载入纹理
	LoadTex("sun.bmp", texture_id[SUN]);
	LoadTex("earth.bmp", texture_id[EARTH]);
	LoadTex("moon.bmp", texture_id[MOON]);
	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// 二次几何体
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GL_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE);
	/// ADD end
}
void SetView()
{
	glRotatef(-ry, 0, 1, 0);		//h
	glRotatef(-rx, 1, 0, 0);		//p
	glRotatef(-rz, 0, 0, 1);		//r
	glTranslatef(-mx, -my, -mz);
}
void RenderWorld()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0, 5, -25);
	glRotatef(seta, 0, 1, 0);
	glPopMatrix();

	//太阳
	glPushMatrix();
	glTranslatef(0, -100, -600);
	glPushMatrix();
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//glColor3f(1,0,0);
	glRotatef(seta * 2, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	/// ADD begin
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);	// Set Up Sphere Mapping
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);	// Set Up Sphere Mapping	
	glBindTexture(GL_TEXTURE_2D, texture_id[SUN]);
	gluSphere(quadric, 70, 100, 100);
	/// ADD end

	//glutSolidSphere(70,100,100);
	float pos[4] = { 0,0,0,1 };    //以太阳为光源
	glLightfv(GL_LIGHT0, GL_POSITION, pos);	//设置光源位置
	glPopMatrix();
	//地球
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//glColor3f(0,0,1);
	glRotatef(seta, 0, 1, 0);//公转
	glTranslatef(175, 0, 0);
	glRotatef(seta, 0, 1, 0);//自转
	glRotatef(90, 1, 0, 0);
	/// ADD begin
	glBindTexture(GL_TEXTURE_2D, texture_id[EARTH]);
	gluSphere(quadric, 16, 200, 200);
	/// ADD end
	//glutSolidSphere(16,200,200);//地球
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//glColor3f(1,1,0);
	glRotatef(seta * 6, 0, 0, -1);
	glTranslatef(35, 0, 0);
	glRotatef(seta * 1, 0, 1, 0);//月球自转
	/// ADD begin
	glBindTexture(GL_TEXTURE_2D, texture_id[MOON]);
	gluSphere(quadric, 4.5, 60, 6);
	/// ADD end
	//glutSolidSphere(4.5,60,60);
	glPopMatrix();//月球

}
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	SetView();
	RenderWorld();
	//模型控制
	glPopMatrix();
	glutSwapBuffers();
}

void myReshape(int w, int h)
{
	GLfloat nRange = 100.0f;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, GLfloat(w) / h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("第一个OpenGL程序");
	glutDisplayFunc(&myDisplay);
	glutMouseFunc(Mouse);
	glutMotionFunc(onMouseMove);
	glutTimerFunc(1, myTimerFunc, 0);
	glutReshapeFunc(&myReshape);
	glutKeyboardFunc(&myKeyboardFunc);
	SetRC();
	glutMainLoop();
	return 0;
}