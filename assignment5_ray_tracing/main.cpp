#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include <windows.h>
#include <glut.h>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))
#define epsilon (1.0e-6)

bitmap_image b_img ("texture.bmp");

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double gunAngZ,gunAngX,gunBodyZ, gunBodyX, gunSpin;

int txVar;


double neaR, faR, fov_X, fov_Y, aspectRatio;
int recursionLevel, numPixelAlongAxis;
double checkerBoardCellWidth;
double ambientCoefficientC, diffuseCoefficientC, reflectionCoefficientC;
double gridWidth, gridHeight;


int startRayTracing;

struct point
{
	double x,y,z;
};

point pos, u, r, l;
point pointBuffer[800][800];
double forT[800][800];

struct sphere
{
    point centerSp;
    double rad, colR, colG, colB, amC, difC, refC, specC, shine;
};

struct pyramid
{
    point p00, p10, p11, p01, maxP;
    double colR, colG, colB, amC, difC, refC, specC, shine;
};

struct normalLightSource
{
    point lightPos;
    double faParameter;
};

struct spotLightSource
{
    point lightPos;
    double faParameter;
    point lookAtCoord;
    double coAngle;
};


struct tVal
{
    double tval;
    int type;
    int indexO;
    int whiteBlack; //0 black, 1 white, 2 not checkerboard
    int tNo;
};
tVal ts;
tVal storeT[800][800];


point uhpoint, lhpoint;
point targetPoint;

vector<point> gunSpot;
vector<sphere> allSpheres;
vector<pyramid> allPyramids;
vector<normalLightSource> allNormalLightSources;
vector<spotLightSource> allSpotLightSources;
//vector<vector<point> > dirVec(800, vector<point>(800));
point dirVec[800][800];
double sphereT[800][800];





class color
{
public:
    double r, g, b;
    color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    color()
    {

    }
};

color colorPixel[800][800];

double lengthEdge(point a, point b)
{
    return (sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z -b.z) * (a.z -b.z)));
}

double areaT(point a, point b, point c)
{
    double edgeA = lengthEdge(a, b);
    double edgeB = lengthEdge(b, c);
    double edgeC = lengthEdge(a, c);

    double halfPerimeter = (edgeA + edgeB + edgeC) / 2;
    double areaTot = sqrt(halfPerimeter * (halfPerimeter - edgeA) * (halfPerimeter - edgeB) * (halfPerimeter - edgeC));
    return areaTot;
}


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

point crossPro(point a, point b)
{
    point v;
    v.x = a.y*b.z - a.z*b.y;
    v.y = b.x*a.z - b.z*a.x;
    v.z = a.x*b.y - a.y*b.x;
    return v;
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

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawSphere(double radius,int slices,int stacks)
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
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
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


color **textureBuffer;
int imheight, imwidth;
void loadImageBmp()
{
    imheight = b_img.height();
    imwidth = b_img.width();
    textureBuffer = new color* [imwidth];
    for (int i = 0; i < imwidth; i++) {
        textureBuffer[i] = new color [imheight];
        for (int j = 0; j < imheight; j++) {
            unsigned char r, g, b;
            b_img.get_pixel(i, j, r, g, b);
            color c(r/255.0, g/255.0, b/255.0);
            textureBuffer[i][j] = c;
        }
    }
}

void colorInitialize()
{
    color background(0, 0, 0);
    for(int i=0; i<numPixelAlongAxis; i++)
    {
        for(int j=0; j<numPixelAlongAxis; j++)
        {
            colorPixel[i][j] = background;
        }
    }
}
void inputTaking()
{
    ifstream description;
    ofstream output;
    description.open("description.txt");
    /*output.open ("checkRead.txt");
    output << std::fixed;
    output << std::setprecision(7);*/

    description >> neaR >> faR >> fov_Y >> aspectRatio;
    description >> recursionLevel >> numPixelAlongAxis;
    description >> checkerBoardCellWidth >> ambientCoefficientC >> diffuseCoefficientC >> reflectionCoefficientC;
    /*cout << neaR << " " << faR << " " << fov_Y << " " << aspectRatio << "\n";
    cout << recursionLevel << " " << numPixelAlongAxis << "\n";
    cout << checkerBoardCellWidth << " " << ambientCoefficient << " " << diffuseCoefficient << " " << reflectionCoefficient << "\n";*/

    int totalObjects;
    string command;
    double centX, centY, centZ, radius, colX, colY, colZ, ambientCoefficient, diffuseCoefficient, reflectionCoefficient, specularCoefficient, shininess, width, height;
    double posLx, posLy, posLz, fallOffParameter, lpX, lpY, lpZ, cutOffAngle;

    description >> totalObjects;

    for(int i=0; i<totalObjects; i++)
    {
        description >> command;
        if(command == "sphere")
        {
            description >> centX >> centY >> centZ;
            description >> radius;
            description >> colX >> colY >> colZ;
            description >> ambientCoefficient >> diffuseCoefficient >> specularCoefficient >> reflectionCoefficient;
            description >> shininess;

            //cout << "radius : " << radius << "\n";

            point centerSph;
            centerSph.x = centX;
            centerSph.y = centY;
            centerSph.z = centZ;

            sphere sph;
            sph.centerSp = centerSph;
            sph.colR = colX;
            sph.colG = colY;
            sph.colB = colZ;
            sph.amC = ambientCoefficient;
            sph.difC = diffuseCoefficient;
            sph.refC = reflectionCoefficient;
            sph.shine = shininess;
            sph.rad = radius;
            sph.specC = specularCoefficient;
            allSpheres.push_back(sph);
        }
        if(command == "pyramid")
        {
            description >> centX >> centY >> centZ;
            description >> width >> height;
            description >> colX >> colY >> colZ;
            description >> ambientCoefficient >> diffuseCoefficient >> specularCoefficient >> reflectionCoefficient;
            description >> shininess;

            //cout << centX << " " << centY << " " << centZ << "\n";
            point p5 , p1, p2, p3, p4;
            p1.x = centX;
            p1.y = centY;
            p1.z = centZ;

            p2.x = centX + width;
            p2.y = centY;
            p2.z = centZ;

            p3.x = centX + width;
            p3.y = centY + width;
            p3.z = centZ;

            p4.x = centX;
            p4.y = centY + width;
            p4.z = centZ;

            p5.x = (p1.x + p3.x) / 2;
            p5.y = (p1.y + p3.y) / 2;
            p5.z = ((p1.z + p3.z) / 2) + height;

            pyramid pyr;
            pyr.p00 = p1;
            pyr.p10 = p2;
            pyr.p11 = p3;
            pyr.p01 = p4;
            pyr.maxP = p5;
            pyr.amC = ambientCoefficient;
            pyr.difC = diffuseCoefficient;
            pyr.refC = reflectionCoefficient;
            pyr.shine = shininess;
            pyr.specC = specularCoefficient;
            pyr.colR = colX;
            pyr.colG = colY;
            pyr.colB = colZ;
            allPyramids.push_back(pyr);
        }
    }
    int numLightSources, numSpotLights;

    description >> numLightSources;
    for(int i=0; i<numLightSources; i++)
    {
        description >> posLx >> posLy >> posLz;
        description >> fallOffParameter;

        normalLightSource nls;
        nls.faParameter = fallOffParameter;
        point lsPos;
        lsPos.x = posLx;
        lsPos.y = posLy;
        lsPos.z = posLz;
        nls.lightPos = lsPos;
        allNormalLightSources.push_back(nls);
    }

    description >> numSpotLights;
    for(int i=0; i<numSpotLights; i++)
    {
        description >> posLx >> posLy >> posLz;
        description >> fallOffParameter;
        description >> lpX >> lpY >> lpZ;
        description >> cutOffAngle;

        point slPos, slLookAt;
        slPos.x = posLx;
        slPos.y = posLy;
        slPos.z = posLz;

        slLookAt.x = lpX;
        slLookAt.y = lpY;
        slLookAt.z = lpZ;

        spotLightSource sls;
        sls.lightPos = slPos;
        sls.lookAtCoord = slLookAt;
        sls.faParameter = fallOffParameter;
        sls.coAngle = cutOffAngle;
        allSpotLightSources.push_back(sls);
    }

    description.close();
    //output.close();
}

void rtOffline()
{
    for(int i=-100; i<100; i++)
    {
        for (int j=-100; j<100; j++)
        {
            if(((i + j) % 2) == 0)
            {
                glColor3f( 0, 0, 0);
            }
            else
            {
                glColor3f( 1, 1, 1);
            }
           // glRecti(i * checkerBoardCellWidth, j * checkerBoardCellWidth, (i + 1) * checkerBoardCellWidth, (j + 1) * checkerBoardCellWidth);
            //drawSquare(checkerBoardCellWidth);
            if(txVar == 0){
            glBegin(GL_QUADS);
            {
                glVertex3f(i * checkerBoardCellWidth, j * checkerBoardCellWidth, 0);
                glVertex3f(i * checkerBoardCellWidth, j * checkerBoardCellWidth + checkerBoardCellWidth, 0);
                glVertex3f(i * checkerBoardCellWidth + checkerBoardCellWidth, j * checkerBoardCellWidth+checkerBoardCellWidth, 0);
                glVertex3f(i * checkerBoardCellWidth+checkerBoardCellWidth, j * checkerBoardCellWidth , 0);
            }
            glEnd();
            }
        }
    }
    for(int i=0; i<allSpheres.size(); i++)
    {
        glPushMatrix();
        {
            glTranslatef(allSpheres[i].centerSp.x, allSpheres[i].centerSp.y, allSpheres[i].centerSp.z);
            glColor3f(allSpheres[i].colR, allSpheres[i].colG, allSpheres[i].colB);
            drawSphere(allSpheres[i].rad, 50, 70);
        }
        glPopMatrix();
    }
    for(int i=0; i<allPyramids.size(); i++)
    {
        glColor3f(allPyramids[i].colR, allPyramids[i].colG, allPyramids[i].colB);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(allPyramids[i].p00.x, allPyramids[i].p00.y, allPyramids[i].p00.z);
            glVertex3f(allPyramids[i].p10.x, allPyramids[i].p10.y, allPyramids[i].p10.z);
            glVertex3f(allPyramids[i].maxP.x, allPyramids[i].maxP.y, allPyramids[i].maxP.z);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(allPyramids[i].p10.x, allPyramids[i].p10.y, allPyramids[i].p10.z);
            glVertex3f(allPyramids[i].p11.x, allPyramids[i].p11.y, allPyramids[i].p11.z);
            glVertex3f(allPyramids[i].maxP.x, allPyramids[i].maxP.y, allPyramids[i].maxP.z);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(allPyramids[i].p11.x, allPyramids[i].p11.y, allPyramids[i].p11.z);
            glVertex3f(allPyramids[i].p01.x, allPyramids[i].p01.y, allPyramids[i].p01.z);
            glVertex3f(allPyramids[i].maxP.x, allPyramids[i].maxP.y, allPyramids[i].maxP.z);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(allPyramids[i].p00.x, allPyramids[i].p00.y, allPyramids[i].p00.z);
            glVertex3f(allPyramids[i].p01.x, allPyramids[i].p01.y, allPyramids[i].p01.z);
            glVertex3f(allPyramids[i].maxP.x, allPyramids[i].maxP.y, allPyramids[i].maxP.z);
        }
        glEnd();
    }
    for(int i=0; i<allNormalLightSources.size(); i++)
    {
        glPushMatrix();
        {
            glTranslatef(allNormalLightSources[i].lightPos.x, allNormalLightSources[i].lightPos.y, allNormalLightSources[i].lightPos.z);
            glColor3f(1, 1, 1);
            drawSphere(7, 28, 20);
        }
        glPopMatrix();
    }
    for(int i=0; i<allSpotLightSources.size(); i++)
    {
        glPushMatrix();
        {
            glTranslatef(allSpotLightSources[i].lightPos.x, allSpotLightSources[i].lightPos.y, allSpotLightSources[i].lightPos.z);
            glColor3f(1, 1, 1);
            drawSphere(7, 28, 20);
        }
        glPopMatrix();
    }

        /*glPushMatrix();
        {
            glTranslatef(pointBuffer[0][0].x, pointBuffer[0][0].y, pointBuffer[0][0].z);
            glColor3f(0, 1, 0);
            drawSphere(1, 28, 20);
        }
        glPopMatrix();
        glPushMatrix();
        {
            glTranslatef(pointBuffer[0][499].x, pointBuffer[0][499].y, pointBuffer[0][499].z);
            glColor3f(0, 1, 0);
            drawSphere(1, 28, 20);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(pointBuffer[499][0].x, pointBuffer[499][0].y, pointBuffer[499][0].z);
            glColor3f(1, 0, 0);
            drawSphere(1, 28, 20);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(pointBuffer[499][499].x, pointBuffer[499][499].y, pointBuffer[499][499].z);
            glColor3f(1, 1, 0);
            drawSphere(1, 28, 20);
        }
        glPopMatrix();*/
}

void rayTracing(point a, point q)
{
    double t, t1, t2;
    //double bigNum = 1000;
    ts.tval = 1000;
    for(int k=0; k<allSpheres.size(); k++)
    {
        double pointX, pointY, pointZ;
        pointX = a.x - allSpheres[k].centerSp.x;
        pointY = a.y - allSpheres[k].centerSp.y;
        pointZ = a.z - allSpheres[k].centerSp.z;

        //double b = 2 * ((dirVec[i][j].x * pointBuffer[i][j].x) + (dirVec[i][j].y * pointBuffer[i][j].y) + (dirVec[i][j].z * pointBuffer[i][j].z));
        //double c = ((pointBuffer[i][j].x * pointBuffer[i][j].x) + (pointBuffer[i][j].y * pointBuffer[i][j].y) + (pointBuffer[i][j].z * pointBuffer[i][j].z)) - ((allSpheres[k].rad)*(allSpheres[k].rad));
        double b = 2 * ((q.x * pointX) + (q.y * pointY) + (q.z * pointZ));
        double c = ((pointX * pointX) + (pointY * pointY) + (pointZ * pointZ)) - ((allSpheres[k].rad)*(allSpheres[k].rad));
        double d = sqrt(b * b - 4 * c);
        if(d >= 0)
        {
            t = 1000;
            t1 = (-b + d) / 2;
            t2 = (-b - d) / 2;
            if(t1 < t2 && t1 > 0)
            {
                t = t1;
            }
            else if(t2 < t1 && t2 > 0)
            {
                t = t2;
            }
//            if(forT[i][j] > t)
//            {
//                //cout << k << " number sphere\n";
//                forT[i][j] = t;
//                colorPixel[i][j].r = allSpheres[k].colR * 255;
//                colorPixel[i][j].g = allSpheres[k].colG * 255;
//                colorPixel[i][j].b = allSpheres[k].colB * 255;
//            }
            if(t < ts.tval && t > 0){
            ts.tval = t;
            ts.type = 0;
            ts.indexO = k;
            ts.tNo = 100;
            ts.whiteBlack = 2;}
        }
    }
    for(int k=0; k<allPyramids.size(); k++)
    {
        point upor, bam, dan;
        point pointNormal, one, two, triangleIntersect;
        double var, areaToatal, area1, area2, area3, areaDiff, normVec;




        bam = allPyramids[k].p00;
        dan = allPyramids[k].p10;
        upor = allPyramids[k].maxP;

        one.x = dan.x - bam.x;
        one.y = dan.y - bam.y;
        one.z = dan.z - bam.z;

        two.x = upor.x - bam.x;
        two.y = upor.y - bam.y;
        two.z = upor.z - bam.z;
        pointNormal = crossPro(one, two);
        normVec  = sqrt((pointNormal.x * pointNormal.x) + (pointNormal.y * pointNormal.y) + (pointNormal.z * pointNormal.z));
        pointNormal.x /= normVec;
        pointNormal.y /= normVec;
        pointNormal.z /= normVec;



        var = (-1) * ((pointNormal.x * upor.x) + (pointNormal.y * upor.y) + (pointNormal.z * upor.z));
        t = ((-1) * (((pointNormal.x * a.x) + (pointNormal.y * a.y) + (pointNormal.z * a.z)) + var)) / ((pointNormal.x * q.x) + (pointNormal.y * q.y) + (pointNormal.z * q.z));
        //cout << t << "\n";

        triangleIntersect.x = a.x + t * q.x;
        triangleIntersect.y = a.y + t * q.y;
        triangleIntersect.z = a.z + t * q.z;

        areaToatal = areaT(upor, bam, dan);
        area1 = areaT(triangleIntersect, upor, bam);
        area2 = areaT(triangleIntersect, bam, dan);
        area3 = areaT(triangleIntersect, upor, dan);

        areaDiff = abs(areaToatal - (area1 + area2 + area3));

        if(areaDiff < 1)
        {
//            if(forT[i][j] > t)
//            {
//                //cout << "inside1\n";
//                forT[i][j] = t;
//                colorPixel[i][j].r = allPyramids[k].colR * 255;
//                colorPixel[i][j].g = allPyramids[k].colG * 255;
//                colorPixel[i][j].b = allPyramids[k].colB * 255;
//            }
            if(t < ts.tval && t > 0){
            ts.tval = t;
            ts.type = 1;
            ts.indexO = k;
            ts.tNo = 1;
            ts.whiteBlack = 2;}
        }







        bam = allPyramids[k].p10;
        dan = allPyramids[k].p11;
        upor = allPyramids[k].maxP;

        one.x = dan.x - bam.x;
        one.y = dan.y - bam.y;
        one.z = dan.z - bam.z;

        two.x = upor.x - bam.x;
        two.y = upor.y - bam.y;
        two.z = upor.z - bam.z;
        pointNormal = crossPro(one, two);
        normVec  = sqrt((pointNormal.x * pointNormal.x) + (pointNormal.y * pointNormal.y) + (pointNormal.z * pointNormal.z));
        pointNormal.x /= normVec;
        pointNormal.y /= normVec;
        pointNormal.z /= normVec;

        var = (-1) * ((pointNormal.x * upor.x) + (pointNormal.y * upor.y) + (pointNormal.z * upor.z));
        t = ((-1) * (((pointNormal.x * a.x) + (pointNormal.y * a.y) + (pointNormal.z * a.z)) + var)) / ((pointNormal.x * q.x) + (pointNormal.y * q.y) + (pointNormal.z * q.z));
        //cout << t << "\n";

        triangleIntersect.x = a.x + t * q.x;
        triangleIntersect.y = a.y + t * q.y;
        triangleIntersect.z = a.z + t * q.z;

        areaToatal = areaT(upor, bam, dan);
        area1 = areaT(triangleIntersect, upor, bam);
        area2 = areaT(triangleIntersect, bam, dan);
        area3 = areaT(triangleIntersect, upor, dan);

        areaDiff = abs(areaToatal - (area1 + area2 + area3));

        if(areaDiff < 1)
        {
//            if(forT[i][j] > t)
//            {
//                forT[i][j] = t;
//                colorPixel[i][j].r = allPyramids[k].colR * 255;
//                colorPixel[i][j].g = allPyramids[k].colG * 255;
//                colorPixel[i][j].b = allPyramids[k].colB * 255;
//                //cout << "inside2\n";
//            }
            if(t < ts.tval && t > 0){
            ts.tval = t;
            ts.type = 1;
            ts.indexO = k;
            ts.tNo = 2;
            ts.whiteBlack = 2;}
        }





        //cout << "aaaaa\n";

        bam = allPyramids[k].p11;
        dan = allPyramids[k].p01;
        upor = allPyramids[k].maxP;

        one.x = dan.x - bam.x;
        one.y = dan.y - bam.y;
        one.z = dan.z - bam.z;

        two.x = upor.x - bam.x;
        two.y = upor.y - bam.y;
        two.z = upor.z - bam.z;
        pointNormal = crossPro(one, two);
        normVec  = sqrt((pointNormal.x * pointNormal.x) + (pointNormal.y * pointNormal.y) + (pointNormal.z * pointNormal.z));
        pointNormal.x /= normVec;
        pointNormal.y /= normVec;
        pointNormal.z /= normVec;

        var = (-1) * ((pointNormal.x * upor.x) + (pointNormal.y * upor.y) + (pointNormal.z * upor.z));
        t = ((-1) * (((pointNormal.x * a.x) + (pointNormal.y * a.y) + (pointNormal.z * a.z)) + var)) / ((pointNormal.x * q.x) + (pointNormal.y * q.y) + (pointNormal.z * q.z));
        //cout << t << "\n";

        triangleIntersect.x = a.x + t * q.x;
        triangleIntersect.y = a.y + t * q.y;
        triangleIntersect.z = a.z + t * q.z;

        areaToatal = areaT(upor, bam, dan);
        area1 = areaT(triangleIntersect, upor, bam);
        area2 = areaT(triangleIntersect, bam, dan);
        area3 = areaT(triangleIntersect, upor, dan);

        areaDiff = abs(areaToatal - (area1 + area2 + area3));

        //if((areaDiff >= 0 && areaDiff <= 1) && (areaDiff <= 0 && areaDiff >= -1))
        if(areaDiff < 1)
        {
//            if(forT[i][j] > t)
//            {
//                //cout << "inside3\n";
//                forT[i][j] = t;
//                colorPixel[i][j].r = allPyramids[k].colR * 255;
//                colorPixel[i][j].g = allPyramids[k].colG * 255;
//                colorPixel[i][j].b = allPyramids[k].colB * 255;
//            }
            if(t < ts.tval && t > 0){
            ts.tval = t;
            ts.type = 1;
            ts.indexO = k;
            ts.tNo = 3;
            ts.whiteBlack = 2;}

        }








        bam = allPyramids[k].p01;
        dan = allPyramids[k].p00;
        upor = allPyramids[k].maxP;

        one.x = dan.x - bam.x;
        one.y = dan.y - bam.y;
        one.z = dan.z - bam.z;

        two.x = upor.x - bam.x;
        two.y = upor.y - bam.y;
        two.z = upor.z - bam.z;
        pointNormal = crossPro(one, two);
        normVec  = sqrt((pointNormal.x * pointNormal.x) + (pointNormal.y * pointNormal.y) + (pointNormal.z * pointNormal.z));
        pointNormal.x /= normVec;
        pointNormal.y /= normVec;
        pointNormal.z /= normVec;

        var = (-1) * ((pointNormal.x * upor.x) + (pointNormal.y * upor.y) + (pointNormal.z * upor.z));
        t = ((-1) * (((pointNormal.x * a.x) + (pointNormal.y * a.y) + (pointNormal.z * a.z)) + var)) / ((pointNormal.x * q.x) + (pointNormal.y * q.y) + (pointNormal.z * q.z));
        //cout << t << "\n";

        triangleIntersect.x = a.x + t * q.x;
        triangleIntersect.y = a.y + t * q.y;
        triangleIntersect.z = a.z + t * q.z;

        areaToatal = areaT(upor, bam, dan);
        area1 = areaT(triangleIntersect, upor, bam);
        area2 = areaT(triangleIntersect, bam, dan);
        area3 = areaT(triangleIntersect, upor, dan);

        areaDiff = abs(areaToatal - (area1 + area2 + area3));

        if(areaDiff < 1)
        {
//            if(forT[i][j] > t && t > 0)
//            {
//                //cout << "inside4\n";
//                forT[i][j] = t;
//                colorPixel[i][j].r = allPyramids[k].colR * 255;
//                colorPixel[i][j].g = allPyramids[k].colG * 255;
//                colorPixel[i][j].b = allPyramids[k].colB * 255;
//            }
            if(t < ts.tval && t > 0){
            ts.tval = t;
            ts.type = 1;
            ts.indexO = k;
            ts.tNo = 4;
            ts.whiteBlack = 2;}
        }



        t = (-1) * a.z / q.z;

//        if(forT[i][j] > t && t > 0)
//        {
            //forT[i][j] = t;

            double cbX, cbY;
            cbX = a.x  + t * q.x;
            cbY = a.y  + t * q.y;

            int iInd = ceil((cbX / checkerBoardCellWidth));
            int jInd = ceil((cbY / checkerBoardCellWidth));

            if(((iInd+ jInd) % 2) == 0)
            {
//                colorPixel[i][j].r = 0;
//                colorPixel[i][j].g = 0;
//                colorPixel[i][j].b = 0;
                if(t < ts.tval && t > 0){
                ts.tval = t;
                ts.type = 2;
                ts.indexO = 100;
                ts.tNo = 100;
                ts.whiteBlack = 0;}
            }
            else
            {
//                colorPixel[i][j].r = 255;
//                colorPixel[i][j].g = 255;
//                colorPixel[i][j].b = 255;
                if(t < ts.tval && t > 0){
                ts.tval = t;
                ts.type = 2;
                ts.indexO = 100;
                ts.tNo = 100;
                ts.whiteBlack = 1;}
            }
        //}
    }
}
bool illuminateOrNot(point a, point b, double dista)
{
    a.x += b.x * epsilon;
    a.y += b.y * epsilon;
    a.z += b.z * epsilon;

    rayTracing(a, b);
    if(ts.tval < dista)
    {
        return false;
    }
    else
    {
        return true;
    }
}
void rayTracingFunction()
{
    fov_X = fov_Y * aspectRatio;
    gridHeight = 2 * neaR * tan((fov_Y / 2) * (pi / 180));
    gridWidth = 2 * neaR * tan((fov_X / 2) * (pi / 180));

    //cout << gridHeight << " " << gridWidth << "\n";

    point testl, testr, testu;
    testl.x = l.x;
    testl.y = l.y;
    testl.z = l.z;

    testr.x = r.x;
    testr.y = r.y;
    testr.z = r.z;

    testu.x = u.x;
    testu.y = u.y;
    testu.z = u.z;

    point gridMid;
    gridMid.x = pos.x + neaR * testl.x;
    gridMid.y = pos.y + neaR * testl.y;
    gridMid.z = pos.z + neaR * testl.z;

    point cornerCell;
    cornerCell.x = (gridMid.x + gridHeight * u.x) + (r.x * gridWidth);
    cornerCell.y = (gridMid.y + gridHeight * u.y) + (r.y * gridWidth);
    cornerCell.z = (gridMid.z + gridHeight * u.z) + (r.z * gridWidth);

    pointBuffer[0][0] = cornerCell;

    for(int i=0; i<numPixelAlongAxis; i++)
    {
        for(int j=0; j<numPixelAlongAxis; j++)
        {
            if(j != 0)
            {
                pointBuffer[i][j].x = pointBuffer[i][j-1].x - ((2 * gridWidth / numPixelAlongAxis) * testr.x);
                pointBuffer[i][j].y = pointBuffer[i][j-1].y - ((2 * gridWidth / numPixelAlongAxis) * testr.y);
                pointBuffer[i][j].z = pointBuffer[i][j-1].z - ((2 * gridWidth / numPixelAlongAxis) * testr.z);
            }
        }
        if(i != (numPixelAlongAxis - 1))
        {
            pointBuffer[i+1][0].x = pointBuffer[i][0].x - ((2 * gridHeight / numPixelAlongAxis) * testu.x);
            pointBuffer[i+1][0].y = pointBuffer[i][0].y - ((2 * gridHeight / numPixelAlongAxis) * testu.y);
            pointBuffer[i+1][0].z = pointBuffer[i][0].z - ((2 * gridHeight / numPixelAlongAxis) * testu.z);
        }
    }
    /*for(int i=0; i<numPixelAlongAxis; i++)
    {
        for(int j=0; j<numPixelAlongAxis; j++)
        {
            cout << pointBuffer[i][j].x << " " << pointBuffer[i][j].y << " " << pointBuffer[i][j].z << " ";
        }
        cout << "\n";
    }*/
    //startRayTracing = 1 - startRayTracing;
    //cout << pointBuffer[0][0].x << " " << pointBuffer[0][0].y << " " << pointBuffer[0][0].z << "\n";
    //cout << pointBuffer[0][500].x << " " << pointBuffer[0][500].y << " " << pointBuffer[0][500].z << "\n";

    for(int i=0; i<numPixelAlongAxis; i++)
    {
        for(int j=0; j<numPixelAlongAxis; j++)
        {
            point dVec;
            dVec.x = pointBuffer[i][j].x - pos.x;
            dVec.y = pointBuffer[i][j].y - pos.y;
            dVec.z = pointBuffer[i][j].z - pos.z;

            double normalize = sqrt((dVec.x * dVec.x) + (dVec.y * dVec.y) + (dVec.z * dVec.z));

            dVec.x /= normalize;
            dVec.y /= normalize;
            dVec.z /= normalize;

            dirVec[i][j] = dVec;
        }
    }


    for(int i=0; i<numPixelAlongAxis; i++)
    {
        for(int j=0; j<numPixelAlongAxis; j++)
        {
            double lambert = 0, phong = 0;
            for(int k=0; k<allNormalLightSources.size(); k++)
            {
                point lpCoord = allNormalLightSources[k].lightPos;


                rayTracing(pointBuffer[i][j], dirVec[i][j]);
                if(ts.tval < forT[i][j] && ts.tval > 0)
                {
                    forT[i][j] = ts.tval;
                    storeT[i][j] = ts;
                    //cout << ts.type << "\n";








                double shiny, spcl, amb, diffu;
                point srcInt;
                point isPoint;
                isPoint.x =  pointBuffer[i][j].x + storeT[i][j].tval * dirVec[i][j].x;
                isPoint.y =  pointBuffer[i][j].y + storeT[i][j].tval * dirVec[i][j].y;
                isPoint.z =  pointBuffer[i][j].z + storeT[i][j].tval * dirVec[i][j].z;

                srcInt.x = lpCoord.x - isPoint.x;
                srcInt.y = lpCoord.y - isPoint.y;
                srcInt.z = lpCoord.z - isPoint.z;

                double normPS;
                normPS = sqrt((srcInt.x * srcInt.x) + (srcInt.y * srcInt.y) + (srcInt.z * srcInt.z));

                srcInt.x /= normPS;
                srcInt.y /= normPS;
                srcInt.z /= normPS;

                point normaL;

                if(storeT[i][j].type == 0)
                {
                    normaL.x = isPoint.x - allSpheres[storeT[i][j].indexO].centerSp.x;
                    normaL.y = isPoint.y - allSpheres[storeT[i][j].indexO].centerSp.y;
                    normaL.z = isPoint.z - allSpheres[storeT[i][j].indexO].centerSp.z;
                    shiny = allSpheres[storeT[i][j].indexO].shine;
                    spcl = allSpheres[storeT[i][j].indexO].specC;
                    amb = allSpheres[storeT[i][j].indexO].amC;
                    diffu = allSpheres[storeT[i][j].indexO].difC;

                    colorPixel[i][j].r += allSpheres[ts.indexO].colR * amb;
                    colorPixel[i][j].g += allSpheres[ts.indexO].colG * amb;
                    colorPixel[i][j].b += allSpheres[ts.indexO].colB * amb;
                }
                else if(storeT[i][j].type == 2)
                {
                    normaL.x = 0;
                    normaL.y = 0;
                    normaL.z = 1;
                    shiny = 0;
                    spcl = 0;
                    amb = ambientCoefficientC;
                    diffu = diffuseCoefficientC;

                    point primE;
                    primE.x = isPoint.x + 100 * checkerBoardCellWidth;
                    primE.y = isPoint.y + 100 * checkerBoardCellWidth;
                    primE.z = 0;


                    primE.x -= checkerBoardCellWidth * (int)(primE.x / checkerBoardCellWidth);
                    primE.y -= checkerBoardCellWidth * (int)(primE.y / checkerBoardCellWidth);
                    primE.z = 0;

                    int ind1, ind2;
                    ind1 = (int)(imwidth * primE.x / checkerBoardCellWidth);
                    ind2 = (int)(imwidth * primE.y / checkerBoardCellWidth);

                    double rC, gC, bC;
                    rC = textureBuffer[ind1][ind2].r;
                    gC = textureBuffer[ind1][ind2].g;
                    bC = textureBuffer[ind1][ind2].b;

                    if(txVar == 1){
                    colorPixel[i][j].r += rC * amb;
                    colorPixel[i][j].g += gC * amb;
                    colorPixel[i][j].b += bC * amb;
                    }


                    else{
                    if(storeT[i][j].whiteBlack == 1)
                    {
                        colorPixel[i][j].r += 1 * amb;
                        colorPixel[i][j].g += 1 * amb;
                        colorPixel[i][j].b += 1 * amb;
                    }
                    else
                    {
                        colorPixel[i][j].r = 0;
                        colorPixel[i][j].g = 0;
                        colorPixel[i][j].b = 0;
                    }
                    }
                }
                else if(storeT[i][j].type == 1)
                {
                    shiny = allPyramids[storeT[i][j].indexO].shine;
                    spcl = allPyramids[storeT[i][j].indexO].specC;
                    amb = allPyramids[storeT[i][j].indexO].amC;
                    diffu = allPyramids[storeT[i][j].indexO].difC;

                    colorPixel[i][j].r += allPyramids[ts.indexO].colR * amb;
                    colorPixel[i][j].g += allPyramids[ts.indexO].colG * amb;
                    colorPixel[i][j].b += allPyramids[ts.indexO].colB * amb;

                    if(storeT[i][j].tNo == 1)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p00.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p00.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p00.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p10.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p10.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p10.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;

                        normaL = crossPro(vect1, vect2);
                    }
                    else if(storeT[i][j].tNo == 2)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p10.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p10.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p10.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p11.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p11.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p11.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;;

                        normaL = crossPro(vect1, vect2);
                    }

                    else if(storeT[i][j].tNo == 3)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p11.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p11.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p11.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p01.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p01.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p01.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;

                        normaL = crossPro(vect1, vect2);
                    }
                    else if(storeT[i][j].tNo == 4)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p01.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p01.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p01.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p00.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p00.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p00.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;

                        normaL = crossPro(vect1, vect2);
                    }
                }
                double distPS;
                distPS = sqrt(((isPoint.x - lpCoord.x) * (isPoint.x - lpCoord.x)) + ((isPoint.y - lpCoord.y) * (isPoint.y - lpCoord.y)) + ((isPoint.z - lpCoord.z) * (isPoint.z - lpCoord.z)));
                double scalingFactor = exp((-1) * distPS * distPS * allNormalLightSources[k].faParameter);

                if(illuminateOrNot(isPoint, srcInt, distPS) == true){
                double dotProNorm = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z));
                if(dotProNorm > 0){
                lambert += (dotProNorm * scalingFactor);}

                point reflectP;
                reflectP.x = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z)) * 2 * normaL.x - srcInt.x;
                reflectP.y = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z)) * 2 * normaL.y - srcInt.y;
                reflectP.z = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z)) * 2 * normaL.z - srcInt.z;

                double normRP;
                normRP = sqrt((reflectP.x * reflectP.x) + (reflectP.y * reflectP.y) + (reflectP.z * reflectP.z));

                reflectP.x /= normRP;
                reflectP.y /= normRP;
                reflectP.z /= normRP;

                double dotRprimeV = (reflectP.x * dirVec[i][j].x) + (reflectP.y * dirVec[i][j].y) + (reflectP.z * dirVec[i][j].z);

                phong += pow(dotRprimeV, shiny) * scalingFactor;

                colorPixel[i][j].r += diffu * lambert * colorPixel[i][j].r / amb + spcl * phong * colorPixel[i][j].r / amb;
                colorPixel[i][j].g += diffu * lambert * colorPixel[i][j].g / amb + spcl * phong * colorPixel[i][j].g / amb;
                colorPixel[i][j].b += diffu * lambert * colorPixel[i][j].b / amb + spcl * phong * colorPixel[i][j].b / amb;

//                colorPixel[i][j].r = amb * colorPixel[i][j].r + diffu * lambert * colorPixel[i][j].r + spcl * phong * colorPixel[i][j].r;
//                colorPixel[i][j].g = amb * colorPixel[i][j].g + diffu * lambert * colorPixel[i][j].g + spcl * phong * colorPixel[i][j].g;
//                colorPixel[i][j].b = amb * colorPixel[i][j].b + diffu * lambert * colorPixel[i][j].b + spcl * phong * colorPixel[i][j].b;

                //cout << spcl * phong * colorPixel[j][k].r;

                if(colorPixel[i][j].r > 1)
                {
                    colorPixel[i][j].r = 1;
                }
                if(colorPixel[i][j].g > 1)
                {
                    colorPixel[i][j].g = 1;
                }
                if(colorPixel[i][j].b > 1)
                {
                    colorPixel[i][j].b = 1;
                }
                }
                }
                }













            for(int k=0; k<allSpotLightSources.size(); k++)
            {
                point lpCoord = allSpotLightSources[k].lightPos;


                rayTracing(pointBuffer[i][j], dirVec[i][j]);
                if(ts.tval < forT[i][j] && ts.tval > 0)
                {
                    forT[i][j] = ts.tval;
                    storeT[i][j] = ts;
                    //cout << ts.type << "\n";








                double shiny, spcl, amb, diffu;
                point srcInt;
                point isPoint;
                isPoint.x =  pointBuffer[i][j].x + storeT[i][j].tval * dirVec[i][j].x;
                isPoint.y =  pointBuffer[i][j].y + storeT[i][j].tval * dirVec[i][j].y;
                isPoint.z =  pointBuffer[i][j].z + storeT[i][j].tval * dirVec[i][j].z;

                srcInt.x = lpCoord.x - isPoint.x;
                srcInt.y = lpCoord.y - isPoint.y;
                srcInt.z = lpCoord.z - isPoint.z;

                double normPS;
                normPS = sqrt((srcInt.x * srcInt.x) + (srcInt.y * srcInt.y) + (srcInt.z * srcInt.z));

                srcInt.x /= normPS;
                srcInt.y /= normPS;
                srcInt.z /= normPS;

                point normaL;

                if(storeT[i][j].type == 0)
                {
                    normaL.x = isPoint.x - allSpheres[storeT[i][j].indexO].centerSp.x;
                    normaL.y = isPoint.y - allSpheres[storeT[i][j].indexO].centerSp.y;
                    normaL.z = isPoint.z - allSpheres[storeT[i][j].indexO].centerSp.z;
                    shiny = allSpheres[storeT[i][j].indexO].shine;
                    spcl = allSpheres[storeT[i][j].indexO].specC;
                    amb = allSpheres[storeT[i][j].indexO].amC;
                    diffu = allSpheres[storeT[i][j].indexO].difC;

                    colorPixel[i][j].r += allSpheres[ts.indexO].colR * amb;
                    colorPixel[i][j].g += allSpheres[ts.indexO].colG * amb;
                    colorPixel[i][j].b += allSpheres[ts.indexO].colB * amb;
                }
                else if(storeT[i][j].type == 2)
                {
                    normaL.x = 0;
                    normaL.y = 0;
                    normaL.z = 1;
                    shiny = 0;
                    spcl = 0;
                    amb = ambientCoefficientC;
                    diffu = diffuseCoefficientC;

                    point primE;
                    primE.x = isPoint.x + 100 * checkerBoardCellWidth;
                    primE.y = isPoint.y + 100 * checkerBoardCellWidth;
                    primE.z = 0;


                    primE.x -= checkerBoardCellWidth * (int)(primE.x / checkerBoardCellWidth);
                    primE.y -= checkerBoardCellWidth * (int)(primE.y / checkerBoardCellWidth);
                    primE.z = 0;

                    int ind1, ind2;
                    ind1 = (int)(imwidth * primE.x / checkerBoardCellWidth);
                    ind2 = (int)(imwidth * primE.y / checkerBoardCellWidth);

                    double rC, gC, bC;
                    rC = textureBuffer[ind1][ind2].r;
                    gC = textureBuffer[ind1][ind2].g;
                    bC = textureBuffer[ind1][ind2].b;
                    if(txVar == 1){
                    colorPixel[i][j].r += rC * amb;
                    colorPixel[i][j].g += gC * amb;
                    colorPixel[i][j].b += bC * amb;}



                    else{
                    if(storeT[i][j].whiteBlack == 1)
                    {
                        colorPixel[i][j].r += 1 * amb;
                        colorPixel[i][j].g += 1 * amb;
                        colorPixel[i][j].b += 1 * amb;
                    }
                    else
                    {
                        colorPixel[i][j].r = 0;
                        colorPixel[i][j].g = 0;
                        colorPixel[i][j].b = 0;
                    }
                    }
                }
                else if(storeT[i][j].type == 1)
                {
                    shiny = allPyramids[storeT[i][j].indexO].shine;
                    spcl = allPyramids[storeT[i][j].indexO].specC;
                    amb = allPyramids[storeT[i][j].indexO].amC;
                    diffu = allPyramids[storeT[i][j].indexO].difC;

                    colorPixel[i][j].r += allPyramids[ts.indexO].colR * amb;
                    colorPixel[i][j].g += allPyramids[ts.indexO].colG * amb;
                    colorPixel[i][j].b += allPyramids[ts.indexO].colB * amb;

                    if(storeT[i][j].tNo == 1)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p00.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p00.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p00.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p10.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p10.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p10.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;

                        normaL = crossPro(vect1, vect2);
                    }
                    else if(storeT[i][j].tNo == 2)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p10.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p10.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p10.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p11.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p11.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p11.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;;

                        normaL = crossPro(vect1, vect2);
                    }

                    else if(storeT[i][j].tNo == 3)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p11.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p11.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p11.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p01.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p01.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p01.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;

                        normaL = crossPro(vect1, vect2);
                    }
                    else if(storeT[i][j].tNo == 4)
                    {
                        point upr, bm, dn;
                        point vect1, vect2;
                        upr.x = allPyramids[storeT[i][j].indexO].maxP.x;
                        upr.y = allPyramids[storeT[i][j].indexO].maxP.y;
                        upr.z = allPyramids[storeT[i][j].indexO].maxP.z;

                        bm.x = allPyramids[storeT[i][j].indexO].p01.x;
                        bm.y = allPyramids[storeT[i][j].indexO].p01.y;
                        bm.z = allPyramids[storeT[i][j].indexO].p01.z;

                        dn.x = allPyramids[storeT[i][j].indexO].p00.x;
                        dn.y = allPyramids[storeT[i][j].indexO].p00.y;
                        dn.z = allPyramids[storeT[i][j].indexO].p00.z;

                        vect1.x = dn.x - bm.x;
                        vect1.y = dn.y - bm.y;
                        vect1.z = dn.z - bm.z;

                        vect2.x = upr.x - bm.x;
                        vect2.y = upr.y - bm.y;
                        vect2.z = upr.z - bm.z;

                        normaL = crossPro(vect1, vect2);
                    }
                }
                double distPS;
                distPS = sqrt(((isPoint.x - lpCoord.x) * (isPoint.x - lpCoord.x)) + ((isPoint.y - lpCoord.y) * (isPoint.y - lpCoord.y)) + ((isPoint.z - lpCoord.z) * (isPoint.z - lpCoord.z)));
                double scalingFactor = exp((-1) * distPS * distPS * allSpotLightSources[k].faParameter);



                double dotProv1v2;
                point v2 = allSpotLightSources[k].lookAtCoord;
                v2.x = lpCoord.x - v2.x;
                v2.y = lpCoord.y - v2.y;
                v2.z = lpCoord.z - v2.z;
                double normv2 = sqrt((v2.x * v2.x) + (v2.y * v2.y) + (v2.z * v2.z));
                v2.x /= normv2;
                v2.y /= normv2;
                v2.z /= normv2;

                dotProv1v2 = (srcInt.x * v2.x) + (srcInt.y * v2.y) + (srcInt.z * v2.z);
                double angl = acos(dotProv1v2) * 180 / pi;



                if(illuminateOrNot(isPoint, srcInt, distPS) == true && angl > allSpotLightSources[k].coAngle){
                //cout << "aaaaaa\n";
                double dotProNorm = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z));
                if(dotProNorm > 0){
                lambert += (dotProNorm * scalingFactor);}

                point reflectP;
                reflectP.x = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z)) * 2 * normaL.x - srcInt.x;
                reflectP.y = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z)) * 2 * normaL.y - srcInt.y;
                reflectP.z = ((srcInt.x * normaL.x) + (srcInt.y * normaL.y) + (srcInt.z * normaL.z)) * 2 * normaL.z - srcInt.z;

                double normRP;
                normRP = sqrt((reflectP.x * reflectP.x) + (reflectP.y * reflectP.y) + (reflectP.z * reflectP.z));

                reflectP.x /= normRP;
                reflectP.y /= normRP;
                reflectP.z /= normRP;

                double dotRprimeV = (reflectP.x * dirVec[i][j].x) + (reflectP.y * dirVec[i][j].y) + (reflectP.z * dirVec[i][j].z);

                phong += pow(dotRprimeV, shiny) * scalingFactor;

                colorPixel[i][j].r += diffu * lambert * colorPixel[i][j].r / amb + spcl * phong * colorPixel[i][j].r / amb;
                colorPixel[i][j].g += diffu * lambert * colorPixel[i][j].g / amb + spcl * phong * colorPixel[i][j].g / amb;
                colorPixel[i][j].b += diffu * lambert * colorPixel[i][j].b / amb + spcl * phong * colorPixel[i][j].b / amb;

//                colorPixel[i][j].r = amb * colorPixel[i][j].r + diffu * lambert * colorPixel[i][j].r + spcl * phong * colorPixel[i][j].r;
//                colorPixel[i][j].g = amb * colorPixel[i][j].g + diffu * lambert * colorPixel[i][j].g + spcl * phong * colorPixel[i][j].g;
//                colorPixel[i][j].b = amb * colorPixel[i][j].b + diffu * lambert * colorPixel[i][j].b + spcl * phong * colorPixel[i][j].b;

                //cout << spcl * phong * colorPixel[j][k].r;

                if(colorPixel[i][j].r > 1)
                {
                    colorPixel[i][j].r = 1;
                }
                if(colorPixel[i][j].g > 1)
                {
                    colorPixel[i][j].g = 1;
                }
                if(colorPixel[i][j].b > 1)
                {
                    colorPixel[i][j].b = 1;
                }
                }
                }
                }







































        }
    }


    bitmap_image image(numPixelAlongAxis, numPixelAlongAxis);
    for (int x = 0; x < numPixelAlongAxis; x++)
    {
        for (int y = 0; y < numPixelAlongAxis; y++)
        {
            image.set_pixel(numPixelAlongAxis - 1 - y, x, colorPixel[x][y].r * 255, colorPixel[x][y].g * 255, colorPixel[x][y].b * 255);
        }
    }
    image.save_image("out.bmp");
    cout << "done";
}


void keyboardListener(unsigned char key, int x,int y){
    point newU, newR, newL;
	switch(key){

		case '1':
			//drawgrid=1-drawgrid;

			newR.x = r.x * cos(pi / 180) + l.x * sin(pi / 180) * (-1);
			newR.y = r.y * cos(pi / 180) + l.y * sin(pi / 180) * (-1);
			newR.z = r.z * cos(pi / 180) + l.z * sin(pi / 180) * (-1);

			newL.x = l.x * cos(pi / 180) + r.x * sin(pi / 180);
			newL.y = l.y * cos(pi / 180) + r.y * sin(pi / 180);
			newL.z = l.z * cos(pi / 180) + r.z * sin(pi / 180);

			r = newR;
			l = newL;

			break;

        case '2':
			newR.x = r.x * cos(pi / 180) + l.x * sin(pi / 180);
			newR.y = r.y * cos(pi / 180) + l.y * sin(pi / 180);
			newR.z = r.z * cos(pi / 180) + l.z * sin(pi / 180);

			newL.x = l.x * cos(pi / 180) + r.x * sin(pi / 180) * (-1);
			newL.y = l.y * cos(pi / 180) + r.y * sin(pi / 180) * (-1);
			newL.z = l.z * cos(pi / 180) + r.z * sin(pi / 180) * (-1);

			r = newR;
			l = newL;


			break;

        case '3':

			newL.x = l.x * cos(pi / 180) + u.x * sin(pi / 180);
			newL.y = l.y * cos(pi / 180) + u.y * sin(pi / 180);
			newL.z = l.z * cos(pi / 180) + u.z * sin(pi / 180);

            newU.x = u.x * cos(pi / 180) + l.x * sin(pi / 180) * (-1);
			newU.y = u.y * cos(pi / 180) + l.y * sin(pi / 180) * (-1);
			newU.z = u.z * cos(pi / 180) + l.z * sin(pi / 180) * (-1);

			u = newU;
			l = newL;


			break;

        case '4':

			newU.x = u.x * cos(pi / 180) + l.x * sin(pi / 180);
			newU.y = u.y * cos(pi / 180) + l.y * sin(pi / 180);
			newU.z = u.z * cos(pi / 180) + l.z * sin(pi / 180);

            newL.x = l.x * cos(pi / 180) + u.x * sin(pi / 180) * (-1);
			newL.y = l.y * cos(pi / 180) + u.y * sin(pi / 180) * (-1);
			newL.z = l.z * cos(pi / 180) + u.z * sin(pi / 180) * (-1);

			u = newU;
			l = newL;


			break;

        case '5':
            newR.x = r.x * cos(pi / 180) + u.x * sin(pi / 180);
			newR.y = r.y * cos(pi / 180) + u.y * sin(pi / 180);
			newR.z = r.z * cos(pi / 180) + u.z * sin(pi / 180);

            newU.x = u.x * cos(pi / 180) + r.x * sin(pi / 180) * (-1);
			newU.y = u.y * cos(pi / 180) + r.y * sin(pi / 180) * (-1);
			newU.z = u.z * cos(pi / 180) + r.z * sin(pi / 180) * (-1);

			u = newU;
			r = newR;



			break;

        case '6':
            newU.x = u.x * cos(pi / 180) + r.x * sin(pi / 180);
			newU.y = u.y * cos(pi / 180) + r.y * sin(pi / 180);
			newU.z = u.z * cos(pi / 180) + r.z * sin(pi / 180);

            newR.x = r.x * cos(pi / 180) + u.x * sin(pi / 180) * (-1);
			newR.y = r.y * cos(pi / 180) + u.y * sin(pi / 180) * (-1);
			newR.z = r.z * cos(pi / 180) + u.z * sin(pi / 180) * (-1);

			u = newU;
			r = newR;

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
        case '0':
            startRayTracing = 1 - startRayTracing;
            rayTracingFunction();

			break;
        case 't':
            txVar = 1 - txVar;

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
			//pos.y += 2;
			pos.y -= 2 * l.y;
			pos.x -= 2 * l.x;
			pos.z -= 2 * l.z;

			break;
		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 3.0;
			//pos.x += 2* u.x;
			/**pos.y += 2* l.y;**/
			//pos.z += 2* u.z;

			pos.y += 2 * l.y;
			pos.x += 2 * l.x;
			pos.z += 2 * l.z;
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			/**pos.x += 2* r.x;**/
			//pos.y += 2* r.y;
			//pos.z += 2* r.z;
			//pos.x -= 2;
			pos.y += 2 * r.y;
			pos.x += 2 * r.x;
			pos.z += 2 * r.z;
			break;
		case GLUT_KEY_LEFT:
			//cameraAngle -= 0.03;
			/**pos.x -= 2* r.x;**/
			//pos.y -= 2* r.y;
			//pos.z -= 2* r.z;


			pos.y -= 2 * r.y;
			pos.x -= 2 * r.x;
			pos.z -= 2 * r.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    //pos.z += 2 * u.z;
		    pos.y += 2 * u.y;
			pos.x += 2 * u.x;
			pos.z += 2 * u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    //pos.z -= 2 * u.z;
		    pos.y -= 2 * u.y;
			pos.x -= 2 * u.x;
			pos.z -= 2 * u.z;
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
	//cout << pos.x << " " << pos.y << " " << pos.z << "\n";
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
	//drawAxes();
	//drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    //drawGunOffline();
    rtOffline();
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
	inputTaking();
	colorInitialize();
	loadImageBmp();
	txVar = 0;
	startRayTracing = 0;
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


    setCameraPosition(16, 100, 42);
    setUvector(0, 0, 1);
    setLvector(0, -1, 0);
    setRvector(-1, 0, 0);
    //setLvector((-1/sqrt(2)), (-1/sqrt(2)), 0);
    //setRvector((-1/sqrt(2)), (1/sqrt(2)), 0);
    //seeDotProduct(u,r);

    tVal tvl;
    tvl.tval = 1000;
    tvl.indexO = 100;
    tvl.type = 3;
    tvl.tNo = 100;
    tvl.whiteBlack = 2;

    for(int i=0; i<800; i++)
    {
        for(int j=0; j<800; j++)
        {
            forT[i][j] = 1000;
            storeT[i][j] = tvl;
        }
    }


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
	//gluPerspective(fov_Y,	aspectRatio,	neaR,	faR);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
    //inputTaking();
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Ray Tracing");

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
