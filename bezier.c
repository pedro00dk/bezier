
#include "math.h"
#include "stdio.h"
#include "GL/freeglut.h"

#define POINT_SIZE 10
#define POINT_NEAR 20
#define MAX_POINTS 100
#define CURVE_TICK 0.0001

struct Point
{
    float x, y;
};

char near(struct Point *p, int x, int y, int threshold)
{
    return abs(p->x - x) <= threshold && abs(p->y - y) <= threshold ? 1 : 0;
}

struct Point points[MAX_POINTS];
int pointCount = 0;

long double combinations[MAX_POINTS][MAX_POINTS];

void computeCombinations()
{
    combinations[0][0] = 1;
    for (int i = 1; i < MAX_POINTS; i++)
    {
        combinations[i][0] = combinations[i][i] = 1;
        for (int j = 1; j < i; j++)
            combinations[i][j] = combinations[i - 1][j - 1] + combinations[i - 1][j];
    }
}

void bezier(void)
{
    glBegin(GL_LINE_STRIP);
    glColor3d(0.1, 0.1, 0.1);
    int n = pointCount - 1;
    for (float t = 0; t <= 1; t += CURVE_TICK)
    {
        long double tx = 0, ty = 0;
        for (int i = 0; i <= n; i++)
        {
            long double coefficient = combinations[n][i] * pow(1 - t, n - i) * pow(t, i);
            tx += coefficient * points[i].x;
            ty += coefficient * points[i].y;
        }
        glVertex2d(tx, ty);
    }
    glEnd();
}

void initialize(void)
{
    glClearColor(0.9, 0.9, 1.0, 1.0);
    glPointSize(POINT_SIZE);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);
    glColor3d(0.8, 0.3, 0.3);
    for (int i = 0; i < pointCount; i++)
        glVertex2d(points[i].x, points[i].y);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glColor3d(0.8, 0.6, 0.6);
    for (int i = 0; i < pointCount; i++)
        glVertex2d(points[i].x, points[i].y);
    glEnd();
    bezier();
    glFlush();
}

float currentWidth = 1;
float currentHeight = 1;

void reshape(int width, int height)
{
    width = width > 0 ? width : 1;
    height = height > 0 ? height : 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    for (int i = 0; i < pointCount; i++)
    {
        points[i].x *= (width / currentWidth);
        points[i].y *= (height / currentHeight);
    }
    currentWidth = width;
    currentHeight = height;
    glutPostRedisplay();
}

int selectedPointIndex = -1;

void mouse(int button, int state, int x, int y)
{
    int nearPointIndex = -1;
    for (int i = 0; i < pointCount; i++)
        if (near(&points[i], x, y, POINT_NEAR))
        {
            nearPointIndex = i;
            break;
        }
    if (state == GLUT_DOWN)
    {
        selectedPointIndex = nearPointIndex;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && selectedPointIndex == -1 && pointCount < MAX_POINTS)
    {
        struct Point point = {x, y};
        points[pointCount++] = point;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        selectedPointIndex = -1;
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && selectedPointIndex != -1)
    {
        for (int i = selectedPointIndex; i < pointCount - 1; i++)
            points[i] = points[i + 1];
        pointCount--;
    }
    glutPostRedisplay();
}

void motion(int x, int y)
{
    if (selectedPointIndex == -1)
        return;
    points[selectedPointIndex].x = x;
    points[selectedPointIndex].y = y;
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    computeCombinations();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    int width = glutGet(GLUT_SCREEN_WIDTH);
    int height = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize(width != 0 ? width * 0.8 : 800, height != 0 ? height * 0.8 : 480);
    glutInitWindowPosition(width != 0 ? width * 0.1 : 100, height != 0 ? height * 0.1 : 100);
    glutCreateWindow("Bezier Curve");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    initialize();
    glutMainLoop();
}
