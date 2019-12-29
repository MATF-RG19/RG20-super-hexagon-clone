#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "common.h"
#include "types.h"
#include "general_gl_inits.h"

//? Callback functions
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int value);

//? Surface drawing flow
void drawSurface();
void drawSurfaceForSingleHexagon(int idx);

//? All inits
void initHexagons();
void initAgent();

//? Hexagon drawing flow
void drawCurrentBoardState();
void drawAllHexagons();
void drawHexagon(int idx);
void drawPartialHexagon(int idx);

//? Hexagon resize flow
void updateScalingFactorsAndScore();
float getRandomizedScalingFactor(int idx);
int impossibleScaling(int idx, float scaling);
void rearrangeHexagons();
void checkForImpassableTerrain(); 
float calculateDistance(int idx0, int idx1);

void updateRotationStep();

void drawAgent();

void detectColission();

void determineRemovedEdge();

void displayCurrentStats();
void printText(char* text_to_be_displayed, float vertical_offset);

void displayGameOver();

int animation_ongoing;

float scaling_factor = 1;
int rotation_step = 0;
float rotation_direction = HEXAGON_POSITIVE_ROTATION_DIRECTION;
int rotation_is_active = 1;

int current_score = 0;
int number_of_lives = 3;

float hexagon_edge_length[NUMBER_OF_HEXAGONS] = {12.0, 6.0, 3.0, 1.0, 0.5}; 
int hexagons_idx_by_size[NUMBER_OF_HEXAGONS] = {0, 1, 2, 3, 4}; //? starting from the biggest hexagon

int using_flat_model = 1;
int already_detected_colission_for_current_hexagon = 0;

GLuint texture_names[2];

//? see: https://www.schemecolor.com/pastel-infinity-stones.php
GLfloat hexagon_colors[5][3] = {
    {224.0/255.0,           1,           1},
    {199.0/255.0, 206.0/255.0, 234.0/255.0},
    {1,           218.0/255.0, 193.0/255.0},
    {1,           154.0/255.0, 162.0/255.0},
    {1,             1,         216.0/255.0}
};  

point vertices[12] = {
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

point agent_pos[3] = {
    {0.025,  0,    0},
    {-0.025, 0,    0},
    {0,      0,   0.1}
};

Hexagon hexagons[NUMBER_OF_HEXAGONS];
Agent agent;

int main(int argc, char** argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    
    initHexagons();
    initAgent();
    initLightning();
    initMaterial();
    initTextures();    
    
    animation_ongoing = 0;

    glClearColor(181.0/255.0, 234.0/255.0, 215.0/255.0, 0.0);
    glLineWidth(2);

    glutMainLoop();
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

void initAgent() {
    agent.agent_pos = agent_pos;
}

static void on_keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case KEY_ESCAPE:
        glDeleteTextures(2, texture_names);
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
        if(!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;
    
    case KEY_CHANGE_SHADE_MODEL:
        if(!using_flat_model) {
            glShadeModel(GL_FLAT);
            using_flat_model = 1;
        }
        else {
            glShadeModel(GL_SMOOTH);
            using_flat_model = 0;
        }
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

    if(number_of_lives == 0) {
        displayGameOver();
        return;
    }
    drawCurrentBoardState();
    drawAgent();
    displayCurrentStats();

    updateRotationStep();
    updateScalingFactorsAndScore();
    detectColission();
    
    glutSwapBuffers();
}

void drawCurrentBoardState() {
    glPushMatrix();
        glRotatef(rotation_step, 0, 1, 0);
        glScalef(scaling_factor, scaling_factor, scaling_factor);
        drawSurface();
        drawAllHexagons();
        determineRemovedEdge();
    glPopMatrix();
}

void drawAllHexagons() {
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        drawHexagon(hexagons_idx_by_size[i]);
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

            hexagons[i].scaling_factor = getRandomizedScalingFactor(i);

            //? reset removed edges, so every time hexagon is rescaled new
            //? random point is choosen
            hexagons[i].removed_edge_index_1 = ILLEGAL_VALUE;
            hexagons[i].removed_edge_index_2 = ILLEGAL_VALUE;
            
            rearrangeHexagons();
            checkForImpassableTerrain();
            already_detected_colission_for_current_hexagon = 0;
        }
        else {
            hexagons[i].scaling_factor *= HEXAGON_SCALING_FACTOR;
        }
    }
}

float getRandomizedScalingFactor(int idx) {

     //? see: https://www.geeksforgeeks.org/generating-random-number-range-c/
    float scaling = 0;

    //? we need to make sure that newly rescaled hexagon has the biggest size, cuz we don't want
    //? to have a situation where the newly rescaled hexagons is not the biggest in order to avoid all sorts off bugs and complications
    while(impossibleScaling(idx, scaling)) {
        scaling = (rand() % (UPPER_LIMIT - LOWER_LIMIT + 1)) + LOWER_LIMIT;
        scaling /= 10;
    }

    return scaling;
}

int impossibleScaling(int idx, float scaling) {
    for (int j = 0; j < NUMBER_OF_HEXAGONS; j++) {
        if(idx != j && hexagons[j].scaling_factor > scaling) {
            return 1;
        }
    }
    return 0;
}

void rearrangeHexagons() {
    //? we are just updating the order of the hexagons, starting from the biggest, since we know that 
    //? when the hexagon rescales it most become the biggest of them all, so we want to update our ordering
    int tmp_arr[NUMBER_OF_HEXAGONS];

    for (int i = 0; i < NUMBER_OF_HEXAGONS-1; i++) {
        tmp_arr[i+1] = hexagons_idx_by_size[i];
    }
    tmp_arr[0] = hexagons_idx_by_size[NUMBER_OF_HEXAGONS-1];
    
    for (int i = 0; i < NUMBER_OF_HEXAGONS; i++) {
        hexagons_idx_by_size[i] = tmp_arr[i];
    }
}

void checkForImpassableTerrain() {
    //? these will always be the 2 biggest hexagons
    int hex0_idx = hexagons_idx_by_size[0];
    int hex1_idx = hexagons_idx_by_size[1];

    float distance = calculateDistance(hex0_idx, hex1_idx);

    if (distance < MIN_DISTANCE_BETWEEN_HEXAGONS) {
        hexagons[hex0_idx].removed_edge_index_1 = hexagons[hex1_idx].removed_edge_index_1;
        hexagons[hex0_idx].removed_edge_index_2 = hexagons[hex1_idx].removed_edge_index_2;
    }
}

float calculateDistance(int idx0, int idx1) {    
    float idx0_edge = hexagons[idx0].vertices[0][2] * hexagons[idx0].scaling_factor;
    float idx1_edge = hexagons[idx1].vertices[0][2] * hexagons[idx1].scaling_factor;

    float d = fabsf(idx0_edge - idx1_edge);
    return d;
}

void updateRotationStep() {
    rotation_step += HEXAGON_ROTATION_STEP * rotation_direction * rotation_is_active;
    rotation_step = rotation_step % 360;
}

void drawAgent() {
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture_names[0]);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
            glColor3f(0, 1, 0);

            glTexCoord2f(0, 0);
            glVertex3fv(agent.agent_pos[0]);

            glTexCoord2f(1, 0);
            glVertex3fv(agent.agent_pos[1]);
            
            glTexCoord2f(0.5, 1);
            glVertex3fv(agent.agent_pos[2]);

        glBindTexture(GL_TEXTURE_2D, 0);        
        glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void detectColission() {
    int nearest_idx = hexagons_idx_by_size[NUMBER_OF_HEXAGONS-1];
    Hexagon current_hexagon = hexagons[nearest_idx];
    
    //? Only check the tip of the agent, that is the third coordinate in the array.
    float agent_z = agent.agent_pos[2][2];

    int right_angle = current_hexagon.removed_edge * 60;
    int left_angle = (current_hexagon.removed_edge + 1) * 60;

    //? checking if we are passing through the edge knowing how much hexagons needed to rotate
    //? checks are both for clockwise and counterclockwise rotations
    int goes_through_removed_edge = 0;
    if(right_angle <= rotation_step && rotation_step <= left_angle) {
        goes_through_removed_edge = 1;
    }
    if(right_angle - 360 <= rotation_step && rotation_step <= left_angle - 360) {
        goes_through_removed_edge = 1;
    }

    //? We have normalized coord system so we can do this
    float hexagon_y = hexagons[nearest_idx].scaling_factor;

    if (fabsf (agent_z - hexagon_y) <= COLLISION_SAFE_DISTANCE && !goes_through_removed_edge) {
        if(!already_detected_colission_for_current_hexagon) {
            number_of_lives--;
            already_detected_colission_for_current_hexagon = 1;
        }
    }
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

void displayGameOver() {    
    float vertical_offset = 0.03;

    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture_names[1]);
        glEnable(GL_TEXTURE_2D);
        glRotatef(90, 0, 1, 0);
        glTranslatef(-0.2, 0, 0);
        glBegin(GL_POLYGON);

            glTexCoord2f(vertical_offset, 1);
            glVertex3f(-1, 0, 1);

            glTexCoord2f(vertical_offset, vertical_offset);
            glVertex3f(1, 0, 1);

            glTexCoord2f(1, vertical_offset);
            glVertex3f(1, 0, -1);

            glTexCoord2f(1, 1);
            glVertex3f(-1, 0, -1);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);        
        glEnd();
    glPopMatrix();
        
    glutSwapBuffers();
}