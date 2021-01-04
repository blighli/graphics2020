#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glut.h"
#include<iostream>
using namespace std;

static float earth_day = 100;       //假定从地球日第100天起，公转365地球日
static float Jupiter_day = 3000;	//假定从木星日第3000天起
int speed	= 20;					//用来控制太阳系的整体运行速度，这里20表示，渲染20帧，地球过了1天
int speed_o = 0;					//这个变量用来显示控制台地球日期的输出频率


static GLuint earth;
static GLuint sun;

void display(void);
void day_count();
void reshape(int w, int h);
unsigned int Texture_init(string file);


void orbit(float day, float altitude, float x, float y, float z); //绘制轨道
void aixs(int len); //绘制天体自转轴线


// 各种天体模型的样式
void style_sun();
void style_earth();
void style_moon();
void style_mars();
void style_Jupiter();
GLuint sunTexture;
GLuint earthTexture;
GLuint moonTexture;
GLuint jupiterTexture;
GLUquadricObj* quadObj;
void light();			 //太阳光照模型
void draw_sun();         //绘制太阳
void draw_earth();       //绘制地球
void draw_moon();        //绘制月球
void draw_mars();		 //绘制火星
void draw_Jupiter();     //绘制木星



int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1366, 768);//初始化窗口大小
	glutInitWindowPosition(500, 200);
	glutCreateWindow("Solar system");    //窗口名称
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	sunTexture		= Texture_init("sun.BMP");
	earthTexture	= Texture_init("earth.bmp");
	moonTexture		= Texture_init("moon.bmp");
	jupiterTexture  = Texture_init("jupiter.bmp");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

void day_count() { 
	//地球公转
	earth_day += (GLfloat)(1.0 / speed);//表示播放20帧过了一天speed=20
	if (earth_day >= 365.25)
		earth_day = earth_day - 365.25;//公转周期是365.25天
	//木星公转
	Jupiter_day += (GLfloat)(1.0 / (speed / 4));//表示播放5帧过了一天,木星公转太慢了，要稍微加速，这样可以看清楚点
	if (Jupiter_day >= 4332.25)
		Jupiter_day = Jupiter_day - 4332.25;//公转周期是4332.25天
	glutPostRedisplay();
}



void reshape(int w, int h) {  
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20, (GLfloat)w / (GLfloat)h, 1.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -120.0);
}




void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	light();  //光照函数，光源放在太阳这里

	day_count();   //控制天体运行速度
	speed_o++;
	if(speed_o == 1){ //显示地球日
		printf("[Day:%3d] ", (int)earth_day);
	}else if(speed_o == 20){
		speed_o = 0;
	}

	glPushMatrix();  //整体的初始的坐标系,原点是太阳
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw_sun();
	glPopMatrix();

	glutSwapBuffers();
}


unsigned int Texture_init(string file)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//__glewGenerateMipmap(GL_TEXTURE_2D);
	printf("loaded texture %s\n", file.c_str());
	stbi_image_free(data);
	return texture;
}


//绘制轨道
void orbit(float day, float altitude, float x, float y, float z) {
	for (int i = 0; i < 360; i++) {
		glPushMatrix();
		glRotatef((360 - i), x, y, z);			//地球公转，旋转太阳系的坐标系，公转周期是一年
		glTranslatef(altitude, 0, 0);           //地球绕月球公转半径是15
		glBegin(GL_POINTS);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}

}

// 绘制自转轴线
void aixs(int len) {
	for (int i = 0; i < 360; i++) {
		glPushMatrix();
			glBegin(GL_LINES);
			glVertex3f(0.0, (i - 180) / len+2, 0.0);
			glVertex3f(0.0, (i - 180) / len-2, 0.0);
			glEnd();
		glPopMatrix();
	}
}


// 绘制天体
void draw_sun(){
	//绘制太阳
	style_sun();
	quadObj = gluNewQuadric();
	gluQuadricTexture(quadObj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	gluSphere(quadObj, 5, 60, 60);
	gluDeleteQuadric(quadObj);
	aixs(15);

	glPushMatrix();                 
		glRotatef((GLfloat)(earth_day /365.25)*360, 0.0,2.5,1.0);  //地球公转，旋转太阳系的坐标系，公转周期是一年
		draw_earth();
	glPopMatrix();
	orbit(earth_day,15, 0.0, 2.5, 1.0);


	glPushMatrix();
		glRotatef((GLfloat)(Jupiter_day / 1000) * 360, 0.0, 2.5, 1.0); 
		draw_mars();
	glPopMatrix();


	glPushMatrix();
		glRotatef((GLfloat)(Jupiter_day / 4332.25) * 360, 0.0, 2.5, 1.0);  
		draw_Jupiter();
	glPopMatrix();

	orbit(earth_day, 15*2, 0.0, 2.5, 1.0);



}

void draw_earth(){

	glTranslatef (15,0,0);           //地球绕月球公转半径是15
	style_earth();
	quadObj = gluNewQuadric();
	gluQuadricTexture(quadObj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, earthTexture);
	gluSphere(quadObj, 2, 60, 60);
	gluDeleteQuadric(quadObj);
	aixs(70);


	glPushMatrix();                  //这是以地球为中心的坐标系    
		glRotatef((GLfloat)(earth_day /28.05)*360, 0.0, 1.0, 0.0);         //月球绕地球的公转周期大约是28.05天
		draw_moon();
	glPopMatrix();
	orbit(earth_day, 4, 0.0, 1.0, 0.0);
}


void draw_moon() {
	glTranslatef(4, 0, 0);        //月球绕地球公转半径是4
	style_moon();
	quadObj = gluNewQuadric();
	gluQuadricTexture(quadObj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, moonTexture);
	gluSphere(quadObj, 1, 60, 60);
	gluDeleteQuadric(quadObj);
}

void draw_mars() {
	glTranslatef(19, 0, 0);
	style_mars();
	glutSolidSphere(2, 60, 60);
}


void draw_Jupiter() {
	glTranslatef(15 * 2, 0, 0);     
	style_Jupiter();
	quadObj = gluNewQuadric();
	gluQuadricTexture(quadObj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, jupiterTexture);
	gluSphere(quadObj, 3, 60, 60);
	gluDeleteQuadric(quadObj);

}



void light() {
	float light_position[] = { 0,0,0,1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}









void style_sun()                          
{
	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 1000.0 };
	GLfloat lmodel_ambient[] = { 1.0,1.0,0.0,1.0 };      
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}

void style_earth()
{
	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat lmodel_ambient[] = { 0.1,0.2,0.6,1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

}

void style_moon()
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat lmodel_ambient[] = { 0.5,0.5,0.5,1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}



void style_mars() {
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat lmodel_ambient[] = { 1,0.0,0.0,1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}


void style_Jupiter() {
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat lmodel_ambient[] = { 0.8,0.4,0.1,1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}
