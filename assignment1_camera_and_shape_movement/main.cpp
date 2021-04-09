#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
using namespace std;
#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double gunAngZ,gunAngX,gunBodyZ, gunBodyX, gunSpin;


struct point
{
	double x,y,z;
};
point pos, u, r, l;

point uhpoint, lhpoint;
point targetPoint;

vector<point> gunSpot;


void rotateOnX(point &a, double ang)
{
    point tmp;
    tmp.x = a.x;
    tmp.y = a.y*cos(ang * pi / 180) + a.z*sin(ang * pi / 180);
    tmp.z = a.z*cos(ang * pi / 180) - a.y*sin(ang * pi / 180);

    a = tmp;
}

void rotateOnY(point &a, double ang)
{
    point tmp;
    tmp.x = a.x*cos(ang * pi / 180) - a.z*sin(ang * pi / 180);
    tmp.y = a.y;
    tmp.z = a.z*cos(ang * pi / 180) + a.x*sin(ang * pi / 180);

    a = tmp;
}

void rotateOnZ(point &a, double ang)
{
    point tmp;
    tmp.x = a.x*cos(ang * pi / 180) + a.y*sin(ang * pi / 180);
    tmp.y = a.y*cos(ang * pi / 180) - a.x*sin(ang * pi / 180);
    tmp.z = a.z;

    a = tmp;
}



void setCameraPosition(double xCoord, double yCoord, double zCoord)
{
    pos.x = xCoord;
    pos.y = yCoord;
    pos.z = zCoord;
}

void setUvector(double xCoord, double yCoord, double zCoord)
{
    u.x = xCoord;
    u.y = yCoord;
    u.z = zCoord;
}

void setRvector(double xCoord, double yCoord, double zCoord)
{
    r.x = xCoord;
    r.y = yCoord;
    r.z = zCoord;
}

void setLvector(double xCoord, double yCoord, double zCoord)
{
    l.x = xCoord;
    l.y = yCoord;
    l.z = zCoord;
}
void seeDotProduct(point a, point b)
{
    double dProduct = ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
    printf("Dot Product %f", dProduct);
}

void line_offline(point a, point b)
{
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);{
        glVertex3f(a.x,a.y,a.z);
        glVertex3f(b.x,b.y,b.z);
    }glEnd();
}

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-300,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
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

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, 0,-a);
		glVertex3f( a,0,a);
		glVertex3f(-a,0,a);
		glVertex3f(-a, 0,-a);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
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

void drawCylinderOffline(double radius,int slices,int gunSize)
{
    struct point points[100];
	int j;
	double h,r;
	//generate points
		for(j=0;j<=slices;j++)
		{
			points[j].x=radius*cos(((double)j/(double)slices)*2*pi);
			points[j].y=0;
			points[j].z=radius*sin(((double)j/(double)slices)*2*pi);
		}
	//draw quads using generated points
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        /*if((i % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }*/
		for(j=0;j<slices;j++)
		{
		    if((j % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[j].x,points[j].y,points[j].z);
				glVertex3f(points[j].x,points[j].y+gunSize,points[j].z);
				glVertex3f(points[j+1].x,points[j+1].y+gunSize,points[j+1].z);
				glVertex3f(points[j+1].x,points[j+1].y,points[j+1].z);
			}glEnd();
		}
}



void drawSphereU(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=h;
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        /*if((i % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }*/
		for(j=0;j<slices;j++)
		{
		    if((j % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }
			glBegin(GL_QUADS);{
			    //upper hemisphere
			    /*uhpoint.x = points[0][0].x;
			    uhpoint.y = points[0][0].y;
                uhpoint.z = points[0][0].z;*/

				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}

void drawSphereL(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=-h;
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
		}
	}
	/*lhpoint.x = points[0][0].x;
    lhpoint.y = points[0][0].y;
    lhpoint.z = points[0][0].z;*/
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        /*if((i % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }*/
		for(j=0;j<slices;j++)
		{
		    if((j % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }
			glBegin(GL_QUADS);{
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

void ak47(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=2*radius - radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=-h;
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        /*if((i % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }*/
		for(j=0;j<slices;j++)
		{
		    if((j % 2) == 0)
        {
            glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
            glColor3f(1.0, 1.0, 1.0);
        }
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                /*glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);*/
			}glEnd();
		}
	}
}




void drawGunOffline()
{
    //line_offline(uhpoint,lhpoint);
    for(int it=0; it<gunSpot.size(); it++)
    {
        glColor3f(1,0,0);
        glTranslatef(gunSpot[it].x,gunSpot[it].y+1,gunSpot[it].z);
        drawSquare(2);
        glTranslatef(-gunSpot[it].x,-gunSpot[it].y-1,-gunSpot[it].z);
    }
    glPushMatrix();
    {
    //double depth = 6*sin(((double)20/(double)21)*(pi/2));
    glRotatef(gunAngZ,0,0,1);
    glRotatef(gunAngX,1,0,0);
    glTranslatef(0,-6,0);
    glPushMatrix();
    {
        drawSphereU(6,28,20);
        glTranslatef(0,-20,0);
        drawCylinderOffline(6,28,20);
        drawSphereL(6,28,20);
    }
    glPopMatrix();

    glPushMatrix();
    {
    glTranslatef(0,-31,0);
    glRotatef(gunBodyX,1,0,0);
    glRotatef(gunSpin,0,-1,0);
    drawSphereU(5,28,20);
    glTranslatef(0,-50,0);
    drawCylinderOffline(5,28,50);
    //glRotatef(180, 0, 1, 0);
    ak47(5,28,20);
    }
    glPopMatrix();
    }
    glPopMatrix();
    glColor3f(0.13,0.13,0.13);
    glTranslatef(0,-200,0);
    //glRotatef(90,1,0,0);
    drawSquare(70);
}
void keyboardListener(unsigned char key, int x,int y){
    point newU, newR, newL;
	switch(key){

		case '1':
			//drawgrid=1-drawgrid;
			/*r.x *= cos(3*pi/180);
			r.y *= cos(3*pi/180);
			r.z *= cos(3*pi/180);

			l.x *= sin(3*pi/180);
			l.y *= sin(3*pi/180);
			l.z *= sin(3*pi/180);

			r.x += l.x;
			r.y += l.y;
			r.z += l.z;

            l.x *= cos(3*pi/180);
			l.y *= cos(3*pi/180);
			l.z *= cos(3*pi/180);

			r.x *= sin(3*pi/180);
			r.y *= sin(3*pi/180);
			r.z *= sin(3*pi/180);
			r.x *= (-1);
			r.y *= (-1);
			r.z *= (-1);

			l.x += r.x;
			l.y += r.y;
			l.z += r.z;*/
			//printf("%f", cos(30 * pi / 180));

			newR.x = r.x * cos(pi / 180) + l.x * sin(pi / 180);
			newR.y = r.y * cos(pi / 180) + l.y * sin(pi / 180);
			newR.z = r.z * cos(pi / 180) + l.z * sin(pi / 180);

			newL.x = l.x * cos(pi / 180) + r.x * sin(pi / 180) * (-1);
			newL.y = l.y * cos(pi / 180) + r.y * sin(pi / 180) * (-1);
			newL.z = l.z * cos(pi / 180) + r.z * sin(pi / 180) * (-1);

			r.x = newR.x;
			r.y = newR.y;
			r.z = newR.z;

			l.x = newL.x;
			l.y = newL.y;
			l.z = newL.z;


			break;

        case '2':

			newR.x = r.x * cos(pi / 180) + l.x * sin(pi / 180) * (-1);
			newR.y = r.y * cos(pi / 180) + l.y * sin(pi / 180) * (-1);
			newR.z = r.z * cos(pi / 180) + l.z * sin(pi / 180) * (-1);

			newL.x = l.x * cos(pi / 180) + r.x * sin(pi / 180);
			newL.y = l.y * cos(pi / 180) + r.y * sin(pi / 180);
			newL.z = l.z * cos(pi / 180) + r.z * sin(pi / 180);

			r.x = newR.x;
			r.y = newR.y;
			r.z = newR.z;

			l.x = newL.x;
			l.y = newL.y;
			l.z = newL.z;


			break;

        case '3':

			newL.x = l.x * cos(pi / 180) + u.x * sin(pi / 180);
			newL.y = l.y * cos(pi / 180) + u.y * sin(pi / 180);
			newL.z = l.z * cos(pi / 180) + u.z * sin(pi / 180);

            newU.x = u.x * cos(pi / 180) + l.x * sin(pi / 180) * (-1);
			newU.y = u.y * cos(pi / 180) + l.y * sin(pi / 180) * (-1);
			newU.z = u.z * cos(pi / 180) + l.z * sin(pi / 180) * (-1);

			u.x = newU.x;
			u.y = newU.y;
			u.z = newU.z;

			l.x = newL.x;
			l.y = newL.y;
			l.z = newL.z;


			break;

        case '4':

			newU.x = u.x * cos(pi / 180) + l.x * sin(pi / 180);
			newU.y = u.y * cos(pi / 180) + l.y * sin(pi / 180);
			newU.z = u.z * cos(pi / 180) + l.z * sin(pi / 180);

            newL.x = l.x * cos(pi / 180) + u.x * sin(pi / 180) * (-1);
			newL.y = l.y * cos(pi / 180) + u.y * sin(pi / 180) * (-1);
			newL.z = l.z * cos(pi / 180) + u.z * sin(pi / 180) * (-1);

			u.x = newU.x;
			u.y = newU.y;
			u.z = newU.z;

			l.x = newL.x;
			l.y = newL.y;
			l.z = newL.z;


			break;

        case '5':

			newU.x = u.x * cos(pi / 180) + r.x * sin(pi / 180);
			newU.y = u.y * cos(pi / 180) + r.y * sin(pi / 180);
			newU.z = u.z * cos(pi / 180) + r.z * sin(pi / 180);

            newR.x = r.x * cos(pi / 180) + u.x * sin(pi / 180) * (-1);
			newR.y = r.y * cos(pi / 180) + u.y * sin(pi / 180) * (-1);
			newR.z = r.z * cos(pi / 180) + u.z * sin(pi / 180) * (-1);

			u.x = newU.x;
			u.y = newU.y;
			u.z = newU.z;

			r.x = newR.x;
			r.y = newR.y;
			r.z = newR.z;


			break;

        case '6':

			newR.x = r.x * cos(pi / 180) + u.x * sin(pi / 180);
			newR.y = r.y * cos(pi / 180) + u.y * sin(pi / 180);
			newR.z = r.z * cos(pi / 180) + u.z * sin(pi / 180);

            newU.x = u.x * cos(pi / 180) + r.x * sin(pi / 180) * (-1);
			newU.y = u.y * cos(pi / 180) + r.y * sin(pi / 180) * (-1);
			newU.z = u.z * cos(pi / 180) + r.z * sin(pi / 180) * (-1);

			u.x = newU.x;
			u.y = newU.y;
			u.z = newU.z;

			r.x = newR.x;
			r.y = newR.y;
			r.z = newR.z;


			break;

        case 'q':
            if(gunAngZ <= 45)
            {
                gunAngZ += 0.7;
                rotateOnZ(uhpoint, -0.7);
                rotateOnZ(lhpoint, -0.7);
                //printf("%f %f %f\n",uhpoint.x,uhpoint.y,uhpoint.z);
                //printf("%f %f %f\n",lhpoint.x,lhpoint.y,lhpoint.z);
            }
            break;
        case 'w':
            if(gunAngZ >= -45)
            {
                gunAngZ -= 0.7;
                rotateOnZ(uhpoint, +0.7);
                rotateOnZ(lhpoint, +0.7);
                //printf("%f %f %f\n",uhpoint.x,uhpoint.y,uhpoint.z);
                //printf("%f %f %f\n",lhpoint.x,lhpoint.y,lhpoint.z);
            }
            break;
        case 'e':
            if(gunAngX >= -45)
            {
                gunAngX -= 0.7;
                rotateOnX(uhpoint, 0.7);
                rotateOnX(lhpoint, 0.7);
            }
            break;
        case 'r':
            if(gunAngX <= 45)
            {
                gunAngX += 0.7;
                rotateOnX(uhpoint, -0.7);
                rotateOnX(lhpoint, -0.7);
            }
            break;
        case 'a':
            if(gunBodyX <= 45)
            {
                gunBodyX += 0.7;
                rotateOnX(lhpoint, -0.7);
            }
            break;
        case 's':
            if(gunBodyX >= -45)
            {
                gunBodyX -= 0.7;
                rotateOnX(lhpoint, 0.7);
            }
            break;
        case 'd':
            if(gunSpin <= 45)
            {
                gunSpin += 0.7;
            }
            break;
        case 'f':
            if(gunSpin >= -45)
            {
                gunSpin -= 0.7;
            }
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraHeight -= 3.0;
			//pos.x -= 2* u.x;
			/**pos.y -= 2* l.y;**/
			//pos.z -= 2* u.z;
			pos.y += 2;
			break;
		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 3.0;
			//pos.x += 2* u.x;
			/**pos.y += 2* l.y;**/
			//pos.z += 2* u.z;
			pos.y -= 2;
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			/**pos.x += 2* r.x;**/
			//pos.y += 2* r.y;
			//pos.z += 2* r.z;
			pos.x -= 2;
			break;
		case GLUT_KEY_LEFT:
			//cameraAngle -= 0.03;
			/**pos.x -= 2* r.x;**/
			//pos.y -= 2* r.y;
			//pos.z -= 2* r.z;
			pos.x += 2;
			break;

		case GLUT_KEY_PAGE_UP:
		    //pos.z += 2 * u.z;
		    pos.z += 2;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    //pos.z -= 2 * u.z;
		    pos.z -= 2;
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
				//drawaxes=1-drawaxes;
				point bulletVec;
				bulletVec.x = lhpoint.x - uhpoint.x;
				bulletVec.y = lhpoint.y - uhpoint.y;
				bulletVec.z = lhpoint.z - uhpoint.z;
				targetPoint.y = -200;

				double t;
				t = (targetPoint.y - uhpoint.y) / bulletVec.y;
				targetPoint.x = uhpoint.x + (t * bulletVec.x);
				targetPoint.z = uhpoint.z + (t * bulletVec.z);
				//printf("%f %f %f\n", targetPoint.x, targetPoint.y, targetPoint.z);

                if(targetPoint.x >= -35 && targetPoint.x <= 35)
                {
                    if(targetPoint.z >= -35 && targetPoint.z <= 35)
                    {
                        gunSpot.push_back(targetPoint);

                    }
                }




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
	//gluLookAt(100,100,100,	0,0,0,	0,1,0);
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects
    //drawCone(20,50,24);
	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    drawGunOffline();
    //drawCircle(30,24);


	//drawSphere(4,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	gunAngZ = 0;
	gunAngX = 0;
	gunBodyZ = 0;
	gunBodyX = 0;
	gunSpin = 0;

	uhpoint.x = 0;
	uhpoint.y = -32;
	uhpoint.z = 0;

	lhpoint.x = 0;
	lhpoint.y = -92;
	lhpoint.z = 0;


    setCameraPosition(20, 50, 10);
    setUvector(0, 0, 1);
    setLvector(0, -1, 0);
    setRvector(-1, 0, 0);
    //setLvector((-1/sqrt(2)), (-1/sqrt(2)), 0);
    //setRvector((-1/sqrt(2)), (1/sqrt(2)), 0);
    //seeDotProduct(u,r);
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

	glutCreateWindow("Gun");

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
