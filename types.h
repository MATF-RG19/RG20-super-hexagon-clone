#ifndef TYPES_H
#define TYPES_H
#include <GL/glut.h>

typedef GLfloat point[3];

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

#endif 