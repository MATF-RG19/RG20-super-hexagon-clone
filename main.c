#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "image.h"

#define WINDOW_WIDTH (1920)
#define WINDOW_HEIGHT (1000)
#define WINDOW_POS_X (1080 / 2)
#define WINDOW_POS_Y (1920 / 2)

#define CAMERA_FOV (45)

#define TIMER_ID (0)
#define TIMER_INTERVAL (17) //? ~60fps

#define KEY_ESCAPE (27)
#define KEY_LEFT ('a')
#define KEY_RIGHT ('d')
#define KEY_STOP ('s')
#define KEY_PAUSE_ROTATION (' ')

#define HEXAGON_X (0.87) //? ~sqrt(3) / 2
#define HEXAGON_Z (0.5)
#define HEXAGON_X_AXIS (0.0)
#define HEXAGON_Z_AXIS (1.0)
#define HEXAGON_SCALING_FACTOR (0.996)
#define HEXAGON_ROTATION_STEP (1)
#define HEXAGON_POSITIVE_ROTATION_DIRECTION (1)
#define HEXAGON_NEGATIVE_ROTATION_DIRECTION (-1)
#define HEXAGON_STARTING_SCALE_FACTOR (2)
#define LOWER_LIMIT (55)
#define UPPER_LIMIT (85)

#define ILLEGAL_EDGE (-1)
#define NO_DISTANCE (-1)
#define ILLEGAL_VALUE (-1)
#define MIN_DISTANCE (0.5)
#define SAFE_DISTANCE (0.015)
#define EPSILON (0.095)

#define NUMBER_OF_HEXAGONS (5)

#define TEXT_POS_X (1.875)
#define TEXT_POS_Z (1.225)
#define TEXT_VERTICAL_OFFSET (0.075)

#define TEXTURE_AGENT "wall.bmp"

static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);
void drawAxis(float len);
void drawHelpBody();

void drawSurface();
void drawSurfaceForSingleHexagon(int idx);

void initHexagons();
void initAgent();

void drawHexagon();
void updateScalingFactorsAndScore();
void updateRotationStep();
void drawAllHexagons();
float getRandomizedScalingFactor();
void drawPartialHexagon();
void drawAllHexagons();
void rearrangeHexagons();
void checkForImpassableTerrain(); 

void drawAgent();

float calculateDistance(int idx0, int idx1);
void detectColission();
void determineRemovedEdge();

void displayCurrentStats();
void printText(char* text_to_be_displayed, float vertical_offset);

static int animation_ongoing;

static float scaling_factor = 1;
static int rotation_step = 0;
static float rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;
static int rotation_is_active = 1;

static int current_score = 0;
static int number_of_lives = 3;

static float hexagon_edge_length[NUMBER_OF_HEXAGONS] = {5.0, 4.0, 3.0, 2.0, 1.0}; 
static int hexagons_idx_by_size[NUMBER_OF_HEXAGONS] = {0, 1, 2, 3, 4}; // starting from the biggest hexagon

static GLuint names[1];

static GLfloat hexagon_colors[5][3] = {
    {224.0/255.0, 1, 1},
    {199.0/255.0, 206.0/255.0, 234.0/255.0},
    {1, 218.0/255.0, 193.0/255.0},
    {1, 154./255.0, 162.0/255.0},
    {1, 1, 216.0/255.0}
};  

typedef GLfloat point[3];

static point vertices[12] = {
        // 0
        {HEXAGON_X_AXIS, 0,  HEXAGON_Z_AXIS           },
        {HEXAGON_X,      0,  HEXAGON_Z                },
        // 1
        {HEXAGON_X,      0,  HEXAGON_Z                },
        {HEXAGON_X,       0, -HEXAGON_Z               },
        // 2
        {HEXAGON_X,       0, -HEXAGON_Z               },
        {HEXAGON_X_AXIS,  0, -HEXAGON_Z_AXIS          },
        // 3
        {HEXAGON_X_AXIS,  0, -HEXAGON_Z_AXIS          },
        {-HEXAGON_X,      0, -HEXAGON_Z               },
        // 4
        {-HEXAGON_X,      0, -HEXAGON_Z               },
        {-HEXAGON_X,     0,  HEXAGON_Z                },
        // 5
        {-HEXAGON_X,     0,  HEXAGON_Z                },
        {HEXAGON_X_AXIS, 0,  HEXAGON_Z_AXIS           }
    };

static point agent_pos[3] = {
    {0.025,  0, 0},
    {-0.025, 0, 0},
    {0,  0, 0.1},
};

typedef struct {
    point* vertices;
    int index;
    float distances[3]; 
    int removed_edge_index_1;
    int removed_edge_index_2;
    float scaling_factor;
    int left_angle;
    int right_angle;
    int removed_edge;
} Hexagon;

typedef struct {
    point* agent_pos;
} Agent;

static Hexagon hexagons[NUMBER_OF_HEXAGONS];
static Agent agent;

int main(int argc, char** argv) {
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
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float light_position[] = {0, 2, 1, 1};
    float light_ambient[] = {.3f, .3f, .3f, 1};
    float light_diffuse[] = {.7f, .7f, .7f, 1};
    float light_specular[] = {.7f, .7f, .7f, 1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    GLfloat ambient[] = {0.3,0.3,0.3,0};
    GLfloat diffuse[] = {0,0.7,0,0};
    GLfloat specular[] = {0.6,0.6,0.6,0};
    GLfloat shininess = 40;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glEnable(GL_COLOR_MATERIAL);


    glEnable(GL_TEXTURE_2D);
    Image* image = image_init(0, 0);
    
    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /* Kreira se prva tekstura. */
    image_read(image, TEXTURE_AGENT);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(2, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);
    
    animation_ongoing = 0;

    glClearColor(181.0/255.0, 234.0/255.0, 215.0/255.0, 0.0);
    glLineWidth(2);

    glutMainLoop();
}

void drawHelpBody() {
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        glTranslatef(0, -2, 0);
        glColor3f(0, 0.8, 0);
        glutSolidCone(5, 0.001, 60, 60);
    glPopMatrix();
}

void drawSurface() {
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        drawSurfaceForSingleHexagon(hexagons_idx_by_size[i]);
    }
}

void drawSurfaceForSingleHexagon(int idx) {
    Hexagon hexagon = hexagons[idx];
    GLfloat coord_center[] = {0, 0, 0};
    GLfloat* color = hexagon_colors[idx];

    glPushMatrix();        
        glColor3fv(color); 
        glScalef(hexagon.scaling_factor, hexagon.scaling_factor, hexagon.scaling_factor);
        glBegin(GL_TRIANGLES);
            for(int i = 0; i < 11; i++) {
                glVertex3fv(coord_center);
                glVertex3fv(hexagon.vertices[i]);
                glVertex3fv(hexagon.vertices[i+1]);
            }
        glEnd();
    glPopMatrix();
}

void drawAxis(float len) {
    glPushMatrix();
        glDisable(GL_LIGHTING);
            glBegin(GL_LINES);
                glColor3f(1,0,0);
                glVertex3f(0,0,0);
                glVertex3f(len,0,0);

                glColor3f(0,1,0);
                glVertex3f(0,0,0);
                glVertex3f(0,len,0);

                glColor3f(0,0,1);
                glVertex3f(0,0,0);
                glVertex3f(0,0,len);
            glEnd();
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

static void on_keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case KEY_ESCAPE:
        glDeleteTextures(2, names);
        exit(0);
        break;

    case KEY_LEFT:
        rotation_is_active = 1;
        rotation_direction = HEXAGON_NEGATIVE_ROTATION_DIRECTION;
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;
    
    case KEY_RIGHT:
        rotation_is_active = 1;
        rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;

    case KEY_PAUSE_ROTATION:
        rotation_is_active = !rotation_is_active;
        break;

    case KEY_STOP:
        animation_ongoing = 0;
        break;
    }
}

static void on_reshape(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        CAMERA_FOV, 
        (float) width / height,
        1, 
        5
    );    
}

static void on_timer(int value) {
    if (value != TIMER_ID)
        return;

    glutPostRedisplay();

    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt (
        0, 2, -1, 
        0, 0, 0, 
        0, 1, 0
    );
    drawAxis(10);

    glPushMatrix();
        glRotatef(rotation_step, 0, 1, 0);
        glScalef(scaling_factor, scaling_factor, scaling_factor);
        drawSurface();
        drawAllHexagons();
        determineRemovedEdge();
    glPopMatrix();

    drawAgent();
    displayCurrentStats();

    
    updateRotationStep();
    updateScalingFactorsAndScore();
    detectColission();
    
    glutSwapBuffers();
}

void drawAllHexagons() {
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        drawHexagon(i);
    }
}

void drawHexagon(int hexagon_idx) {
    float scale_factor = hexagons[hexagon_idx].scaling_factor;

    glPushMatrix();
        glScalef(scale_factor, scale_factor, scale_factor); 
        glBegin(GL_LINES);
            glColor3f(0, 0, 0);
            glLineWidth(10);
            drawPartialHexagon(hexagon_idx);
        glEnd();
    glPopMatrix();
}

void drawPartialHexagon(int hexagon_idx) {
    int ver_num = 12;

    //? make sure to choose even number to be first, since every point starts
    //? with even index 
    int left_edges[6] = {0, 2, 4, 6, 8, 10};
    int random_idx = rand() % 6;
    
    int no_draw_1 = left_edges[random_idx];
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

void updateScalingFactorsAndScore() {
    
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        if(hexagons[i].scaling_factor < EPSILON) {

            //? We are updating the score cuz this means we've passed 1 hexagon
            current_score++;

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

void updateRotationStep() {
    rotation_step += HEXAGON_ROTATION_STEP * rotation_direction * rotation_is_active;
    rotation_step = rotation_step % 360;
}

float getRandomizedScalingFactor() {
    //? see: https://www.geeksforgeeks.org/generating-random-number-range-c/
    float scaling = (rand() % (UPPER_LIMIT - LOWER_LIMIT + 1)) + LOWER_LIMIT;
    scaling /= 10;

    return scaling;
}

void drawAgent() {
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, names[0]);
        glBegin(GL_TRIANGLES);
            glColor3f(0, 1, 0);
            for (int i = 0; i < 3; i++) {
                glVertex3fv(agent.agent_pos[i]);
                glTexCoord3fv(agent.agent_pos[i]);
            }
        glBindTexture(GL_TEXTURE_2D, 0);        
        glEnd();
    glPopMatrix();
}

void initHexagons() {
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        hexagons[i].vertices = vertices;
        hexagons[i].index = i;
        
        for(int j = 0; j < NUMBER_OF_HEXAGONS-1; j++) {
            hexagons[i].distances[j] = NO_DISTANCE;
        }
        
        hexagons[i].removed_edge_index_1 = ILLEGAL_VALUE;
        hexagons[i].removed_edge_index_2 = ILLEGAL_VALUE;
        hexagons[i].scaling_factor = hexagon_edge_length[i];
        
        hexagons[i].left_angle = ILLEGAL_VALUE;
        hexagons[i].right_angle = ILLEGAL_VALUE;
    }
}

void rearrangeHexagons() {
    //? SHR
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

void checkForImpassableTerrain() {
    int hex0_idx = hexagons_idx_by_size[0];
    int hex1_idx = hexagons_idx_by_size[1];

    printf("Checking %d and %d\n", hex0_idx, hex1_idx);

    float distance = calculateDistance(hex0_idx, hex1_idx);
    printf("current distance: %f\n", distance);

    if (distance < MIN_DISTANCE) {
        hexagons[hex0_idx].removed_edge_index_1 = hexagons[hex1_idx].removed_edge_index_1;
        hexagons[hex0_idx].removed_edge_index_2 = hexagons[hex1_idx].removed_edge_index_2;
    }
}

void initAgent() {
    agent.agent_pos = agent_pos;
}

float calculateDistance(int idx0, int idx1) {    
    float idx0_edge = hexagons[idx0].vertices[0][2] * hexagons[idx0].scaling_factor;
    float idx1_edge = hexagons[idx1].vertices[0][2] * hexagons[idx1].scaling_factor;

    float d = fabsf(idx0_edge - idx1_edge);
    printf("Distance between %d and %d is %f\n", idx0, idx1, d);

    return d;
}

//TODO change to bool at some point
void detectColission() {
    int nearest_idx = hexagons_idx_by_size[NUMBER_OF_HEXAGONS-1];
    Hexagon current_hexagon = hexagons[nearest_idx];
    
    // printf("current_removed_edge: %d\n", current_hexagon.removed_edge);

    //? Only check the tip of the agent, that is the third coordinate in the array.
    float agent_z = agent.agent_pos[2][2];

    int right_angle = current_hexagon.removed_edge * 60;
    int left_angle = (current_hexagon.removed_edge + 1) * 60;

    // printf("Right angle: %d, left angle: %d\n", right_angle, left_angle);
    // printf("Right angle reversed : %d, left angle reversed: %d\n", right_angle - 360, left_angle-360);
    // printf("rotation_step: %d\n", rotation_step);

    int goes_through_removed_edge = 0;
    if(right_angle <= rotation_step && rotation_step <= left_angle) {
        goes_through_removed_edge = 1;
    }
    if(right_angle - 360 <= rotation_step && rotation_step <= left_angle - 360) {
        goes_through_removed_edge = 1;
    }

    // printf("Goes through removed edge: %d\n", goes_through_removed_edge);

    //? We have normalized coord system so we can do this
    float hexagon_y = hexagons[nearest_idx].scaling_factor;

    int colission_detected = 0;

    if (fabsf (agent_z - hexagon_y) <= SAFE_DISTANCE && !goes_through_removed_edge) {
        // printf("Colission detected\n");
        colission_detected = 1;
    }

    // return colission_detected
}

void determineRemovedEdge() {
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {        
    switch (hexagons[i].removed_edge_index_1)
        {
        case  0:
            hexagons[i].removed_edge = 5;
            break;
        case  2:
            hexagons[i].removed_edge = 4;
            break;
        case  4:
            hexagons[i].removed_edge = 3;
            break;
        case  6:
            hexagons[i].removed_edge = 2;
            break;
        case  8:
            hexagons[i].removed_edge = 1;
            break;
        case  10:
            hexagons[i].removed_edge = 0;
            break;
        
        default:
            printf("No removed edges!\n");
            break;
        }
    }
}

void displayCurrentStats() {
    char display_current_score[64];
    char display_number_of_lives[64];
    
    sprintf(display_current_score, "Current Score: %d", current_score);
    sprintf(display_number_of_lives, "Number of lives: %d", number_of_lives);
    
    printText(display_current_score, 0);
    printText(display_number_of_lives, TEXT_VERTICAL_OFFSET);
}

void printText(char* text_to_be_displayed, float vertical_offset) {
    glPushMatrix();
        glColor3f(1, 1, 1);
        glRasterPos3f(TEXT_POS_X, 0, TEXT_POS_Z - vertical_offset);
        for(int i = 0; text_to_be_displayed[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text_to_be_displayed[i]);
        }
    glPopMatrix();
}

