#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define ESCAPE_KEY (27)

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
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);

    glClearColor(0.75, 0.75, 0.75, 0.0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);

    glutMainLoop();
}

void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case ESCAPE_KEY:
        exit(0);
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

    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60, 
        window_width / (float)window_height,
        1, 
        5
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        1, 2, 3,
        0, 0, 0,
        0, 1, 0
    );

    glColor3f(0, 0, 1);
    glTranslatef(0, 0.5, 0);
    glScalef(1, 1, 1);
    glutWireCube(1);

    glutSwapBuffers();
}
