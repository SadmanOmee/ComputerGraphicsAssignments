#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <stack>
#include <queue>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))
#define epsilon (1.0e-6)

int totalPush = 0;
stack<int> trackPushPop;
class homogeneous_point
{
public:
    double x, y, z, w;

    // set the three coordinates, set w to 1
    homogeneous_point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }

    /*
    default constructor. does nothing. allows declarations like below:
        matrix m;
    therefore, usage is dangerous
    */
    homogeneous_point() {
    }

    // constructs a homogeneous point with given coordinates. forces w to be 1.0
    // if w is zero, raises error
    homogeneous_point(double x, double y, double z, double w)
    {
        assert (w != 0);
        this->x = x/w;
        this->y = y/w;
        this->z = z/w;
        this->w = 1;
    }

    // adds two points. returns a point forcing w to be 1.0
    homogeneous_point operator+ (const homogeneous_point& point)
    {
        double x = this->x + point.x;
        double y = this->y + point.y;
        double z = this->z + point.z;
        double w = this->w + point.w;
        homogeneous_point p(x, y, z, w);
        return p;
    }

    // subtracts one point from another. returns a point forcing w to be 1.0
    homogeneous_point operator- (const homogeneous_point& point)
    {
        double x = this->x - point.x;
        double y = this->y - point.y;
        double z = this->z - point.z;
        double w = this->w - point.w;
        homogeneous_point p(x, y, z, w);
    }

    // Print the coordinates of a point. exists for testing purpose.
    void print()
    {
        cout << "Point: " << endl;
        cout << x << " " << y << " " << z << " " << w << endl;
    }

};


class Vector
{
public:
    double x, y, z;

    // constructs a vector with given components
    Vector(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // keeps the direction same. recalculates the vector to be unit.
    void normalize()
    {
        double r = sqrt(x*x + y*y + z*z);
        x = x / r;
        y = y / r;
        z = z / r;
    }

    // add two vectors
    Vector operator+(const Vector& v)
    {
        Vector v1(x+v.x, y+v.y, z+v.z);
        return v1;
    }

    // subtract one vector from another
    Vector operator-(const Vector& v)
    {
        Vector v1(x-v.x, y-v.y, z-v.z);
        return v1;
    }

    // scale a vector with a given coefficient
    Vector operator* (double m)
    {
        Vector v(x*m, y*m, z*m);
        return v;
    }

    // get the dot product of two vectors
    static double dot(Vector a, Vector b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    // get the cross product of two vectors
    static Vector cross(Vector a, Vector b)
    {
        Vector v(a.y*b.z - a.z*b.y, b.x*a.z - b.z*a.x, a.x*b.y - a.y*b.x);
        return v;
    }

    // print a vector. only for testing purposes.
    void print ()
    {
        cout << "Vector" << endl;
        cout << x << " " << y << " " << z << endl;
    }
};


/*
The matrices are forced to be 4x4. This is because in this assignment, we will deal with points in triangles.
Maximum # of points that we will deal with at once is 3. And all the standard matrices are 4x4 (i.e. scale, translation, rotation etc.)
*/
class matrix
{
public:
    double values[4][4];
    int num_rows, num_cols;

    // only set the number of rows and cols
    matrix(int rows, int cols)
    {
        assert (rows <= 4 && cols <= 4);
        num_rows = rows;
        num_cols = cols;
    }

    // prepare an nxn square matrix
    matrix(int n)
    {
        assert (n <= 4);
        num_rows = num_cols = n;
    }

    // prepare and return an identity matrix of size nxn
    static matrix make_identity(int n)
    {
        assert (n <= 4);
        matrix m(n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                    m.values[i][j] = 1;
                else
                    m.values[i][j] = 0;
            }
        }
        return m;
    }

    // print the matrix. exists for testing purposes
    void print()
    {
        cout << "Matrix:" << endl;
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                cout << values[i][j] << "\t";
            }
            cout << endl;
        }
    }

    // add the two matrices. Raise error if dimension mismatches
    matrix operator+ (const matrix& m)
    {
        assert (this->num_rows == m.num_rows);
        assert (this->num_cols == m.num_cols);

        matrix m1(num_rows, num_cols);
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                m1.values[i][j] = values[i][j] + m.values[i][j];
            }
        }
        return m1;
    }

    // subtract a matrix from another. raise error if dimension mismatches
    matrix operator- (const matrix& m)
    {
        assert (this->num_rows == m.num_rows);
        assert (this->num_cols == m.num_cols);

        matrix m1(num_rows, num_cols);
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                m1.values[i][j] = values[i][j] - m.values[i][j];
            }
        }
        return m1;
    }

    // multiply two matrices. allows statements like m1 = m2 * m3; raises error is dimension mismatches
    matrix operator* (const matrix& m)
    {
        assert (this->num_cols == m.num_rows);
        matrix m1(this->num_rows, m.num_cols);

        for (int i = 0; i < m1.num_rows; i++) {
            for (int j = 0; j < m1.num_cols; j++) {
                double val = 0;
                for (int k = 0; k < this->num_cols; k++) {
                    val += this->values[i][k] * m.values[k][j];
                }
                m1.values[i][j] = val;
            }
        }
        return m1;
    }

    // multiply a matrix with a constant
    matrix operator* (double m)
    {
        matrix m1(this->num_rows, this->num_cols);
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                m1.values[i][j] = m * this->values[i][j];
            }
        }
        return m1;
    }

    // multiply a 4x4 matrix with a homogeneous point and return the resulting point.
    // usage: homogeneous_point p = m * p1;
    // here, m is a 4x4 matrix, intended to be the transformation matrix
    // p1 is the point on which the transformation is being made
    // p is the resulting homogeneous point
    homogeneous_point operator* (const homogeneous_point& p)
    {
        assert (this->num_rows == this->num_cols && this->num_rows == 4);

        matrix m(4, 1);
        m.values[0][0] = p.x;
        m.values[1][0] = p.y;
        m.values[2][0] = p.z;
        m.values[3][0] = p.w;

        matrix m1 = (*this)*m;
        homogeneous_point p1(m1.values[0][0], m1.values[1][0], m1.values[2][0], m1.values[3][0]);
        return p1;
    }

    // return the transpose of a matrix
    matrix transpose()
    {
        matrix m(num_cols, num_rows);
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                m.values[j][i] = values[i][j];
            }
        }
        return m;
    }

};

/*
A simple class to hold the color components, r, g, b of a certain shade.
*/
class color {
public:
    double r, g, b;
    color(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    color() {
    }
};


double eye_x, eye_y, eye_z;
double look_x, look_y, look_z;
double up_x, up_y, up_z;
double fov_x, fov_y, aspectRatio, near, far;
color backgroud;
int screen_x, screen_y;

color colorOfTrinagle[1000];
int colorOfTrinagleIndex = -1;
color newColorOfTrinagle[1000];
int newColorOfTrinagleIndex = -1;



void scan_convert() {
    ifstream stage3;
    stage3.open("stage3.txt");

    color** pixels = new color*[screen_x];
    double** zs = new double*[screen_x];
    for (int i = 0; i < screen_x; i++) {
        pixels[i] = new color [screen_y];
        for (int j = 0; j < screen_y; j++) {
            pixels[i][j] = backgroud;
        }
        zs[i] = new double [screen_y];
        for (int j = 0; j < screen_y; j++) {
            zs[i][j] = +20; // a very large value intended as +INFINITY
        }
    }

    // perform scan conversion, populate the 2D array pixels
    // the array zs is the z-buffer.

    double t1x, t1y, t1z, t2x, t2y, t2z, t3x, t3y, t3z;
    homogeneous_point pointSerial[3];
    int total = -1;
    while(stage3 >> t1x >> t1y >> t1z >> t2x >> t2y >> t2z >> t3x >> t3y >> t3z)
    {
        homogeneous_point t1(t1x, t1y, t1z), t2(t2x, t2y, t2z), t3(t3x, t3y, t3z);

        total++;

        pointSerial[0] = t1;
        pointSerial[1] = t2;
        pointSerial[2] = t3;

        for(int i=0; i<3; i++)
        {
            for(int j=0; j<3; j++)
            {
                if(pointSerial[i].y > pointSerial[j].y)
                {
                    swap(pointSerial[i], pointSerial[j]);
                }
            }
        }

        homogeneous_point ft = pointSerial[0];
        homogeneous_point st = pointSerial[1];
        homogeneous_point tt = pointSerial[2];

        //cout << "done\n";
        //cout << ft.y << " " << st.y << " " << tt.y << "\n";

        int t1f, t2f, t3f, t1s, t2s, t3s;
        double fromScrrenX, fromScrrenY;

        fromScrrenX = (screen_x - 1) / 2.0;
        fromScrrenY = (screen_y - 1) / 2.0;

        t1f = int((ft.x + 1) * fromScrrenX);
        t1s = int((ft.y + 1) * fromScrrenY);
        t2f = int((st.x + 1) * fromScrrenX);
        t2s = int((st.y + 1) * fromScrrenY);
        t3f = int((tt.x + 1) * fromScrrenX);
        t3s = int((tt.y + 1) * fromScrrenY);

        //cout << "done\n";
        //cout << t1f  << " " << t2f << " " << t3f << "\n";
        //cout << t1s  << " " << t2s << " " << t3s << "\n";


        double xt1, xt2, zt1, zt2, cellMiddle;

        for(int i = t1s; i >= t2s; i--)
        {
            cellMiddle = (i * 2.0 + 1) / screen_y - 1;

            if(cellMiddle < st.y)
            {
                break;
            }

            zt1 = ft.z - ((ft.z - st.z) * ((ft.y - cellMiddle) / (ft.y - st.y)));
            zt2 = ft.z - ((ft.z - tt.z) * ((ft.y - cellMiddle) / (ft.y - tt.y)));
            xt1 = ft.x - ((ft.x - st.x) * ((ft.y - cellMiddle) / (ft.y - st.y)));
            xt2 = ft.x - ((ft.x - tt.x) * ((ft.y - cellMiddle) / (ft.y - tt.y)));


            //cout << "done\n";

            int cellX, cellY;

            cellX = int((xt1 + 1) * fromScrrenX);
            cellY = int((xt2 + 1) * fromScrrenX);

            //cout << cellX << " " << cellY << "\n";

            double cellMiddle2, cellMiddleZ;

            if(cellX > cellY)
            {
                for(int j = cellX; j >= cellY; j--)
                {
                    cellMiddle2 = (2.0 * j + 1) / screen_x - 1;
                    cellMiddleZ = zt2 - ((zt2 - zt1) * ((xt2 - cellMiddle2)) / (xt2 - xt1));

                    if(cellMiddleZ < zs[j][screen_y - 1 - i])
                    {
                        zs[j][screen_y - 1 - i] = cellMiddleZ;
                        pixels[j][screen_y - 1 - i] = newColorOfTrinagle[total];
                        //cout << "done\n";
                    }
                }
            }
            else
            {
                for(int j = cellX; j <= cellY; j++)
                {
                    cellMiddle2 = (2.0 * j + 1) / screen_x - 1;
                    cellMiddleZ = zt2 - ((zt2 - zt1) * ((xt2 - cellMiddle2)) / (xt2 - xt1));
                    if(cellMiddleZ < zs[j][screen_y - 1 - i])
                    {
                        zs[j][screen_y - 1 - i] = cellMiddleZ;
                        pixels[j][screen_y - 1 - i] = newColorOfTrinagle[total];
                        //cout << "done\n";
                    }
                }
            }
        }
        for(int i = t2s; i >= t3s; i--)
        {
            cellMiddle = (2.0 * i + 1) / screen_y - 1;
            if(cellMiddle < tt.y)
            {
                break;
            }

            zt1 = ft.z - ((ft.z - tt.z) * ((ft.y - cellMiddle) / (ft.y - tt.y)));
            zt2 = st.z - ((st.z - tt.z) * ((st.y - cellMiddle) / (st.y - tt.y)));
            xt1 = ft.x - ((ft.x - tt.x) * ((ft.y - cellMiddle) / (ft.y - tt.y)));
            xt2 = st.x - ((st.x - tt.x) * ((st.y - cellMiddle) / (st.y - tt.y)));

            int cellX, cellY;

            cellX = int((xt1 + 1) * fromScrrenX);
            cellY = int((xt2 + 1) * fromScrrenX);

            //cout << "done\n";

            double cellMiddle2, cellMiddleZ;

            if(cellX > cellY)
            {
                for(int j = cellX; j >= cellY; j--)
                {
                    cellMiddle2 = (2.0 * j + 1) / screen_x - 1;
                    cellMiddleZ = zt2 - ((zt2 - zt1) * ((xt2 - cellMiddle2)) / (xt2 - xt1));
                    if(cellMiddleZ < zs[j][screen_y - 1 - i])
                    {
                        zs[j][screen_y - 1 - i] = cellMiddleZ;
                        pixels[j][screen_y - 1 - i] = newColorOfTrinagle[total];
                        //cout << "done\n";
                    }
                }
            }
            else
            {
                for(int j = cellX; j <= cellY; j++)
                {
                    cellMiddle2 = (2.0 * j + 1) / screen_x - 1;
                    cellMiddleZ = zt2 - ((zt2 - zt1) * ((xt2 - cellMiddle2)) / (xt2 - xt1));
                    if(cellMiddleZ < zs[j][screen_y - 1 - i])
                    {
                        zs[j][screen_y - 1 - i] = cellMiddleZ;
                        pixels[j][screen_y - 1 - i] = newColorOfTrinagle[total];
                        //cout << "done\n";
                    }
                }
            }
        }
    }


    // the following code generates a bmp image. do not change this.
    bitmap_image image(screen_x, screen_y);
    for (int x = 0; x < screen_x; x++) {
        for (int y = 0; y < screen_y; y++) {
            image.set_pixel(x, y, pixels[x][y].r, pixels[x][y].g, pixels[x][y].b);
        }
    }
    image.save_image("out.bmp");

    // free the dynamically allocated memory

}


void stage3()
{
    if (near == far) return;
    ifstream stage2;
    ofstream stage3;
    stage2.open ("stage2.txt");
    stage3.open ("stage3.txt");
    stage3 << std::fixed;
    stage3 << std::setprecision(7);

    // process input from stage2 and write to stage3


    fov_x = fov_y * aspectRatio;
    double rp, tp;
    tp = near * tan(fov_y/2 * pi / 180);
    rp = near * tan(fov_x/2 * pi / 180);

    matrix projectinMatrix(4);
    projectinMatrix = matrix::make_identity(4);
    projectinMatrix.values[0][0] = near / rp;
    projectinMatrix.values[1][1] = near / tp;
    projectinMatrix.values[2][2] = -(far + near) / (far - near);
    projectinMatrix.values[2][3] = -(2 * far * near) / (far - near);
    projectinMatrix.values[3][2] = -1;
    projectinMatrix.values[3][3] = 0;

    double t1x, t1y, t1z, t2x, t2y, t2z, t3x, t3y, t3z;
    int oldTriangleColorIndex = -1;
    while(stage2 >> t1x)
    {
        stage2 >> t1y >> t1z;
        stage2 >> t2x >> t2y >> t2z;
        stage2 >> t3x >> t3y >> t3z;
        homogeneous_point t1(t1x, t1y, t1z), t2(t2x, t2y, t2z), t3(t3x, t3y, t3z);
        //homogeneous_point ft = (t1z < t2z) ? ((t1.z < t3.z) ?  t1 : t3) : ((t2.z < t3.z) ? t2 : t3);
        oldTriangleColorIndex++;

        homogeneous_point pointSerial[3];
        pointSerial[0] = t1;
        pointSerial[1] = t2;
        pointSerial[2] = t3;

        for(int i=0; i<3; i++)
        {
            for(int j=0; j<3; j++)
            {
                if(pointSerial[i].z > pointSerial[j].z)
                {
                    swap(pointSerial[i], pointSerial[j]);
                }
            }
        }
        homogeneous_point ft = pointSerial[0];
        homogeneous_point st = pointSerial[1];
        homogeneous_point tt = pointSerial[2];
        //cout << ft.z << " " << st.z << " " << tt.z << endl;

        if((ft.z < -near && ft.z > -far) && (st.z < -near && st.z > -far) && (tt.z < -near && tt.z > -far)) // case 3 in
        {
            homogeneous_point newft = projectinMatrix * ft;
            homogeneous_point newst = projectinMatrix * st;
            homogeneous_point newtt = projectinMatrix * tt;

            stage3 << newft.x << " " << newft.y << " " << newft.z << "\n";
            stage3 << newst.x << " " << newst.y << " " << newst.z << "\n";
            stage3 << newtt.x << " " << newtt.y << " " << newtt.z << "\n";
            stage3 << "\n";

            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else if((ft.z > -near) && (st.z < -near && st.z > -far) && (tt.z < -near && tt.z > -far)) //case 1 out(left) 2 in
        {
            double newP1x, newP1y, newP1z, newP2x, newP2y, newP2z, v1, v2, v3;

            v1 = ft.x - st.x;
            v2 = ft.y - st.y;
            v3 = ft.z - st.z;

            newP1x = st.x + (-near - st.z) * v1 / v3;
            newP1y = st.y + (-near - st.z) * v2 / v3;
            newP1z = -near;

            v1 = ft.x - tt.x;
            v2 = ft.y - tt.y;
            v3 = ft.z - tt.z;

            newP2x = tt.x + (-near - tt.z) * v1 / v3;
            newP2y = tt.y + (-near - tt.z) * v2 / v3;
            newP2z = -near;

            homogeneous_point cp1(newP1x, newP1y, newP1z), cp2(newP2x, newP2y, newP2z);

            homogeneous_point newst = projectinMatrix * st;
            homogeneous_point newtt = projectinMatrix * tt;
            homogeneous_point newcp1 = projectinMatrix * cp1;
            homogeneous_point newcp2 = projectinMatrix * cp2;

            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newst.x << " " << newst.y << " " << newst.z << "\n";
            stage3 << newtt.x << " " << newtt.y << " " << newtt.z << "\n";
            stage3 << "\n";
            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << newtt.x << " " << newtt.y << " " << newtt.z << "\n";
            stage3 << "\n";

            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else if((ft.z < -near && ft.z > -far) && (st.z < -near && st.z > -far) && (tt.z < -far)) //case 1 out(right) 2 in
        {
            double newP1x, newP1y, newP1z, newP2x, newP2y, newP2z, v1, v2, v3;

            v1 = tt.x - st.x;
            v2 = tt.y - st.y;
            v3 = tt.z - st.z;

            newP1x = st.x + (-far - st.z) * v1 / v3;
            newP1y = st.y + (-far - st.z) * v2 / v3;
            newP1z = -far;

            v1 = tt.x - ft.x;
            v2 = tt.y - ft.y;
            v3 = tt.z - ft.z;

            newP2x = ft.x + (-far - ft.z) * v1 / v3;
            newP2y = ft.y + (-far - ft.z) * v2 / v3;
            newP2z = -far;

            homogeneous_point cp1(newP1x, newP1y, newP1z), cp2(newP2x, newP2y, newP2z);

            homogeneous_point newft = projectinMatrix * ft;
            homogeneous_point newst = projectinMatrix * st;
            homogeneous_point newcp1 = projectinMatrix * cp1;
            homogeneous_point newcp2 = projectinMatrix * cp2;

            stage3 << newst.x << " " << newst.y << " " << newst.z << "\n";
            stage3 << newft.x << " " << newft.y << " " << newft.z << "\n";
            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << "\n";
            stage3 << newft.x << " " << newft.y << " " << newft.z << "\n";
            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << "\n";

            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else if((ft.z > -near) && (st.z > -near) && (tt.z < -near && tt.z > -far)) //case 2 out(left) 1 in
        {
            double newP1x, newP1y, newP1z, newP2x, newP2y, newP2z, v1, v2, v3;

            v1 = tt.x - st.x;
            v2 = tt.y - st.y;
            v3 = tt.z - st.z;

            newP1x = st.x + (-near - st.z) * v1 / v3;
            newP1y = st.y + (-near - st.z) * v2 / v3;
            newP1z = -near;

            v1 = tt.x - ft.x;
            v2 = tt.y - ft.y;
            v3 = tt.z - ft.z;

            newP2x = ft.x + (-near - ft.z) * v1 / v3;
            newP2y = ft.y + (-near - ft.z) * v2 / v3;
            newP2z = -near;

            homogeneous_point cp1(newP1x, newP1y, newP1z), cp2(newP2x, newP2y, newP2z);

            homogeneous_point newtt = projectinMatrix * tt;
            homogeneous_point newcp1 = projectinMatrix * cp1;
            homogeneous_point newcp2 = projectinMatrix * cp2;

            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << newtt.x << " " << newtt.y << " " << newtt.z << "\n";
            stage3 << "\n";
            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else if((ft.z < -near && ft.z > -far) && (st.z < -far) && (tt.z < -far)) //case 2 out(right) 1 in
        {
            double newP1x, newP1y, newP1z, newP2x, newP2y, newP2z, v1, v2, v3;

            v1 = ft.x - st.x;
            v2 = ft.y - st.y;
            v3 = ft.z - st.z;

            newP1x = st.x + (-far - st.z) * v1 / v3;
            newP1y = st.y + (-far - st.z) * v2 / v3;
            newP1z = -far;

            v1 = ft.x - tt.x;
            v2 = ft.y - tt.y;
            v3 = ft.z - tt.z;

            newP2x = tt.x + (-far - tt.z) * v1 / v3;
            newP2y = tt.y + (-far - tt.z) * v2 / v3;
            newP2z = -far;

            homogeneous_point cp1(newP1x, newP1y, newP1z), cp2(newP2x, newP2y, newP2z);

            homogeneous_point newft = projectinMatrix * ft;
            homogeneous_point newcp1 = projectinMatrix * cp1;
            homogeneous_point newcp2 = projectinMatrix * cp2;

            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << newft.x << " " << newft.y << " " << newft.z << "\n";
            stage3 << "\n";
            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else if((ft.z > -near) && (st.z < -near && st.z > -far) && (tt.z < -far)) //case 1 left(out) 1 in 1 right(out)
        {
            double newP1x, newP1y, newP1z, newP2x, newP2y, newP2z, newP3x, newP3y, newP3z, newP4x, newP4y, newP4z, v1, v2, v3;

            v1 = ft.x - st.x;
            v2 = ft.y - st.y;
            v3 = ft.z - st.z;

            newP1x = st.x + (-near - st.z) * v1 / v3;
            newP1y = st.y + (-near - st.z) * v2 / v3;
            newP1z = -near;

            v1 = ft.x - tt.x;
            v2 = ft.y - tt.y;
            v3 = ft.z - tt.z;

            newP2x = tt.x + (-near - tt.z) * v1 / v3;
            newP2y = tt.y + (-near - tt.z) * v2 / v3;
            newP2z = -near;

            homogeneous_point cp1(newP1x, newP1y, newP1z), cp2(newP2x, newP2y, newP2z);

            v1 = tt.x - st.x;
            v2 = tt.y - st.y;
            v3 = tt.z - st.z;

            newP3x = st.x + (-far - st.z) * v1 / v3;
            newP3y = st.y + (-far - st.z) * v2 / v3;
            newP3z = -far;

            v1 = tt.x - ft.x;
            v2 = tt.y - ft.y;
            v3 = tt.z - ft.z;

            newP4x = ft.x + (-far - ft.z) * v1 / v3;
            newP4y = ft.y + (-far - ft.z) * v2 / v3;
            newP4z = -far;

            homogeneous_point cp3(newP3x, newP3y, newP3z), cp4(newP4x, newP4y, newP4z);

            homogeneous_point newcp1 = projectinMatrix * cp1;
            homogeneous_point newcp2 = projectinMatrix * cp2;
            homogeneous_point newcp3 = projectinMatrix * cp3;
            homogeneous_point newcp4 = projectinMatrix * cp4;
            homogeneous_point newst = projectinMatrix * st;

            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << newst.x << " " << newst.y << " " << newst.z << "\n";
            stage3 << "\n";
            stage3 << newst.x << " " << newst.y << " " << newst.z << "\n";
            stage3 << newcp3.x << " " << newcp3.y << " " << newcp3.z << "\n";
            stage3 << newcp4.x << " " << newcp4.y << " " << newcp4.z << "\n";
            stage3 << "\n";

            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else if((ft.z > -near) && (st.z < -far) && (tt.z < -far)) //case 3 out(1 left 2 right)
        {
            double newP1x, newP1y, newP1z, newP2x, newP2y, newP2z, newP3x, newP3y, newP3z, newP4x, newP4y, newP4z, v1, v2, v3;

            v1 = ft.x - st.x;
            v2 = ft.y - st.y;
            v3 = ft.z - st.z;

            newP1x = st.x + (-near - st.z) * v1 / v3;
            newP1y = st.y + (-near - st.z) * v2 / v3;
            newP1z = -near;

            v1 = ft.x - tt.x;
            v2 = ft.y - tt.y;
            v3 = ft.z - tt.z;

            newP2x = tt.x + (-near - tt.z) * v1 / v3;
            newP2y = tt.y + (-near - tt.z) * v2 / v3;
            newP2z = -near;

            homogeneous_point cp1(newP1x, newP1y, newP1z), cp2(newP2x, newP2y, newP2z);

            v1 = ft.x - st.x;
            v2 = ft.y - st.y;
            v3 = ft.z - st.z;

            newP3x = st.x + (-far - st.z) * v1 / v3;
            newP3y = st.y + (-far - st.z) * v2 / v3;
            newP3z = -far;

            v1 = ft.x - tt.x;
            v2 = ft.y - tt.y;
            v3 = ft.z - tt.z;

            newP4x = tt.x + (-far - tt.z) * v1 / v3;
            newP4y = tt.y + (-far - tt.z) * v2 / v3;
            newP4z = -far;

            homogeneous_point cp3(newP3x, newP3y, newP3z), cp4(newP4x, newP4y, newP4z);

            homogeneous_point newcp1 = projectinMatrix * cp1;
            homogeneous_point newcp2 = projectinMatrix * cp2;
            homogeneous_point newcp3 = projectinMatrix * cp3;
            homogeneous_point newcp4 = projectinMatrix * cp4;

            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << newcp3.x << " " << newcp3.y << " " << newcp3.z << "\n";
            stage3 << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << newcp3.x << " " << newcp3.y << " " << newcp3.z << "\n";
            stage3 << newcp4.x << " " << newcp4.y << " " << newcp4.z << "\n";
            stage3 << "\n";

            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else if((ft.z > -near) && (st.z > -near) && (tt.z < -far)) //case 3 out(2 left 1 right)
        {
            double newP1x, newP1y, newP1z, newP2x, newP2y, newP2z, newP3x, newP3y, newP3z, newP4x, newP4y, newP4z, v1, v2, v3;

            v1 = tt.x - st.x;
            v2 = tt.y - st.y;
            v3 = tt.z - st.z;

            newP1x = st.x + (-near - st.z) * v1 / v3;
            newP1y = st.y + (-near - st.z) * v2 / v3;
            newP1z = -near;

            v1 = tt.x - ft.x;
            v2 = tt.y - ft.y;
            v3 = tt.z - ft.z;

            newP2x = ft.x + (-near - ft.z) * v1 / v3;
            newP2y = ft.y + (-near - ft.z) * v2 / v3;
            newP2z = -near;

            homogeneous_point cp1(newP1x, newP1y, newP1z), cp2(newP2x, newP2y, newP2z);

            v1 = ft.x - st.x;
            v2 = ft.y - st.y;
            v3 = ft.z - st.z;

            newP3x = st.x + (-far - st.z) * v1 / v3;
            newP3y = st.y + (-far - st.z) * v2 / v3;
            newP3z = -far;

            v1 = ft.x - tt.x;
            v2 = ft.y - tt.y;
            v3 = ft.z - tt.z;

            newP4x = tt.x + (-far - tt.z) * v1 / v3;
            newP4y = tt.y + (-far - tt.z) * v2 / v3;
            newP4z = -far;

            homogeneous_point cp3(newP3x, newP3y, newP3z), cp4(newP4x, newP4y, newP4z);


            homogeneous_point newcp1 = projectinMatrix * cp1;
            homogeneous_point newcp2 = projectinMatrix * cp2;
            homogeneous_point newcp3 = projectinMatrix * cp3;
            homogeneous_point newcp4 = projectinMatrix * cp4;


            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp2.x << " " << newcp2.y << " " << newcp2.z << "\n";
            stage3 << newcp4.x << " " << newcp4.y << " " << newcp4.z << "\n";
            stage3 << "\n";
            stage3 << newcp1.x << " " << newcp1.y << " " << newcp1.z << "\n";
            stage3 << newcp3.x << " " << newcp3.y << " " << newcp3.z << "\n";
            stage3 << newcp4.x << " " << newcp4.y << " " << newcp4.z << "\n";
            stage3 << "\n";

            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
            newColorOfTrinagle[++newColorOfTrinagleIndex] = colorOfTrinagle[oldTriangleColorIndex];
        }
        else
        {

        }




        //homogeneous_point newPnt = projectinMatrix * pnt;
        //stage3 << newPnt.x << " " << newPnt.y << " " << newPnt.z << "\n";
        //stage2 << "\n";
    }




    stage3.close();
    stage2.close();

}

void stage2()
{
    ifstream stage1;
    ofstream stage2;
    stage1.open ("stage1.txt");
    stage2.open ("stage2.txt");
    stage2 << std::fixed;
    stage2 << std::setprecision(7);

    // collect input from stage1 and process, write output to stage2


    Vector l(look_x - eye_x, look_y - eye_y, look_z - eye_z);
    l.normalize();
    Vector upVec(up_x, up_y, up_z);
    Vector r = Vector::cross(l, upVec);
    r.normalize();
    Vector u = Vector::cross(r, l);

    matrix transtateEyeCameraToOrigin(4);
    transtateEyeCameraToOrigin=matrix::make_identity(4);
    transtateEyeCameraToOrigin.values[0][3] = -eye_x;
    transtateEyeCameraToOrigin.values[1][3] = -eye_y;
    transtateEyeCameraToOrigin.values[2][3] = -eye_z;


    matrix rotateToAllign(4);
    rotateToAllign=matrix::make_identity(4);
    rotateToAllign.values[0][0] = r.x;
    rotateToAllign.values[0][1] = r.y;
    rotateToAllign.values[0][2] = r.z;

    rotateToAllign.values[1][0] = u.x;
    rotateToAllign.values[1][1] = u.y;
    rotateToAllign.values[1][2] = u.z;

    rotateToAllign.values[2][0] = -l.x;
    rotateToAllign.values[2][1] = -l.y;
    rotateToAllign.values[2][2] = -l.z;



    double coordinateX, coordinateY, coordinateZ;
    int totalPointCount = 0;
    while(stage1 >> coordinateX >> coordinateY >> coordinateZ)
    {
        totalPointCount++;
        homogeneous_point pnt(coordinateX, coordinateY, coordinateZ);
        homogeneous_point newPnt = (rotateToAllign * transtateEyeCameraToOrigin) * pnt;
        stage2 << newPnt.x << " " << newPnt.y << " " << newPnt.z << "\n";
        if(totalPointCount % 3 == 0)
        {
            stage2 << "\n";
        }
    }

    stage1.close();
    stage2.close();

}

void stage1()
{
    ifstream scene;
    ofstream stage1;
    scene.open ("scene.txt");
    stage1.open ("stage1.txt");
    stage1 << std::fixed;
    stage1 << std::setprecision(7);

    string command;
    string abc;

    scene >> eye_x >> eye_y >> eye_z;
    scene >> look_x >> look_y >> look_z;
    scene >> up_x >> up_y >> up_z;
    scene >> fov_y >> aspectRatio >> near >> far;
    scene >> screen_x >> screen_y;
    scene >> backgroud.r >> backgroud.g >> backgroud.b;
    //cout << eye_x << endl;
    //cout << eye_x + eye_y;
    stack<matrix> S;
    matrix mat(4);
    mat = mat.make_identity(4);


    S.push(mat);
    trackPushPop.push(totalPush);

    while(true)
    {
        scene >> command;
        if(command == "triangle")
        {
            double p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, red, blue, green;

            scene >> p1x >> p1y >> p1z;
            scene >> p2x >> p2y >> p2z;
            scene >> p3x >> p3y >> p3z;
            scene >> red >> green >> blue;

            colorOfTrinagle[++colorOfTrinagleIndex].r = red;
            colorOfTrinagle[colorOfTrinagleIndex].g = green;
            colorOfTrinagle[colorOfTrinagleIndex].b = blue;

            homogeneous_point p1(p1x, p1y, p1z), p2(p2x, p2y, p2z), p3(p3x, p3y, p3z);
            homogeneous_point newP1 = S.top() * p1;
            homogeneous_point newP2 = S.top() * p2;
            homogeneous_point newP3 = S.top() * p3;

            stage1 << newP1.x << " " << newP1.y << " " << newP1.z << "\n";
            stage1 << newP2.x << " " << newP2.y << " " << newP2.z << "\n";
            stage1 << newP3.x << " " << newP3.y << " " << newP3.z << "\n";
            stage1 << "\n";
        }
        else if(command == "translate")
        {
            double tx, ty, tz;
            scene >> tx >> ty >> tz;

            matrix transformMatrix(4);
            transformMatrix = transformMatrix.make_identity(4);
            transformMatrix.values[0][3] = tx;
            transformMatrix.values[1][3] = ty;
            transformMatrix.values[2][3] = tz;

            S.push(S.top() * transformMatrix);
            trackPushPop.push(totalPush);
        }
        else if(command == "scale")
        {
            double sx, sy, sz;
            scene >> sx >> sy >> sz;

            matrix scaleMatrix(4);
            scaleMatrix = scaleMatrix.make_identity(4);
            scaleMatrix.values[0][0] = sx;
            scaleMatrix.values[1][1] = sy;
            scaleMatrix.values[2][2] = sz;

            S.push(S.top() * scaleMatrix);
            trackPushPop.push(totalPush);
        }
        else if(command == "rotate")
        {
            double ang, ax , ay, az;
            scene >> ang >> ax >> ay >> az;
            Vector coordVec(ax, ay, az);

            coordVec.normalize();
            Vector iVec(1, 0, 0);
            Vector jVec(0, 1, 0);
            Vector kVec(0, 0, 1);

            Vector crosspI = crosspI.cross(coordVec, iVec);
            Vector crosspJ = crosspJ.cross(coordVec, jVec);
            Vector crosspK = crosspK.cross(coordVec, kVec);

            Vector C1 = (iVec * cos(ang * pi / 180)) + (coordVec * ((1 - cos(ang * pi / 180)) * Vector::dot(coordVec, iVec))) + (crosspI * sin(ang * pi / 180));
            Vector C2 = (jVec * cos(ang * pi / 180)) + (coordVec * ((1 - cos(ang * pi / 180)) * Vector::dot(coordVec, jVec))) + (crosspJ * sin(ang * pi / 180));
            Vector C3 = (kVec * cos(ang * pi / 180)) + (coordVec * ((1 - cos(ang * pi / 180)) * Vector::dot(coordVec, kVec))) + (crosspK * sin(ang * pi / 180));

            matrix rotateMatrix(4);
            rotateMatrix = rotateMatrix.make_identity(4);

            rotateMatrix.values[0][0] = C1.x;
            rotateMatrix.values[0][1] = C2.x;
            rotateMatrix.values[0][2] = C3.x;

            rotateMatrix.values[1][0] = C1.y;
            rotateMatrix.values[1][1] = C2.y;
            rotateMatrix.values[1][2] = C3.y;

            rotateMatrix.values[2][0] = C1.z;
            rotateMatrix.values[2][1] = C2.z;
            rotateMatrix.values[2][2] = C3.z;

            S.push(S.top() * rotateMatrix);
            trackPushPop.push(totalPush);
        }
        else if(command == "push")
        {
            totalPush++;
        }
        else if(command == "pop")
        {
            totalPush--;
            while(trackPushPop.top() != totalPush)
            {
                trackPushPop.pop();
                S.pop();
            }
        }
        else if(command == "end")
        {
            //cout << "aaaa";
            break;
        }
    }



    // take other commands as input from scene in a loop
    // process accordingly
    // write to stage1

    scene.close();
    stage1.close();

}

int main()
{
    cout << std::fixed;
    cout << std::setprecision(4);

    stage1();
    stage2();
    stage3();
    scan_convert();

    return 0;
}
