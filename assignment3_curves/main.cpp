#include<stdio.h>
#include<bits/stdc++.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include <windows.h>
#include <glut.h>
using namespace std;

#define pi (2*acos(0.0))

struct point2d
{
    double x, y;
};

struct hermitGeometry
{
    double p1, p4, r1, r4;
};
point2d cp[200];
point2d beginPoint, endPoint;
int cpidx;
int offlineState;
int pointOrVector;
int geometryToggle;
int updateCurvePoint;
point2d replacingCoord;
int minDistanceIndex;
int clickNo;
int curveTraversal;
int curveTraverseIndexC;
int curveTraverseIndexP;
double slow;

int curveNo, curvePointNo;
point2d curveTraverse[105][105];

void rotateOnXY(point2d &a, double ang)
{
    point2d forA;

    forA.x = a.x * cos(ang * pi / 180) - a.y * sin(ang * pi / 180);
    forA.y = a.y * cos(ang * pi / 180) + a.x * sin(ang * pi / 180);

    a = forA;
}

void hermitBasisMatrixMult(hermitGeometry &a)
{
    hermitGeometry forA;

    forA.p1 = 2 * a.p1 - 2 * a.p4 + a.r1 + a.r4;
    forA.p4 = (-3) * a.p1 + 3 * a.p4 + (-2) * a.r1 - a.r4;
    forA.r1 = a.r1;
    forA.r4 = a.p1;

    a = forA;
}


void drawSquare()
{
    glBegin(GL_QUADS);
    {
        glVertex3d( 3,  3, 0);
        glVertex3d( 3, -3, 0);
        glVertex3d(-3, -3, 0);
        glVertex3d(-3,  3, 0);
    }
    glEnd();
}

void drawCircle(double radius,int segments)
{
    int i;
    point2d points[105];
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


void drawCurvePoints()
{
    point2d testCurvePoint1, testCurvePoint2, testCurvePointV1, testCurvePointV2;
    double increment = 0.01;
    point2d fun, del1fun, del2fun, del3fun, prevFun;

    if(geometryToggle != 1)
    {
        for (int i=0; i<cpidx; i++)
        {
            //glColor3f(0, 1, 0);
            if((i%2) == 0)
            {
                glColor3f(0, 1, 0);
            }
            else
            {
                glColor3f(1, 0, 0);
            }
            glPushMatrix();
            {
                if((i%2) == 1)
                {
                    point2d arrowVec;
                    arrowVec.x = (cp[i].x - cp[i-1].x);
                    arrowVec.y = (cp[i].y - cp[i-1].y);
                    //cout << "vec: " << arrowVec.x << " " << arrowVec.y << "\n";

                    point2d perpArrowVec, arrowPos1, arrowPos2, newArrowVec, arrowBasePoint;
                    perpArrowVec.x = arrowVec.y;
                    perpArrowVec.y = -arrowVec.x;

                    /*newArrowVec.x = arrowVec.x * (-1) * cos(10*pi/180) + perpArrowVec.x * (-1) * sin(10*pi/180);
                    newArrowVec.y = arrowVec.y * (-1) * cos(10*pi/180) + perpArrowVec.y * (-1) * sin(10*pi/180);

                    arrowPos1 = newArrowVec;
                    arrowPos1.x *= 0.2;
                    arrowPos1.y *= 0.2;


                    newArrowVec.x = arrowVec.x * cos(30*pi/180) + perpArrowVec.x * sin(30*pi/180);
                    newArrowVec.y = arrowVec.y * cos(30*pi/180) + perpArrowVec.y * sin(30*pi/180);

                    arrowPos2 = newArrowVec;
                    arrowPos2.x *= 0.2;
                    arrowPos2.y *= 0.2;

                    glBegin(GL_TRIANGLES);
                    {
                        glColor3f(1, 0, 0);
                        glVertex3f(cp[i].x, cp[i].y, 0);
                        glVertex3f(arrowPos1.x,arrowPos1.y,0);
                        glVertex3f(arrowPos2.x,arrowPos2.y,0);
                    }
                    glEnd();*/


                    arrowBasePoint.x = cp[i-1].x + 0.8 * arrowVec.x;
                    arrowBasePoint.y = cp[i-1].y + 0.8 * arrowVec.y;

                    /*glTranslatef(arrowBasePoint.x, arrowBasePoint.y, 0);
                    drawSquare();
                    glTranslatef(-arrowBasePoint.x, -arrowBasePoint.y, 0);*/

                    double parametricT = 5 / sqrt(arrowVec.x*arrowVec.x + arrowVec.y*arrowVec.y);

                    arrowPos1.x = arrowBasePoint.x + parametricT * perpArrowVec.x;
                    arrowPos1.y = arrowBasePoint.y + parametricT * perpArrowVec.y;

                    arrowPos2.x = arrowBasePoint.x - parametricT * perpArrowVec.x;
                    arrowPos2.y = arrowBasePoint.y - parametricT * perpArrowVec.y;

                    glBegin(GL_TRIANGLES);
                    {
                        glColor3f(1, 0, 0);
                        glVertex3f(cp[i].x, cp[i].y, 0);
                        glVertex3f(arrowPos1.x,arrowPos1.y,0);
                        glVertex3f(arrowPos2.x,arrowPos2.y,0);
                    }
                    glEnd();

                    glBegin(GL_LINES);
                    {
                        glColor3f(1, 1, 1);
                        glVertex3f(cp[i-1].x, cp[i-1].y, 0);
                        glVertex3f(arrowBasePoint.x, arrowBasePoint.y, 0);
                    }
                    glEnd();



                    glColor3f(1, 1, 0);
                }
                glTranslatef(cp[i].x, cp[i].y, 0);
                drawSquare();
            }
            glPopMatrix();
        }
    }



    if(offlineState == 1)
    {
        glColor3f(1, 1, 1);
        /*glTranslatef(cp[i].x + 15, cp[i].y + 15, 0);
        drawSquare();
        glTranslatef(-cp[i].x - 15, -cp[i].y - 15, 0);*/

        for(int i=3; i<=cpidx; i++)
        {
            if(i == cpidx)
            {
                testCurvePoint1.x = cp[cpidx-2].x;
                testCurvePoint1.y = cp[cpidx-2].y;
                testCurvePointV1.x = cp[cpidx-1].x - cp[cpidx-2].x;
                testCurvePointV1.y = cp[cpidx-1].y - cp[cpidx-2].y;

                testCurvePoint2.x = cp[0].x;
                testCurvePoint2.y = cp[0].y;
                testCurvePointV2.x = cp[1].x - cp[0].x;
                testCurvePointV2.y = cp[1].y - cp[0].y;
            }
            else if((i%2) == 1)
            {
                testCurvePoint1.x = cp[i-3].x;
                testCurvePoint1.y = cp[i-3].y;
                testCurvePointV1.x = cp[i-2].x - cp[i-3].x;
                testCurvePointV1.y = cp[i-2].y - cp[i-3].y;

                testCurvePoint2.x = cp[i-1].x;
                testCurvePoint2.y = cp[i-1].y;
                testCurvePointV2.x = cp[i].x - cp[i-1].x;
                testCurvePointV2.y = cp[i].y - cp[i-1].y;
            }
            else continue;


            hermitGeometry forCoeffX, forCoeffY;
            forCoeffX.p1 = testCurvePoint1.x;
            forCoeffX.p4 = testCurvePoint2.x;
            forCoeffX.r1 = testCurvePointV1.x;
            forCoeffX.r4 = testCurvePointV2.x;

            forCoeffY.p1 = testCurvePoint1.y;
            forCoeffY.p4 = testCurvePoint2.y;
            forCoeffY.r1 = testCurvePointV1.y;
            forCoeffY.r4 = testCurvePointV2.y;

            hermitBasisMatrixMult(forCoeffX);
            hermitBasisMatrixMult(forCoeffY);
            //cout << forCoeffX.p1 << " " << forCoeffX.p4 << " " << forCoeffX.r1 << " " << forCoeffX.r4 << " -----> ax,bx,cx,dx\n";



            fun.x = forCoeffX.r4;
            del1fun.x = (forCoeffX.p1 * increment * increment * increment) + (forCoeffX.p4 * increment * increment) + (forCoeffX.r1 * increment);
            del2fun.x = (6 * forCoeffX.p1 * increment * increment * increment) + (2 * forCoeffX.p4 * increment * increment);
            del3fun.x = (6 * forCoeffX.p1 * increment * increment * increment);

            fun.y = forCoeffY.r4;
            del1fun.y = (forCoeffY.p1 * increment * increment * increment) + (forCoeffY.p4 * increment * increment) + (forCoeffY.r1 * increment);
            del2fun.y = (6 * forCoeffY.p1 * increment * increment * increment) + (2 * forCoeffY.p4 * increment * increment);
            del3fun.y = (6 * forCoeffY.p1 * increment * increment * increment);

            prevFun = fun;
            curvePointNo = -1;
            curveTraverse[++curveNo][++curvePointNo].x = fun.x;
            curveTraverse[curveNo][curvePointNo].y = fun.y;
            //curveNo++;

            //cout << increment * increment *increment;
            //cout << "fun---> " <<fun.x << " " << forCoeffX.r4 << "\n";

            /*glBegin(GL_LINES);
            {
                glVertex3f(cp[i-3].x, cp[i-3].y, 0);
                glVertex3f(fun.x, fun.y, 0);
            }
            glEnd();*/

            for(int j=0 ; j<=100; j++)
            {
                fun.x += del1fun.x;
                del1fun.x += del2fun.x;
                del2fun.x += del3fun.x;

                fun.y += del1fun.y;
                del1fun.y += del2fun.y;
                del2fun.y += del3fun.y;

                curveTraverse[curveNo][++curvePointNo].x = fun.x;
                curveTraverse[curveNo][curvePointNo].y = fun.y;

                /*glBegin(GL_LINES);
                {
                    glVertex3f(prevFun.x, prevFun.y, 0);
                    glVertex3f(fun.x, fun.y, 0);
                }
                glEnd();*/

                prevFun = fun;
            }

            /*glBegin(GL_LINES);
            {
                glVertex3f(fun.x, fun.y, 0);
                glVertex3f(cp[i].x, cp[i].y, 0);
            }
            glEnd();*/
        }
        offlineState = 2;
    }
    if(offlineState == 2)
    {
        for(int i=0; i<=curveNo; i++)
        {
            for(int j=0; j<curvePointNo; j++)
            {
                glBegin(GL_LINES);
                {
                    glColor3f(1, 1, 1);
                    glVertex3f(curveTraverse[i][j].x, curveTraverse[i][j].y, 0);
                    glVertex3f(curveTraverse[i][j+1].x, curveTraverse[i][j+1].y, 0);
                }
                glEnd();
            }
        }
    }

    if(updateCurvePoint == 1)
    {
        if(clickNo == 1)
        {
            glColor3f(0, 1, 1);
            double it = 0.01;
            while(it < 10)
            {
                glTranslatef(replacingCoord.x, replacingCoord.y, 0);
                //drawSquare();
                drawCircle(it, 42);
                glTranslatef(-replacingCoord.x, -replacingCoord.y, 0);
                it = it + 0.01;
            }
        }
    }
    if(curveTraversal == 1)
    {
        glColor3f(0, 1, 1);
        glPushMatrix();
        {
            double it = 0.01;
            while(it < 5)
            {
                glTranslatef(curveTraverse[curveTraverseIndexC][curveTraverseIndexP].x, curveTraverse[curveTraverseIndexC][curveTraverseIndexP].y, 0);
                //drawSquare();
                drawCircle(it, 42);
                glTranslatef(-curveTraverse[curveTraverseIndexC][curveTraverseIndexP].x, -curveTraverse[curveTraverseIndexC][curveTraverseIndexP].y, 0);
                it = it + 0.01;
            }
        }
        glPopMatrix();
    }
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

        case 'g':
            geometryToggle = 1 - geometryToggle;
			break;
        case 'u':
            if(offlineState == 2)
            {
                updateCurvePoint = 1;
                curveTraversal = 0;
            }
			break;
        case 'a':
            if(offlineState == 2)
            {
                curveTraversal = 1 - curveTraversal;
            }
			break;

		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			break;
		case GLUT_KEY_UP:		// up arrow key
			break;

		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
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
		    //pointOrVector = 1 - pointOrVector;
		    if(offlineState == 0 && state == GLUT_DOWN)
            {
                cp[cpidx].x = (double)x;
                cp[cpidx].y = (double)(600 - y);
                cpidx++;
		    }

		    if(updateCurvePoint == 1 && state == GLUT_DOWN)
		    {
		        if(clickNo == 0)
                {
                    //cout << clickNo << "\n";
                    double xCoord, yCoord;
                    double minDistancePoint = 100000.0, distanceFromPoint;

                    xCoord = (double)x;
                    yCoord = (double)(600 - y);
                    for(int i=0; i<cpidx; i++)
                    {
                        distanceFromPoint = sqrt((xCoord - cp[i].x) * (xCoord - cp[i].x) + (yCoord - cp[i].y) * (yCoord - cp[i].y));
                        if(distanceFromPoint <  minDistancePoint)
                        {
                            minDistancePoint = distanceFromPoint;
                            minDistanceIndex = i;
                        }
                    }
                    replacingCoord.x = cp[minDistanceIndex].x;
                    replacingCoord.y = cp[minDistanceIndex].y;
                }
                else
                {
                    //cout << clickNo << "\n";
                    cp[minDistanceIndex].x = (double)x;
                    cp[minDistanceIndex].y = (double)(600 - y);
                    updateCurvePoint = 0;
                    curveNo = -1;
                    offlineState = 1;
                }
                clickNo = 1 - clickNo;
                //cout << clickNo << "\n";
		    }

			break;

		case GLUT_RIGHT_BUTTON:
			offlineState = 1;
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
	//gluLookAt(150*cos(cameraAngle), 150*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,0,	0,0,-1,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects


	/*int i;

    for (i = 0; i < cpidx; i++)
    {
        glColor3f(1, 1, 0);
        glPushMatrix();
        {
            glTranslatef(cp[i].x, cp[i].y, 0);
            drawSquare();
        }
        glPopMatrix();
    }*/
    drawCurvePoints();



    /*while(slow < 30)
    {
        slow = slow + 30;
    }
    slow = 0.0;*/
    curveTraverseIndexP++;
    if(curveTraverseIndexP > curvePointNo)
    {
        curveTraverseIndexC++;
        if(curveTraverseIndexC > curveNo)
        {
            curveTraverseIndexC = 0;
        }
        curveTraverseIndexP = 0;
    }
    //cout << curveTraverseIndexC << " " << curveTraverseIndexP << " " << curveNo << " " << curvePointNo << "\n";

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){


	//codes for any changes in Models, Camera
	glutPostRedisplay();


}

void init(){
	//codes for initialization

	cpidx = 0;
	offlineState = 0;
	pointOrVector = 0;
	geometryToggle = 0;
	updateCurvePoint = 0;
	clickNo = 0;
	curveTraversal = 0;
	curveNo = -1;
	curvePointNo = -1;
	curveTraverseIndexC = 0;
	curveTraverseIndexP = 0;
	slow = 0.0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluOrtho2D(0, 800, 0, 600);
	//gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 600);
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
