/**
 * @file globals.h
 * @brief "*Need to be Added*"
*/
#ifndef GLOBALS_H
#define GLOBALS_H

#include "utils/utils.hpp"

extern int screen_width;
extern int screen_height;
extern GLint vModel_uniform;
extern GLint vView_uniform;
extern GLint vProjection_uniform;
extern GLint lpos_world_uniform;
extern GLint eye_normal_uniform;
extern glm::mat4 modelT;
extern glm::mat4 viewT;
extern glm::mat4 projectionT;
extern double oldX;
extern double oldY;
extern double currentX;
extern double currentY;
extern bool isDragging;
extern GLuint nVertices;
extern float scale;
extern unsigned int texture,normalMap;
extern int width, height, nrChannels;
#endif

