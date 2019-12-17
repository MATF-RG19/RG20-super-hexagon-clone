#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WINDOW_WIDTH (1920)
#define WINDOW_HEIGHT (1000)
#define WINDOW_POS_X (1080 / 2)
#define WINDOW_POS_Y (1920 / 2)

#define TIMER_ID (0)
#define TIMER_INTERVAL (17) // ~60 rotations per second

#define KEY_ESCAPE (27)
#define KEY_LEFT ('a')
#define KEY_RIGHT ('d')
#define KEY_STOP ('s')

#define HEXAGON_X (0.87) // ~sqrt(3) / 2
#define HEXAGON_Y (0.5)
#define HEXAGON_X_AXIS (0.0)
#define HEXAGON_Y_AXIS (1.0)
#define HEXAGON_SCALING_FACTOR (0.995)
#define HEXAGON_ROTATION_STEP (1)
#define HEXAGON_POSITIVE_ROTATION_DIRECTION (1)
#define HEXAGON_NEGATIVE_ROTATION_DIRECTION (-1)
#define HEXAGON_STARTING_SCALE_FACTOR (2)


#define LOWER_LIMIT (20)
#define UPPER_LIMIT (35)

#define ILLEGAL_EDGE (-1)
#define NO_DISTANCE (-1)
#define ILLEGAL_VALUE (-1)
#define MIN_DISTANCE (0.5)
#define SAFE_DISTANCE (0.015)

#define EPSILON (0.095)

#define NUMBER_OF_HEXAGONS (4)

static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);

static void initHexagons();
static void initAgent();

static void drawHexagon();
static void updateScalingFactors();
static void drawAllHexagons();
static float getRandomizedScalingFactor();
static void drawPartialHexagon();
static void drawAllHexagons();
static void rearrangeHexagons();
static void checkForImpassableTerrain(); 
static void drawAgent();

static float calculateDistance(int idx0, int idx1);
static void detectColission();

static int window_width, window_height;
static int animation_ongoing;

static float scaling_factor = 1;
static int rotation_step = 0;
static float rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;


static float hexagon_edge_length[NUMBER_OF_HEXAGONS] = {2.0, 1.5, 1.0, 0.50}; 
static int hexagons_idx_by_size[NUMBER_OF_HEXAGONS] = {0, 1, 2, 3}; // starting from the biggest hexagon

typedef GLfloat point[3];

static point vertices[12] = {
        // 0
        {HEXAGON_X_AXIS,  HEXAGON_Y_AXIS,           0},
        {HEXAGON_X,       HEXAGON_Y,                0},
        // 1
        {HEXAGON_X,       HEXAGON_Y,                0},
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

static point agent_pos[3] = {
    {0.025,  0, 0},
    {-0.025, 0, 0},
    {0,  0.1, 0},
};

typedef struct {
    point* vertices;
    int index;
    float distances[3]; 
    int removed_edge_index_1;
    int removed_edge_index_2;
    float scaling_factor;
} Hexagon;

typedef struct {
    point* agent_pos;
} Agent;

static Hexagon hexagons[NUMBER_OF_HEXAGONS];
static Agent agent;

int main(int argc, char** argv)
{
    initHexagons();
    initAgent();
    
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
        rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;
    
    case KEY_RIGHT:
        rotation_direction = HEXAGON_NEGATIVE_ROTATION_DIRECTION;
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;
    case KEY_STOP:
        animation_ongoing = 0;
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

    glPushMatrix();
    glRotatef(rotation_step, 0, 0, 1);
    glScalef(scaling_factor, scaling_factor, scaling_factor);
    drawAllHexagons();
    glPopMatrix();

    drawAgent();

    rotation_step += HEXAGON_ROTATION_STEP * rotation_direction;
    rotation_step = rotation_step % 360;
    updateScalingFactors();
    detectColission();
    
    glutSwapBuffers();
}

static void drawHexagon(int hexagon_idx)
{
    float scale_factor = hexagons[hexagon_idx].scaling_factor;

    glPushMatrix();
        glScalef(scale_factor, scale_factor, scale_factor); 
        glBegin(GL_LINES);
            drawPartialHexagon(hexagon_idx);
        glEnd();
    glPopMatrix();
}

static void drawPartialHexagon(int hexagon_idx)
{
    int ver_num = 12;

    //? make sure to choose even number to be first, since every point starts
    //? with even index 
    int no_draw_1 = rand() % 12;
    if (no_draw_1 % 2 != 0) {
        if(no_draw_1 != 11) {
            no_draw_1++;
        }
        else {
            no_draw_1 = 10;
        }
    }

    int no_draw_2 = no_draw_1 + 1;

    if(hexagons[hexagon_idx].removed_edge_index_1 == ILLEGAL_VALUE) {
        hexagons[hexagon_idx].removed_edge_index_1 = no_draw_1;
        hexagons[hexagon_idx].removed_edge_index_2 = no_draw_2;
    }

    for (int i = 0; i < ver_num; i++) {
        if(i != hexagons[hexagon_idx].removed_edge_index_1 && i != hexagons[hexagon_idx].removed_edge_index_2) {
            glVertex3fv(hexagons[hexagon_idx].vertices[i]);
        }
    }
}

static void updateScalingFactors() 
{
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        if(hexagons[i].scaling_factor < EPSILON) {
            hexagons[i].scaling_factor = getRandomizedScalingFactor();
            
            //? reset removed edges, so every time hexagon is rescaled new 
            //? random point is choosen
            hexagons[i].removed_edge_index_1 = ILLEGAL_VALUE;
            hexagons[i].removed_edge_index_2 = ILLEGAL_VALUE;
            
            rearrangeHexagons();
            checkForImpassableTerrain();
        }
        else {
            hexagons[i].scaling_factor *= HEXAGON_SCALING_FACTOR;
        }
    }
}

static void drawAllHexagons() 
{
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        drawHexagon(i);
    }
}

static float getRandomizedScalingFactor()
{
    //? see: https://www.geeksforgeeks.org/generating-random-number-range-c/
    float scaling = (rand() % (UPPER_LIMIT - LOWER_LIMIT + 1)) + LOWER_LIMIT;
    scaling /= 10;

    return scaling;
}

static void drawAgent() {
    glPushMatrix();
        glBegin(GL_TRIANGLES);
            glColor3f(0, 1, 0);
            for (int i = 0; i < 3; i++) {
                glVertex3fv(agent.agent_pos[i]);
            }
        glEnd();
    glPopMatrix();
}

static void initHexagons()
{
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        hexagons[i].vertices = vertices;
        hexagons[i].index = i;
        
        for(int j = 0; j < NUMBER_OF_HEXAGONS-1; j++) {
            hexagons[i].distances[j] = NO_DISTANCE;
        }
        
        hexagons[i].removed_edge_index_1 = ILLEGAL_VALUE;
        hexagons[i].removed_edge_index_2 = ILLEGAL_VALUE;
        hexagons[i].scaling_factor = hexagon_edge_length[i];
    } 
}

static void rearrangeHexagons() {
    // SHR
    int tmp_arr[NUMBER_OF_HEXAGONS];

    for (int i = 0; i < NUMBER_OF_HEXAGONS-1; i++) {
        tmp_arr[i+1] = hexagons_idx_by_size[i];
    }
    tmp_arr[0] = hexagons_idx_by_size[NUMBER_OF_HEXAGONS-1];
    
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        hexagons_idx_by_size[i] = tmp_arr[i];
    }

    printf("Current order of hexagons: \n");
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        printf("%d ", hexagons_idx_by_size[i]);
    }
    printf("\n");
}

static void checkForImpassableTerrain() {
    int hex0_idx = hexagons_idx_by_size[0];
    int hex1_idx = hexagons_idx_by_size[1];

    printf("Checking %d and %d\n", hex0_idx, hex1_idx);

    float distance = calculateDistance(hex0_idx, hex1_idx);

    if (distance < MIN_DISTANCE) {
        hexagons[hex0_idx].removed_edge_index_1 = hexagons[hex1_idx].removed_edge_index_1;
        hexagons[hex0_idx].removed_edge_index_2 = hexagons[hex1_idx].removed_edge_index_2;
    }
}

static void initAgent() {
    agent.agent_pos = agent_pos;
}

static float calculateDistance(int idx0, int idx1) {    
    float idx0_edge = hexagons[idx0].vertices[0][1] * hexagons[idx0].scaling_factor;
    float idx1_edge = hexagons[idx1].vertices[0][1] * hexagons[idx1].scaling_factor;

    float d = fabsf(idx0_edge - idx1_edge);
    printf("Distance between %d and %d is %f\n", idx0, idx1, d);

    return d;
}

//TODO change to bool at some point
static void detectColission() {
    int nearest_idx = hexagons_idx_by_size[NUMBER_OF_HEXAGONS-1];

    //? Only check the tip of the agent, that is the third coordinate in the array.
    float agent_y = agent.agent_pos[2][1];

    //? We have normalized coord system so we can do this
    float hexagon_y = hexagons[nearest_idx].scaling_factor;

    int colission_detected = 0;

    if (fabsf (agent_y - hexagon_y) <= SAFE_DISTANCE) {
        printf("Colission detected\n");
        colission_detected = 1;
    }

    // return colission_detected
}