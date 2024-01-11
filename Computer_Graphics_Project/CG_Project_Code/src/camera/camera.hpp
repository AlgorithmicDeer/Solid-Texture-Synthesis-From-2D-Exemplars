/**
 * @file camera.hpp
 * @brief "*Need to be Added*"
*/
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "utils/utils.hpp" // Include necessary dependencies
#include "globals/globals.h"

class Camera {
public:
    Camera(); // Constructor
    void processMouseInput(GLFWwindow* window, int& prevLeftButtonState, double& oldX, double& oldY, double& currentX, double& currentY, bool& isDragging);
    void rotateModel(int mode, double oldX, double oldY, double currentX, double currentY, bool isDragging, glm::mat4& modelT, GLint vModel_uniform, glm::mat4 viewT);
    void setupModelTransformation(unsigned int &program);
    void setupViewTransformation(unsigned int &program);
    void setupProjectionTransformation(unsigned int &program);
    glm::vec3 getTrackBallVector(double x, double y);
};

#endif // CAMERA_HPP
