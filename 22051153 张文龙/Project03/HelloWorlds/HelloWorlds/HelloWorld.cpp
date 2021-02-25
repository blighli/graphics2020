#define _CRT_SECURE_NO_WARNINGS
#include "glut.h"

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <string>
#include <sstream>     

using namespace std;

int ret;
int rol_flag = 1; //obj是需要否旋转
int rol_flag2 = 0;
float fTranslate;
float fRotate;
float fScale = 1.0f;	

// 显示模式控制
bool bPersp = 1;
bool bAnim = false;
bool bWire = false;


// 有关场景漫游的一些全局变量
float rr = 0.00001;  //相机运动所在球体的 '经度'
float rt = -1.40000;  //相机运动所在球体的 '纬度'

float camera2obj_len = 10; //  初始化相机离物体的距离为10个单位长度
float eye[] = { 0.0, 1.0, 10.0 };  // 初始化相机位置
float center[] = { 0.0, 0.0, 0.0 };   // 固定相机朝向坐标系的中心，也就是物体所摆放的位置

int t_x = 0, t_y = 0, t_z = 0; // obj放置的默认位置
int rol = 0; //obj的旋转角度

class ObjLoader {
public:
	ObjLoader(string filename);
	void Draw();
private:
	vector<vector<GLfloat>> vSets;
	vector<vector<GLint>>   fSets;
};

// 初始化载入obj模型
ObjLoader grass = ObjLoader("grass.obj");
ObjLoader Mugs = ObjLoader("Mug.obj");
ObjLoader tree_1 = ObjLoader("tree-1.obj");
ObjLoader tree_2 = ObjLoader("tree-2.obj");


int wHeight = 0;
int wWidth = 0;
void light();
void updateView(int width, int height);


// 各个obj的着色器
void style_Grass();
void style_Ground();
void style_stool();
void style_sun();
void style_tree_1();
void style_tree_2();
void style_tree_2_1();

void draw_sun();

// 整体场景的布置；
void Draw_Scene();

// 鼠标键控
void MousFunc(int button, int state, int x, int y);
void key(unsigned char k, int x, int y);


void redraw();
void reshape(int width, int height);
void PassiveMotion(int x, int y);
void idle();



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1000, 1000);
	int windowHandle = glutCreateWindow("Simple GLUT App");

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutMouseFunc(MousFunc);  //注册鼠标事件
	glutMotionFunc(PassiveMotion);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}



GLfloat RGB2Float(int a) {
	float b = a;
	return b / 255;
}


void style_Grass()
{
	GLfloat mat_specular[] = { RGB2Float(0), RGB2Float(100), RGB2Float(0), 1.0 };
	GLfloat mat_shininess[] = { 1000.0 };
	GLfloat lmodel_ambient[] = { RGB2Float(0), RGB2Float(100), RGB2Float(0), 1.0 };
	GLfloat light[] = { 0.8, 0.8,0.8, 1.0 }; 


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);// 将会使物体在有绿色成分的光照下表现为绿色。
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}


void style_Ground()
{
	GLfloat mat_specular[] = { RGB2Float(250), RGB2Float(215), RGB2Float(185), 1.0 };
	GLfloat mat_shininess[] = { 1000.0 };
	GLfloat lmodel_ambient[] = { RGB2Float(250), RGB2Float(215), RGB2Float(185), 1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}

void style_stool()
{
	GLfloat mat_specular[] = { RGB2Float(160), RGB2Float(82), RGB2Float(45), 1.0 };
	GLfloat mat_shininess[] = { 1000.0 };
	GLfloat lmodel_ambient[] = { RGB2Float(160), RGB2Float(82), RGB2Float(45), 1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
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


void style_tree_1()
{
	GLfloat mat_specular[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 1000.0 };
	GLfloat lmodel_ambient[] = { 0.0,1.0,0.0,1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}


void style_tree_2()
{
	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 1000.0 };
	GLfloat lmodel_ambient[] = { 1.0,0.0,0.0,1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}


void style_tree_2_1()
{
	GLfloat mat_specular[] = { RGB2Float(255), RGB2Float(215), RGB2Float(0), 1.0 };
	GLfloat mat_shininess[] = { 1000.0 };
	GLfloat lmodel_ambient[] = { RGB2Float(255), RGB2Float(215), RGB2Float(0), 1.0 };
	GLfloat white_light[] = { 1.0, 1.0,1.0, 1.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}





void Draw_Leg()//绘制桌腿
{
	glScalef(1, 1, 3);
	glutSolidCube(1.0);
}



// 绘制天体
void draw_sun() {
	//绘制太阳
	style_sun();
	glutSolidSphere(2, 30, 30);

}



void Draw_Scene()
{
	glPushMatrix();
	glTranslatef(50, 50, 200);
	draw_sun();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(t_x-4, 0 + t_y, 0.6 + 0.6 + t_z);
	glRotatef(90, 1, 0, 0);
	glRotatef(rol, 0, 1, 0);
	glutSolidTeapot(1);
	glPopMatrix();


	style_stool();
	glPushMatrix();
	glTranslatef(-4 , 0 , 0  );
	glScalef(5, 4, 1);
	glutSolidCube(1.0);
	glPopMatrix();


	style_stool();
	glPushMatrix();
	glTranslatef(0, -8, 2.0);
	// glRotatef(90, 1, 0, 0);
	// glRotatef(90, 20, 0, 0);
	glScalef(3, 3, 3);
	Mugs.Draw();
	glPopMatrix();


	int k = 0;
	int i = 0;

	for (k = 0; k < 3; k++) {
		glPushMatrix();
			glTranslatef(-10, -20 + 6*k, 0.0);
			glRotatef(90, 1, 0, 0);
			glScalef(4, 4, 4);
			style_tree_1();
			tree_1.Draw();
		glPopMatrix();
	}


	glPushMatrix();
		glTranslatef(10, -8, 0.0);
		glRotatef(90, 1, 0, 0);
		glScalef(4, 4, 4);
		style_tree_1();
		tree_1.Draw();
	glPopMatrix();



	for (k = 0; k < 30; k = k + 2) {
		glPushMatrix();
			glTranslatef(-50, -50 + 12*k, -5);
			glRotatef(90, 1, 0, 0);
			glScalef(2*(k%2+1)*0.5, 3 * (k % 2 + 1) * 0.5, 2 * (k % 2 + 1) * 0.5);
			style_tree_2_1();
			tree_2.Draw();
		glPopMatrix();
	}

	for (k = 1; k < 30; k = k + 2) {
		glPushMatrix();
			glTranslatef(-50, -50 + 12 * k, -10);
			glRotatef(90, 1, 0, 0);
			glScalef(2 * (k % 2 + 1) * 0.5, 3 * (k % 2 + 1) * 0.5, 2 * (k % 2 + 1) * 0.5);
			style_tree_2();
			tree_2.Draw();
		glPopMatrix();
	}



	for (k = 1; k < 20; k = k + 2) {
		style_Grass();
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef(12 - (k/4)*4, -6, 20.5-k*4 );
		glScalef(1.5, 1.5, 1.5);
		grass.Draw();
		glPopMatrix();
	}

	for (k = 1; k < 10; k = k + 2) {
		style_Grass();
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef(12 - (k / 4) * 4, -6, 20.5 + k * 4);
		glScalef(1.5, 1.5, 1.5);
		grass.Draw();
		glPopMatrix();
	}



	for (k = 0; k < 3; k++) {
		glPushMatrix();
		glTranslatef(t_x + 3, 0 + t_y+i, 4 + 0.8+ i + t_z);
		glRotatef(90, 1, 0, 0);
		glRotatef(rol, 0, 1, 0);
		glutSolidTeapot(1);
		glPopMatrix();


		style_stool();
		glPushMatrix();
		glTranslatef(3, 0 + i, 3.5+i);
		glScalef(5, 4, 1);
		glutSolidCube(1.0);
		glPopMatrix();


		i += 6;
	}



	style_Ground();
	glPushMatrix();
	glTranslatef(-4, 0, 0);

	glBegin(GL_POLYGON);
	glColor3f(0.0, 1.0, 0.1);
	glVertex3f(-500, -500,0.0); 
	glVertex3f(-500, 500,0.0);  
	glVertex3f(500, 500,0.0);              	
	glVertex3f(500, -500,0.0); 
	glEnd();



	glPopMatrix();


	if (rol_flag==1) { rol++; printf("roling ...\n"); }


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
////////////////////////////////////////
void updateView(int width, int height)
{
	glViewport(0, 0, width, height);						

	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();									

	float whRatio = (GLfloat)width / (GLfloat)height;

	if (bPersp) {
		gluPerspective(30, 1, 1, -5);
		printf("gluPerspective\n");
	}
	else
		glOrtho(-3, 3, -3, 3, -100, 100);
	    printf("glOrtho\n");


	glMatrixMode(GL_MODELVIEW);							
}


void idle()
{
	glutPostRedisplay();
}

//////////////////////////////////



//float e_x = 0, e_y = 0, e_z = 0;
//float c_x = 0, c_y = 0, c_z = 0;



void key(unsigned char k, int x, int y)
{
	if (rol_flag2){ // rol_flag2  =1 以模型为中心的平移旋转和缩放 // rol_flag2 =0 以视点为中心的场景漫游
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
			eye[0] = camera2obj_len * sin(rr) * cos(rt);
			eye[2] = camera2obj_len * cos(rr) * cos(rt);
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
	
	else { // 以视点为中心的场景漫游
		switch (k)
		{
			case 27:
			case 'q': {exit(0); break; }
			case 'p': {bPersp = !bPersp; updateView(wHeight, wWidth); break; }

			case ' ': {bAnim = !bAnim; break; }
			case 'o': {bWire = !bWire; break; }
					//WA SD ZC：控制相机上下左右前后移动 //场景漫游 
			case 'd': {
				eye[0] += 0.1 * sin(rr);
				center[0] += 0.1 * sin(rr);
				eye[0] += 0.1 * cos(rr);
				center[0] += 0.1 * cos(rr);
				break;
			}
			case 'a': {
				eye[0] -= 0.1 * sin(rr);
				center[0] -= 0.1 * sin(rr);
				eye[0] -= 0.1 * cos(rr);
				center[0] -= 0.1 * cos(rr);
				break;
			}
			case 'c': {//场景漫游 
				eye[1] -= 0.03;
				center[1] -= 0.03;
				break;
			}
			case 'z': {
				eye[1] += 0.03;
				center[1] += 0.03;
				break;
			}


			case 'w': { // 场景漫游
				eye[2] -= 0.1;
				center[2] -= 0.1;
				break;
			}
			case 's': { //
				eye[2] += 0.1;
				center[2] += 0.1;
				break;
			}


			case 'n': {
				eye[2] -= 0.03 * cos(rr) * sin(rt);
				center[2] -= 0.03 * cos(rr) * sin(rt);
				eye[0] += 0.03 * sin(rr) * sin(rt);
				center[0] += 0.03 * sin(rr) * sin(rt);
				eye[1] += 0.03 * cos(rt);
				center[1] += 0.03 * cos(rt);
				break;
			}
			case 'm': {
				eye[2] += 0.03 * cos(rr) * sin(rt);
				center[2] += 0.03 * cos(rr) * sin(rt);
				eye[0] -= 0.03 * sin(rr) * sin(rt);
				center[0] -= 0.03 * sin(rr) * sin(rt);
				eye[1] -= 0.03 * cos(rt);
				center[1] -= 0.03 * cos(rt);
				break;
			}



					///////////////////
			case 'l': {//
				t_x++;
				break;
			}
			case 'j': {
				t_x--;
				break;
			}
			case 'i': {//前todo 
				t_y++;
				break;
			}
			case 'k': {//后todo
				t_y--;
				break;
			}
			case 'e': {
				rol_flag = -rol_flag;
				break;
			}
		}
	}



}

bool mouseLeft;
float M_X, M_Y;
void MousFunc(int button, int state, int x, int y)
{
	M_X = x;
	M_Y = y;
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeft= true;
		}
		else if (state == GLUT_UP)
			mouseLeft = false;
	}

	
}

void PassiveMotion(int x, int y)
{
	if (mouseLeft){
		center[0] += (x - M_X)/10;
		center[1] -= (y - M_Y)/10;
		//eye[2] -= (x- M_X) / 10 + (y - M_Y) / 10;
		M_X = x;
		M_Y = y;
	}
	glutPostRedisplay();
	rr = -atan((center[0]-eye[0])/(center[2]-eye[2]));


	rt = atan(  sqrt((center[0] - eye[0])*(center[0] - eye[0])+ (center[2] - eye[2])*(center[2] - eye[2]))/ (center[1] - eye[1]));

	printf("%f,%f, ----%f %f \n", center[0],rt/3.14*180,rr,rt);
}


ObjLoader::ObjLoader(string filename)
{
	std::ifstream file(filename);
	std::string line;
	while (getline(file, line))
	{
		if (line.substr(0, 2) == "vt")
		{

		}
		else if (line.substr(0, 2) == "vn")
		{

		}
		else if (line.substr(0, 1) == "v")
		{
			vector<GLfloat> Point;
			GLfloat x, y, z;
			std::istringstream s(line.substr(2));
			s >> x; s >> y; s >> z;
			Point.push_back(x);
			Point.push_back(y);
			Point.push_back(z);
			vSets.push_back(Point);

		}
		else if (line.substr(0, 1) == "f")
		{
			vector<GLint> vIndexSets;
			GLint u[3], v[3], w[3], m[3];
			if (filename == "Mug.obj") { // 适配不同的obj格式
				ret = sscanf(line.data(), "f %d//%d %d//%d %d//%d\n", &u[0], &u[1], &v[0], &v[1], &w[0], &w[1]);
			}
			else {
				ret = sscanf(line.data(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &u[0], &u[1], &u[2], &v[0], &v[1], &v[2], &w[0], &w[1], &w[2]);
			}
			//std::istringstream vtns(line.substr(2));
			vIndexSets.push_back(u[0] - 1);
			vIndexSets.push_back(v[0] - 1);
			vIndexSets.push_back(w[0] - 1);
			fSets.push_back(vIndexSets);
		}
		else if (line.substr(0, 1) == "#")
		{

		}
		else
		{

		}
	}
	file.close();
	printf("obj loaded...");
}




void ObjLoader::Draw() {
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLES);//开始绘制
	for (int i = 0; i < fSets.size(); i++) {
		GLfloat VN[3];
		//三个顶点
		GLfloat SV1[3];
		GLfloat SV2[3];
		GLfloat SV3[3];

		if ((fSets[i]).size() != 3) {
			cout << "the fSetsets_Size is not correct" << endl;
		}
		else {
			GLint firstVertexIndex = (fSets[i])[0];//取出顶点索引
			GLint secondVertexIndex = (fSets[i])[1];
			GLint thirdVertexIndex = (fSets[i])[2];

			SV1[0] = (vSets[firstVertexIndex])[0];//第一个顶点
			SV1[1] = (vSets[firstVertexIndex])[1];
			SV1[2] = (vSets[firstVertexIndex])[2];

			SV2[0] = (vSets[secondVertexIndex])[0]; //第二个顶点
			SV2[1] = (vSets[secondVertexIndex])[1];
			SV2[2] = (vSets[secondVertexIndex])[2];

			SV3[0] = (vSets[thirdVertexIndex])[0]; //第三个顶点
			SV3[1] = (vSets[thirdVertexIndex])[1];
			SV3[2] = (vSets[thirdVertexIndex])[2];


			GLfloat vec1[3], vec2[3], vec3[3];//计算法向量
			//(x2-x1,y2-y1,z2-z1)
			vec1[0] = SV1[0] - SV2[0];
			vec1[1] = SV1[1] - SV2[1];
			vec1[2] = SV1[2] - SV2[2];

			//(x3-x2,y3-y2,z3-z2)
			vec2[0] = SV1[0] - SV3[0];
			vec2[1] = SV1[1] - SV3[1];
			vec2[2] = SV1[2] - SV3[2];

			//(x3-x1,y3-y1,z3-z1)
			vec3[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
			vec3[1] = vec2[0] * vec1[2] - vec2[2] * vec1[0];
			vec3[2] = vec2[1] * vec1[0] - vec2[0] * vec1[1];

			GLfloat D = sqrt(pow(vec3[0], 2) + pow(vec3[1], 2) + pow(vec3[2], 2));

			VN[0] = vec3[0] / D;
			VN[1] = vec3[1] / D;
			VN[2] = vec3[2] / D;

			glNormal3f(VN[0], VN[1], VN[2]);//绘制法向量

			glVertex3f(SV1[0], SV1[1], SV1[2]);//绘制三角面片
			glVertex3f(SV2[0], SV2[1], SV2[2]);
			glVertex3f(SV3[0], SV3[1], SV3[2]);
		}
	}
	glEnd();
}



void light() {

	GLfloat white[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat red[] =   { 0.2, 1.0, 0.0, 0.0 };
	GLfloat light_pos0[] = { 5,500,20,0 };


	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);


}






void redraw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);

	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnable(GL_DEPTH_TEST);
	light();
	//	glTranslatef(0.0f, 0.0f,-6.0f);			
	glRotatef(fRotate, 0, 1.0f, 0);
	glRotatef(-90, 1, 0, 0);
	glScalef(0.2, 0.2, 0.2);
	Draw_Scene();

	if (bAnim) fRotate += 0.5f;

	glutSwapBuffers();
}
