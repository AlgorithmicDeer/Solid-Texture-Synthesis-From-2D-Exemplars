/**
 * @file globals.cpp
 * @brief "*Need to be Added*"
*/

#include "globals.h"

int screen_width = 1000;
int screen_height = 1000;
GLint vModel_uniform;
GLint vView_uniform;
GLint vProjection_uniform;
GLint lpos_world_uniform;
GLint eye_normal_uniform;
glm::mat4 modelT;
glm::mat4 viewT;
glm::mat4 projectionT;
double oldX =0;
double oldY = 0;
double currentX = 0;
double currentY = 0;
bool isDragging = false;
GLuint nVertices;
float scale = 1;
unsigned int texture ,normalMap;
int width, height, nrChannels;