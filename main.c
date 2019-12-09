#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_WIDTH (1000)
#define WINDOW_HEIGHT (1000)
#define WINDOW_POS_X (1080 / 2)
#define WINDOW_POS_Y (1920 / 2)

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
#define HEXAGON_STARTING_SCALE_FACTOR (2)

#define EPSILON (0.3)

#define NUMBER_OF_HEXAGONS (4)

static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void drawHexagon();
static void updateScalingFactors();
static void drawAllHexagons();
static double get_scaling_factor_update();
static void update_scaling();


static int window_width, window_height;
static int animation_ongoing;

static double scaling_factor = 1;
static double rotation_step = 0;
static double rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;

static double current_scaling_factor = HEXAGON_SCALING_FACTOR;
static double hexagon_scaling_factors[NUMBER_OF_HEXAGONS] = {2.0, 1.5, 1.0, 0.50};

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);

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

        rotation_direction = HEXAGON_NEGATIVE_ROTATION_DIRECTION;
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;
    
    case KEY_RIGHT:
        printf("go right\n");
        rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;
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
    
    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60, 
        window_width / (float)window_height,
        1, 
        5
    );
    
    glLoadIdentity();
    gluLookAt (
        0.1, 0.2, -0.3, 
        0.0, 0.0, 0.0, 
        0.0, 2.0, 0.0
    );

    glColor3f(0, 0, 1);

    glRotatef(rotation_step * rotation_direction, 0, 0, 1);
    glScalef(scaling_factor, scaling_factor, scaling_factor);

    drawAllHexagons();

    rotation_step += HEXAGON_ROTATION_STEP;
    updateScalingFactors();
    

    printf("rotation_direction: %lf\n", rotation_direction);

    glutSwapBuffers();
}

static void drawHexagon(int hexagon_idx)
{
    printf("drawing hexagon with idx: %d\n", hexagon_idx);
    
    double scale_factor = hexagon_scaling_factors[hexagon_idx];

    glPushMatrix();
        glScalef(scale_factor, scale_factor, scale_factor); 
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

static void updateScalingFactors() 
{
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        if(hexagon_scaling_factors[i] < EPSILON) {
            hexagon_scaling_factors[i] = HEXAGON_STARTING_SCALE_FACTOR;
        }
        else {
            hexagon_scaling_factors[i] *= HEXAGON_SCALING_FACTOR;
        }
    }
}

static void drawAllHexagons() 
{
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        drawHexagon(i);
    }
}
static double get_scaling_factor_update()
{   
    double scaling_factor = fabs(hexagon_scaling_factors[0] - hexagon_scaling_factors[1]);
    printf("Scaling factor: %lf\n", scaling_factor);
    return scaling_factor;
}

static void update_scaling() 
{   
    // FIXME
    double scale_x = current_scaling_factor;
    printf("Scaling factor for x: %lf\n", scale_x);
    for(int i = 0; i < NUMBER_OF_HEXAGONS-1; i++) {
        double scale_y = 
            (hexagon_scaling_factors[i] * scale_x - 
            (hexagon_scaling_factors[i] - hexagon_scaling_factors[i+1])
            ) / hexagon_scaling_factors[i+1];
        hexagon_scaling_factors[i] *= scale_x;
        scale_x = scale_y;
    }
    current_scaling_factor *= HEXAGON_SCALING_FACTOR;

}