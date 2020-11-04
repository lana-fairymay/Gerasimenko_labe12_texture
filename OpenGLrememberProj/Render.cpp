#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
#include "Normal.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void normal(float *mas) {
	float x, y, z;
	x = mas[1] * 5 - 0 * mas[2];
	y = -mas[0] * 5 + 0 * mas[2];
	z = mas[0] * 0 - 0 * mas[1];
	mas[0] = x;
	mas[1] = y;
	mas[2] = z;	
}


void treug() {
	float r = 3.041; //������
	float XYZ[3];


	glColor3d(0.7, 0.76, 0.92);


	for (int i = 0; i <= 9; i++) {
		float a = (float)i * 0.55 / (float)10 * 3.1415f * 0.1;
		float b = (float)(i + 1) * 0.55 / (float)10 * 3.1415f * 0.1;
		glBegin(GL_QUADS);
		XYZ[0] = (4 - cos(b) * r)-(4 - cos(a) * r ) ;
		XYZ[1] = (11.5 - sin(b) * r) -(11.5 - sin(a) * r);
		XYZ[2] = 0;

		normal(XYZ);
		glNormal3f(XYZ[0], XYZ[1], XYZ[2]);
		glTexCoord2d(0, 0);
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 0);
		glTexCoord2d(0, 0.65);
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 5);
		glTexCoord2d(0.65, 0);
		glVertex3f(4 - cos(b) * r, 11.5 - sin(b) * r, 5);
		glTexCoord2d(0.65, 1);
		glVertex3f(4 - cos(b) * r, 11.5 - sin(b) * r, 0);
		glEnd();
	}

	for (int i = 0; i <= 9; i++) {
		float a = (float)i / (float)10 * 3.1415f * (-0.95);
		float b = (float)(i + 1) / (float)10 * 3.1415f * (-0.95);
		glBegin(GL_QUADS);
		XYZ[0] = 4 - cos(a) * r - (4 - cos(b) * r);
		XYZ[1] = 11.5 - sin(a) * r - (11.5 - sin(b) * r);
		XYZ[2] = 0;
		normal(XYZ);
		glNormal3f(XYZ[0], XYZ[1], XYZ[2]);
		glTexCoord2d(0, 0);
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 0);
		glTexCoord2d(0, 0.65);
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 5);
		glTexCoord2d(0.65, 0);
		glVertex3f(4 - cos(b) * r, 11.5 - sin(b) * r, 5);
		glTexCoord2d(0.65, 1);
		glVertex3f(4 - cos(b) * r, 11.5 - sin(b) * r, 0);
		glEnd();
	}

}

void treug1() {
	glNormal3d(0, 0, -1);
	float r = 3.041; //������
	glColor3d(0.3, 0.35, 0.42);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(4, 11.5, 0); //����� ����������
	
	for (int i = 0; i <= 10; i++) {
		
		float a = (float)i * 0.55 / (float)10 * 3.1415f * 0.1;
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 0);
	}
	for (int i = 0; i <= 10; i++) {
		
		float a = (float)i / (float)10 * 3.1415f * (-0.95);
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 0);
	}

	glEnd();
}
void treug2() {
	glNormal3d(0, 0, 1);
	float r = 3.041; //������
	glColor3d(0.62, 0.68, 0.82);
	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(4, 11.5, 5); //����� ����������
	for (int i = 0; i <= 10; i++) {
		float a = (float)i * 0.55 / (float)10 * 3.1415f * 0.1;
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 5);
	}
	for (int i = 0; i <= 10; i++) {
		float a = (float)i / (float)10 * 3.1415f * (-0.95);
		glVertex3f(4 - cos(a) * r, 11.5 - sin(a) * r, 5);
	}
	glEnd();
}

 //��� ���������
void treug3() {
	float X = 13.66667, Y = -14, R = 19.56;
	double N[3];
	double* n = N;
	glBegin(GL_QUADS);
	glColor3d(1, 0.75, 0.79);
	for (float p = 1.81; p <= 2.345; p += 0.001) {

		float x1 = X + cos(p) * R;
		float y1 = Y + sin(p) * R;
		float x2 = X + cos(p + 0.001) * R;
		float y2 = Y + sin(p + 0.001) * R;
		double xx1[3], xx2[3], xx3[3];
		xx1[0] = x1;
		xx1[1] = y1;
		xx1[2] = 0;
		xx2[0] = x1;
		xx2[1] = y1;
		xx2[2] = 5;
		xx3[0] = x2;
		xx3[1] = y2;
		xx3[2] = 0;

		CalcNormal(xx2, xx1, xx3, n);
		N[0] *= (-1);
		N[1] *= (-1);
		glNormal3dv(N);
		glVertex3f(x1, y1, 0);
		glVertex3f(x2, y2, 0);
		glVertex3f(x2, y2, 5);
		glVertex3f(x1, y1, 5);
	}
	glEnd();
}

void treug0()
{

	glBindTexture(GL_TEXTURE_2D, texId);
	
	//��� ���������1
	glBegin(GL_POLYGON);
	float X = 13.66667, Y = -14, R = 19.56;
	glNormal3d(0, 0, -1);
	glColor3d(0, 1, 0);
	glTexCoord2d(1, 0.6);
	glVertex3d(5, 5, 0);
	for (float a = 4.95; a <= 5.48; a += 0.001) {

		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;

		glTexCoord2d(0.25, 0.6);
		glVertex3d(X1, Y1, 0);
	}
	glEnd();
	
	//��� ���������2
	glBegin(GL_POLYGON);
	
	glNormal3d(0, 0, -1);
	glColor3d(0, 1, 0);
	glTexCoord2d(1, 1);
	glVertex3d(2, 4.53, 0);
	for (float a = 4.95; a <= 5.4863; a += 0.001) {

		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;

		glTexCoord2d(0.5, 0.5);
		glVertex3d(X1, Y1, 0);
	}
	glEnd();


	glBegin(GL_TRIANGLES);
	glColor3d(0, 1, 0);
	glTexCoord2d(0, 0);
	glVertex3d(0, 0, 0);
	glTexCoord2d(0.05, 0.95);
	glVertex3d(4, 9, 0);
	glTexCoord2d(0.75, 0.65);
	glVertex3d(5, 5, 0);
	glEnd(); 


	glBegin(GL_TRIANGLES);
	glColor3d(0, 1, 0);
	glTexCoord2d(0, 1);
	glVertex3d(5, 5, 0);
	glTexCoord2d(0.05, 0.35);
	glVertex3d(4, 9, 0);
	glTexCoord2d(0.65, 0.25);
	glVertex3d(7, 12, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(0, 1, 0);
	glTexCoord2d(0, 0);
	glVertex3d(4, 9, 0);
	glTexCoord2d(0.025, 1);
	glVertex3d(7, 12, 0);
	glTexCoord2d(1, 1);
	glVertex3d(1, 11, 0);
	glEnd();


	//����

	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glColor3d(1, 0, 0);
	glTexCoord2d( 0.2,0.9);
	glVertex3d(5, 5, 5);
	for (float a = 4.95; a <= 5.48; a += 0.001) {
		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;
		glTexCoord2d(0, 0.4);
		glVertex3d(X1, Y1, 5);
	}
	glEnd();

	//���� ���������2
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glColor3d(1, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3d(2, 4.53, 5);
	for (float a = 4.95; a <= 5.4863; a += 0.001) {

		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;

		glTexCoord2d(0.2, 0.5);
		glVertex3d(X1, Y1, 5);
	}
	glEnd();

	//glBegin(GL_TRIANGLES);
	//glColor3d(1, 0, 0);
	//glTexCoord2d(0, 1);
	//glVertex3d(2, 4.53, 0);
	//glTexCoord2d(0.7, 1);
	//glVertex3d(4, 3, 0);
	//glTexCoord2d(1, 1);
	//glVertex3d(5, 5, 0);
	//glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);
	glTexCoord2d(0, 0);
	glVertex3d(0, 0, 5);
	glTexCoord2d(0.05, 0.95);
	glVertex3d(4, 9, 5);
	glTexCoord2d(0.75, 0.65);
	glVertex3d(5, 5, 5);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);
	glTexCoord2d(0, 1);
	glVertex3d(5, 5, 5);
	glTexCoord2d(0.05, 0.35);
	glVertex3d(4, 9, 5);
	glTexCoord2d(0.65, 0.25);
	glVertex3d(7, 12, 5);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);
	glTexCoord2d(0, 0);
	glVertex3d(4, 9, 5);
	glTexCoord2d(0.025, 1);
	glVertex3d(7, 12, 5);
	glTexCoord2d(1, 1);
	glVertex3d(1, 11, 5);
	glEnd();

	//����

	glNormal3d(-45,20, 0);
	glBegin(GL_QUADS);
	glColor3d(1, 0.3, 0.5);
	glTexCoord2d(0, 0);
	glVertex3d(0, 0, 0);
	glTexCoord2d(0, 1);
	glVertex3d(0, 0, 5);
	glTexCoord2d(1, 0);
	glVertex3d(4, 9, 5);
	glTexCoord2d(1, 1);
	glVertex3d(4, 9, 0);
	glEnd();

	glNormal3d(-10, -15, 0);
	glBegin(GL_QUADS);
	glColor3d(0.3, 1, 0.7);
	glTexCoord2d(0, 0);
	glVertex3d(4, 9, 0);
	glTexCoord2d(0, 1);
	glVertex3d(4, 9, 5);
	glTexCoord2d(1, 0);
	glVertex3d(1, 11, 5);
	glTexCoord2d(1, 1);
	glVertex3d(1, 11, 0);
	glEnd();


	glNormal3d(35, -10, 0);
	glBegin(GL_QUADS);
	glColor3d(0.6, 0.8, 0.4);
	glTexCoord2d(0, 0);
	glVertex3d(7, 12, 0);
	glTexCoord2d(0, 1);
	glVertex3d(7, 12, 5);
	glTexCoord2d(1, 0);
	glVertex3d(5, 5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(5, 5, 0);
	glEnd();

	glNormal3d(0, 1, 0);
	glBegin(GL_QUADS);
	glColor3d(0.9, 1, 0.4);
	glTexCoord2d(0, 0);
	glVertex3d(9, 5, 0);
	glTexCoord2d(0, 1);
	glVertex3d(9, 5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(5, 5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(5, 5, 0);
	glEnd();

	treug();
	treug1();
	treug2();
	treug3();

}

void Render(OpenGL *ogl)
{

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	treug0();

	//������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}