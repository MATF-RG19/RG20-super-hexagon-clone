#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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
#define HEXAGON_ROTATION_STEP (2)
#define HEXAGON_POSITIVE_ROTATION_DIRECTION (1)
#define HEXAGON_NEGATIVE_ROTATION_DIRECTION (-1)
#define HEXAGON_STARTING_SCALE_FACTOR (2)

#define ILLEGAL_EDGE (-1)

#define EPSILON (0.3)

#define NUMBER_OF_HEXAGONS (4)

static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void drawHexagon();
static void updateScalingFactors();
static void drawAllHexagons();
static double get_randomized_scaling_factor();
static void draw_partial_hexagon();


static int window_width, window_height;
static int animation_ongoing;

static double scaling_factor = 1;
static double rotation_step = 0;
static double rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;

static double hexagon_edge_length[NUMBER_OF_HEXAGONS] = {2.0, 1.5, 1.0, 0.50};
static int current_removed_edge[NUMBER_OF_HEXAGONS] = {2, 3, 1, ILLEGAL_EDGE};

int main(int argc, char** argv)
{
    srand(time(NULL));

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
    
    glutSwapBuffers();
}

static void drawHexagon(int hexagon_idx)
{
    double scale_factor = hexagon_edge_length[hexagon_idx];

    glPushMatrix();
        glScalef(scale_factor, scale_factor, scale_factor); 
        glBegin(GL_LINES);
            draw_partial_hexagon();
        glEnd();
    glPopMatrix();
}

static void draw_partial_hexagon()
{
    int idx_to_be_removed = rand() % 6;
    printf("idx to be removed: %d\n", idx_to_be_removed);
    int ver_num = 12;
    GLfloat vertices[12][3] = {
        // 0
        {HEXAGON_X_AXIS,  HEXAGON_Y_AXIS,           0},
        {HEXAGON_X,       HEXAGON_Y,               0},

        // 1
        {HEXAGON_X,       HEXAGON_Y,               0},
        {HEXAGON_X,       -HEXAGON_Y,               0},
        
        // 2
        {HEXAGON_X,       -HEXAGON_Y,               0},
        {HEXAGON_X_AXIS,  -HEXAGON_Y_AXIS,          0},
        
        // 3
        {HEXAGON_X_AXIS,  -HEXAGON_Y_AXIS,          0},
        {-HEXAGON_X,      -HEXAGON_Y,               0},
        
        // 4
        {-HEXAGON_X,      -HEXAGON_Y,               0},
        {-HEXAGON_X,      HEXAGON_Y,                0},

        // 5
        {-HEXAGON_X,      HEXAGON_Y,                0},
        {HEXAGON_X_AXIS,  HEXAGON_Y_AXIS,           0}
    };

    for(int i = 0; i < ver_num; i++) {

        // FIXME DOESNT WORK, IT NEEDS TO SAVE THE REMOVED EDGE AND TO REMOVE IT AGAIN IN THE NEXT ITERATION
        if(i % 2 == 0 && current_removed_edge[i/2] == ILLEGAL_EDGE) {
            current_removed_edge[i/2] = idx_to_be_removed;
        }
    }
    for(int i = 0; i < ver_num; i++) {
        if(current_removed_edge[i/2] != i) {
            glVertex3fv(vertices[i]);
        }
    }

}

static void updateScalingFactors() 
{
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        if(hexagon_edge_length[i] < EPSILON) {
            hexagon_edge_length[i] = get_randomized_scaling_factor();
            //current_removed_edge[i] = 0;
        }
        else {
            hexagon_edge_length[i] *= HEXAGON_SCALING_FACTOR;
        }
    }
}

static void drawAllHexagons() 
{
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        drawHexagon(i);
    }
}

static double get_randomized_scaling_factor()
{
    int lower = 20;
    int upper = 35;

    //? see: https://www.geeksforgeeks.org/generating-random-number-range-c/
    double scaling = (rand() % (upper - lower + 1)) + lower;
    scaling /= 10;

    printf("random scaling: %lf\n", scaling);

    return scaling;
}