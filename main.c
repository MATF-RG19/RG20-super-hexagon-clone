#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>


#define TIMER_ID (0)
#define TIMER_INTERVAL (17) // ~60 rotations per second

#define KEY_ESCAPE (27)
#define KEY_LEFT ('a')
#define KEY_RIGHT ('d')

#define HEXAGON_X (0.87) // ~sqrt(3) / 2
#define HEXAGON_Y (0.5)
#define HEXAGON_X_AXIS (0.0)
#define HEXAGON_Y_AXIS (1.0)
#define HEXAGON_SCALING_FACTOR (0.995)
#define HEXAGON_ROTATION_STEP (5)
#define HEXAGON_POSITIVE_ROTATION_DIRECTION (1)
#define HEXAGON_NEGATIVE_ROTATION_DIRECTION (-1)

static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void drawHexagon();

static int window_width, window_height;
static int animation_ongoing;

static double scaling_factor = 1;
static double rotation_step = 0;
static double rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;

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

    animation_ongoing = 0;

    glClearColor(0.75, 0.75, 0.75, 0.0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);

    glutMainLoop();
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case KEY_ESCAPE:
        exit(0);
        break;

    case KEY_LEFT:
        printf("go left\n");

        rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;
    
    case KEY_RIGHT:
        printf("go right\n");
        rotation_direction = HEXAGON_NEGATIVE_ROTATION_DIRECTION;
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;
    }
}

static void on_reshape(int width, int height)
{
    window_height = height;
    window_width = width;
}

static void on_timer(int value)
{
    if (value != TIMER_ID)
        return;

    glutPostRedisplay();

    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3f(0, 0, 1);
    glLoadIdentity();
    gluLookAt (
        0.1, 0.2, -0.3, 
        0.0, 0.0, 0.0, 
        0.0, 2.0, 0.0
    );

    glRotatef(rotation_step * rotation_direction, 0, 0, 1);
    glScalef(scaling_factor, scaling_factor, scaling_factor);

    drawHexagon(0);
    drawHexagon(1.1);
    drawHexagon(0.9);
    drawHexagon(0.8);
    drawHexagon(0.7);

    

    rotation_step += HEXAGON_ROTATION_STEP;
    scaling_factor *= HEXAGON_SCALING_FACTOR;

    printf("rotation_direction: %lf\n", rotation_direction);

    glutSwapBuffers();
}

static void drawHexagon(double scale_factor)
{
    printf("drawing with scale factor: %lf\n", scale_factor);
    glPushMatrix();
        if (scale_factor != 0) {
            glScalef(scale_factor, scale_factor, scale_factor);
        }
        glBegin(GL_LINE_LOOP);
            glVertex3f(HEXAGON_X_AXIS,  HEXAGON_Y_AXIS,   0);
            glVertex3f(HEXAGON_X,       HEXAGON_Y,        0);
            glVertex3f(HEXAGON_X,      -HEXAGON_Y,        0);
            glVertex3f(HEXAGON_X_AXIS, -HEXAGON_Y_AXIS,   0);
            glVertex3f(-HEXAGON_X,     -HEXAGON_Y,        0);
            glColor3f(1, 0, 0);
            glVertex3f(-HEXAGON_X,      HEXAGON_Y,        0);
        glEnd();
    glPopMatrix();
}

