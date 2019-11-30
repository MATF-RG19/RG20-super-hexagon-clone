#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY_ESCAPE (27)
#define KEY_LEFT ('j')
#define KEY_RIGHT ('l')


static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);

static int window_width, window_height;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);


    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    glClearColor(0.75, 0.75, 0.75, 0.0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);

    glutMainLoop();
}

void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case KEY_ESCAPE:
        exit(0);
        break;
    case KEY_LEFT:
        printf("go left\n");
        break;
    
    case KEY_RIGHT:
        printf("go right\n");
        break;
    }
}

void on_reshape(int width, int height)
{
    window_height = height;
    window_width = width;
}

void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    double x = 0.87; // ~ sqrt(3) / 2
    double y = 0.5;

    glColor3f(0, 0, 1);
    glBegin(GL_LINE_LOOP);
        glVertex3f(0, 1, 0);
        glVertex3f(x, y, 0);
        glVertex3f(x, -y, 0);
        glVertex3f(0, -1, 0);
        glVertex3f(-x, -y, 0);
        glVertex3f(-x, y, 0);

    glEnd();

    glutSwapBuffers();
}
