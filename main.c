#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>


#define TIMER_ID (0)
#define TIMER_INTERVAL (20)

#define KEY_ESCAPE (27)
#define KEY_LEFT ('j')
#define KEY_RIGHT ('l')

#define HEXAGON_X (0.87) // ~sqrt(3) / 2
#define HEXAGON_Y (0.5)
#define HEXAGON_X_AXIS (0.0)
#define HEXAGON_Y_AXIS (1.0)
#define HEXAGON_SCALING_FACTOR (0.95)
#define HEXAGON_ROTATE_STEP (20)


static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);


static int window_width, window_height;
static int animation_ongoing;

static double curr_x = HEXAGON_X;
static double curr_y = HEXAGON_Y;
static double curr_x_axis = HEXAGON_X_AXIS;
static double curr_y_axis = HEXAGON_Y_AXIS;

static double scaling_factor = 1;
static int rotation_step = 0;

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

void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case KEY_ESCAPE:
        exit(0);
        break;
    case KEY_LEFT:
        printf("go left\n");
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
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

static void on_timer(int value)
{
    if (value != TIMER_ID)
        return;

    glutPostRedisplay();

    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}
void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3f(0, 0, 1);
    glLoadIdentity();
    gluLookAt (
        0.1, 0.2, -0.3, 
        0.0, 0.0, 0.0, 
        0.0, 2.0, 0.0
    );
    
    glRotatef(rotation_step, 0, 0, 1);
    glScalef(scaling_factor, scaling_factor, scaling_factor);

    glBegin(GL_LINE_LOOP);
        glVertex3f(curr_x_axis,  curr_y_axis,   0);
        glVertex3f(curr_x,       curr_y,        0);
        glVertex3f(curr_x,      -curr_y,        0);
        glVertex3f(curr_x_axis, -curr_y_axis,   0);
        glVertex3f(-curr_x,     -curr_y,        0);
        glVertex3f(-curr_x,      curr_y,        0);
    glEnd();

    rotation_step += HEXAGON_ROTATE_STEP;
    scaling_factor *= HEXAGON_SCALING_FACTOR;

    printf("x_curr = %lf\ny_curr=%lf\n", curr_x, curr_y);
    printf("x_curr_axis = %lf\ny_curr_axis=%lf\n", curr_x_axis, curr_y_axis);


    

    glutSwapBuffers();
}
