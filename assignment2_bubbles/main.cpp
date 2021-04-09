#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <bits/stdc++.h>

#include <windows.h>
#include <glut.h>
using namespace std;

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
	double x,y,z;
};

point yellowBallPosition, greenBallPosition, yellowBallVector, greenBallVector;


void setYballVector(double xCoord, double yCoord)
{
    yellowBallVector.x = xCoord;
    yellowBallVector.y = yCoord;
    yellowBallVector.z = 0.0;
}


void setGballVector(double xCoord, double yCoord)
{
    greenBallVector.x = xCoord;
    greenBallVector.y = yCoord;
    greenBallVector.z = 0.0;
}


void setYballPosition(double xCoord, double yCoord)
{
    yellowBallPosition.x = xCoord;
    yellowBallPosition.y = yCoord;
    yellowBallPosition.z = 0.0;
}


void setGballPosition(double xCoord, double yCoord)
{
    greenBallPosition.x = xCoord;
    greenBallPosition.y = yCoord;
    greenBallPosition.z = 0.0;
}



void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}



void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}




void drawBubbleOffline()
{
    double angForRay;
    glColor3f(1,1,1);
    drawCircle(75,99);
    point forRay;
    glPushMatrix();

    glColor3f(1,1,0);
    glTranslatef(yellowBallPosition.x, yellowBallPosition.y, 0);
    glBegin(GL_LINES);
    {
        glColor3f(1,1,1);
        angForRay  = atan2(yellowBallVector.y, yellowBallVector.x);
        forRay.x = 8 * cos(angForRay);
        forRay.y = 8 * sin(angForRay);
        glVertex3f(0, 0, 0);
        glVertex3f(forRay.x, forRay.y,0);
        //glVertex3f(5,yellowBallVector.y+5,0);
        //glVertex3f(forRay.x, forRay.y,0);

    }glEnd();
    glColor3f(1,1,0);
    drawCircle(8,30);

        point arrowVecY, perpNewVec, newVecY;
        perpNewVec.x = -yellowBallVector.y;
        perpNewVec.y = yellowBallVector.x;

        point Left,Right;
        newVecY.x = yellowBallVector.x * cos(30 * pi / 180) + perpNewVec.x * sin(30 * pi / 180);
        newVecY.y = yellowBallVector.y * cos(30 * pi / 180) + perpNewVec.y * sin(30 * pi / 180);
        Left.x = newVecY.x*60;
        Left.y = newVecY.y*60;

        newVecY.x = yellowBallVector.x * cos(30 * pi / 180) - perpNewVec.x * sin(30 * pi / 180);
        newVecY.y = yellowBallVector.y * cos(30 * pi / 180) - perpNewVec.y * sin(30 * pi / 180);
        Right.x = newVecY.x*60;
        Right.y = newVecY.y*60;

    glColor3f(1,0,0);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(forRay.x,forRay.y,forRay.z);
        glVertex3f(Left.x,Left.y,Left.z);
        glVertex3f(Right.x,Right.y,Right.z);
    }
    glEnd();
    glPopMatrix();


    glColor3f(0,1,0);
    glTranslatef(greenBallPosition.x, greenBallPosition.y, 0);
    glBegin(GL_LINES);
    {
        glColor3f(1,1,1);
        angForRay  = atan2(greenBallVector.y, greenBallVector.x);
        forRay.x = 8 * cos(angForRay);
        forRay.y = 8 * sin(angForRay);
        glVertex3f(0, 0, 0);
        glVertex3f(forRay.x, forRay.y,0);


        point arrowVecG, perpNewVecG, newVecG;

            perpNewVecG.x = greenBallVector.y;
            perpNewVecG.y = -greenBallVector.x;


            newVecG.x = greenBallVector.x * cos(30 * pi / 180) + perpNewVecG.x * sin(30 * pi / 180);
            newVecG.y = greenBallVector.y * cos(30 * pi / 180) + perpNewVecG.y * sin(30 * pi / 180);
            arrowVecG.x = newVecG.x*0.6;
            arrowVecG.y = newVecG.y*0.6;
    }glEnd();
    glColor3f(0,1,0);
    drawCircle(8,30);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			drawgrid=1-drawgrid;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
    point newYvec, perpYvec;
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
            perpYvec.x = -yellowBallVector.y;
			perpYvec.y = yellowBallVector.x;
            newYvec.x = yellowBallVector.x * cos(2*pi / 180) + (-1) * perpYvec.x * sin(2*pi / 180);
            newYvec.y = yellowBallVector.y * cos(2*pi / 180) + (-1) * perpYvec.y * sin(2*pi / 180);
            yellowBallVector.x = newYvec.x;
            yellowBallVector.y = newYvec.y;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			perpYvec.x = -yellowBallVector.y;
			perpYvec.y = yellowBallVector.x;
            newYvec.x = yellowBallVector.x * cos(2*pi / 180) + perpYvec.x * sin(2*pi / 180);
            newYvec.y = yellowBallVector.y * cos(2*pi / 180) + perpYvec.y * sin(2*pi / 180);
            yellowBallVector.x = newYvec.x;
            yellowBallVector.y = newYvec.y;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,100,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	//drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    //drawCircle(70,90);


    //drawCone(20,50,24);

	//drawSphere(30,24,20);
	drawBubbleOffline();




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.02;
	//codes for any changes in Models, Camera
	double newYballPositionX, newYballPositionY, newGballPositionX, newGballPositionY;
    newYballPositionX = yellowBallPosition.x+yellowBallVector.x;
    newYballPositionY = yellowBallPosition.y+yellowBallVector.y;
    newGballPositionX = greenBallPosition.x+greenBallVector.x;
    newGballPositionY = greenBallPosition.y+greenBallVector.y;


    double C1_C2_Y = sqrt((newYballPositionX * newYballPositionX) + (newYballPositionY * newYballPositionY));
    double C1_C2_G = sqrt((newGballPositionX * newGballPositionX) + (newGballPositionY * newGballPositionY));
    double R1_R2 = (75.0-8.0);
    double r1_r2 = (8.0+8.0);
    double c1_c2 = sqrt(((newGballPositionX- newYballPositionX) * (newGballPositionX- newYballPositionX)) +  ((newGballPositionY- newYballPositionY) * (newGballPositionY- newYballPositionY)));
    double thetaAngle;
    point forNvec, Nvec;
    if(R1_R2 <= C1_C2_Y)
    {
        //printf("a");
        //thetaAngle = atan2(yellowBallPosition.y, yellowBallPosition.x) * 180 / pi;
        //printf("%f\n", thetaAngle);
        //forNvec.x = 75 * cos(thetaAngle);
        //forNvec.y = 75 * sin(thetaAngle);
        //printf("%f %f\n", forNvec.x, forNvec.y);

        Nvec.x = -yellowBallPosition.x;
        Nvec.y = -yellowBallPosition.y;
        double normalize = sqrt((Nvec.x*Nvec.x)+(Nvec.y*Nvec.y));
        Nvec.z = 0.0;

        Nvec.x /= normalize;
        Nvec.y /= normalize;

        double Ax_Bx = (yellowBallVector.x * Nvec.x);
        double Ay_By = (yellowBallVector.y * Nvec.y);
        double a_dot_n = Ax_Bx + Ay_By;
        double mult = 2 * a_dot_n;

        Nvec.x *= mult;
        Nvec.y *= mult;

        yellowBallVector.x -= Nvec.x;
        yellowBallVector.y -= Nvec.y;
        newYballPositionX = yellowBallPosition.x+yellowBallVector.x;
        newYballPositionY = yellowBallPosition.y+yellowBallVector.y;
        //newGballPositionX = greenBallPosition.x+greenBallVector.x;
        //newGballPositionY = greenBallPosition.y+greenBallVector.y;
        //setYballPosition(newYballPositionX, newYballPositionY);
        setGballPosition(newGballPositionX, newGballPositionY);
    }
    else
    {
        setYballPosition(newYballPositionX, newYballPositionY);
        //setGballPosition(newGballPositionX, newGballPositionY);
    }
    if(R1_R2 <= C1_C2_G)
    {
        //printf("a");
        //thetaAngle = atan2(yellowBallPosition.y, yellowBallPosition.x) * 180 / pi;
        //printf("%f\n", thetaAngle);
        //forNvec.x = 75 * cos(thetaAngle);
        //forNvec.y = 75 * sin(thetaAngle);
        //printf("%f %f\n", forNvec.x, forNvec.y);

        Nvec.x = -greenBallPosition.x;
        Nvec.y = -greenBallPosition.y;
        double normalize = sqrt((Nvec.x*Nvec.x)+(Nvec.y*Nvec.y));
        Nvec.z = 0.0;

        Nvec.x /= normalize;
        Nvec.y /= normalize;

        double Ax_Bx = (greenBallVector.x * Nvec.x);
        double Ay_By = (greenBallVector.y * Nvec.y);
        double a_dot_n = Ax_Bx + Ay_By;
        double mult = 2 * a_dot_n;

        Nvec.x *= mult;
        Nvec.y *= mult;

        greenBallVector.x -= Nvec.x;
        greenBallVector.y -= Nvec.y;
        //newYballPositionX = yellowBallPosition.x+yellowBallVector.x;
        //newYballPositionY = yellowBallPosition.y+yellowBallVector.y;
        newGballPositionX = greenBallPosition.x+greenBallVector.x;
        newGballPositionY = greenBallPosition.y+greenBallVector.y;
        //setYballPosition(newYballPositionX, newYballPositionY);
        setGballPosition(newGballPositionX, newGballPositionY);
    }
    else
    {
        //setYballPosition(newYballPositionX, newYballPositionY);
        setGballPosition(newGballPositionX, newGballPositionY);
    }

    if(c1_c2 <=  r1_r2)
    {
        point forNvec1, Nvec1, forNvec2, Nvec2;
        forNvec.x = (newGballPositionX + newYballPositionX)/2;
        forNvec.y = (newGballPositionY + newYballPositionY)/2;

        Nvec1.x = newGballPositionX - forNvec.x;
        Nvec1.y = newGballPositionY - forNvec.y;
        double normalize1 = sqrt((Nvec1.x*Nvec1.x)+(Nvec1.y*Nvec1.y));
        Nvec1.z = 0.0;

        Nvec1.x /= normalize1;
        Nvec1.y /= normalize1;


        Nvec2.x = newYballPositionX - forNvec.x;
        Nvec2.y = newYballPositionY - forNvec.y;
        double normalize2 = sqrt((Nvec2.x*Nvec2.x)+(Nvec2.y*Nvec2.y));
        Nvec2.z = 0.0;

        Nvec2.x /= normalize2;
        Nvec2.y /= normalize2;

        double Ax_Bx_1 = (greenBallVector.x * Nvec1.x);
        double Ay_By_1 = (greenBallVector.y * Nvec1.y);
        double a_dot_n_1 = Ax_Bx_1 + Ay_By_1;
        double mult_1 = 2 * a_dot_n_1;

        Nvec1.x *= mult_1;
        Nvec1.y *= mult_1;

        greenBallVector.x -= Nvec1.x;
        greenBallVector.y -= Nvec1.y;
        newGballPositionX = greenBallPosition.x+greenBallVector.x;
        newGballPositionY = greenBallPosition.y+greenBallVector.y;
        setGballPosition(newGballPositionX, newGballPositionY);


        double Ax_Bx_2 = (yellowBallVector.x * Nvec2.x);
        double Ay_By_2 = (yellowBallVector.y * Nvec2.y);
        double a_dot_n_2 = Ax_Bx_2 + Ay_By_2;
        double mult_2 = 2 * a_dot_n_2;

        Nvec2.x *= mult_2;
        Nvec2.y *= mult_2;

        yellowBallVector.x -= Nvec2.x;
        yellowBallVector.y -= Nvec2.y;
        newYballPositionX = yellowBallPosition.x+yellowBallVector.x;
        newYballPositionY = yellowBallPosition.y+yellowBallVector.y;
        setYballPosition(newYballPositionX, newYballPositionY);

    }
    else
    {
        setYballPosition(newYballPositionX, newYballPositionY);
        setGballPosition(newGballPositionX, newGballPositionY);
    }



	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

    setYballPosition(-40, 20);
    setGballPosition(-20, -8);
    setYballVector(0.03,0.03);
    setGballVector(0.03,-0.03);
	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
