/**
 * @file camera.cpp
 * @brief "*Need to be Added*"
*/

#include "camera.hpp"

Camera::Camera() {
    // Constructor definition if needed
}

void Camera::processMouseInput(GLFWwindow* window, int& prevLeftButtonState, double& oldX, double& oldY, double& currentX, double& currentY, bool& isDragging) {
    // Get current mouse position
    int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_RELEASE) {
        isDragging = true;
        currentX = oldX = x;
        currentY = oldY = y;
    } else if (leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_PRESS) {
        currentX = x;
        currentY = y;
    } else if (leftButtonState == GLFW_RELEASE && prevLeftButtonState == GLFW_PRESS) {
        isDragging = false;
    }
    prevLeftButtonState = leftButtonState;
}

void Camera::rotateModel(int mode, double oldX, double oldY, double currentX, double currentY, bool isDragging, glm::mat4& modelT, GLint vModel_uniform, glm::mat4 viewT) {
    float angle = 0.0f;
    if (mode == 0) {
        if (isDragging && (currentX != oldX || currentY != oldY)) {
            glm::vec3 va = getTrackBallVector(oldX, oldY);
            glm::vec3 vb = getTrackBallVector(currentX, currentY);

            angle = acos(std::min(1.0f, glm::dot(va, vb)));
            glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
            glm::mat3 camera2object = glm::inverse(glm::mat3(viewT * modelT));
            glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
            modelT = glm::rotate(modelT, angle, axis_in_object_coord);
            glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

            oldX = currentX;
            oldY = currentY;
        }
    } else if (mode == 1) {
        angle += 0.05;
        modelT = glm::rotate(modelT, angle, glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
    }
}

void Camera::setupModelTransformation(unsigned int &program) {
    //Modelling transformations (Model -> World coordinates)
    modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)); // Model coordinates are the world coordinates

    // Pass on the modelling matrix to the vertex shader
    glUseProgram(program);
    vModel_uniform = glGetUniformLocation(program, "vModel");
    if (vModel_uniform == -1) {
        fprintf(stderr, "Could not bind location: vModel\n");
        exit(0);
    }
    glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void Camera::setupViewTransformation(unsigned int &program) {
    // Viewing transformations (World -> Camera coordinates)
    // Camera at (40, 20, 40) in a right-handed coordinate system
    viewT = glm::lookAt(glm::vec3(40.0, 20.0, 40.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    // Pass-on the viewing matrix to the vertex shader
    glUseProgram(program);
    vView_uniform = glGetUniformLocation(program, "vView");
    if (vView_uniform == -1) {
        fprintf(stderr, "Could not bind location: vView\n");
        exit(0);
    }
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void Camera::setupProjectionTransformation(unsigned int &program) {
    // Projection transformation
    projectionT = glm::perspective(45.0f, (GLfloat)screen_width / (GLfloat)screen_height, 0.1f, 1000.0f);

    // Pass on the projection matrix to the vertex shader
    glUseProgram(program);
    vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if (vProjection_uniform == -1) {
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}

glm::vec3 Camera::getTrackBallVector(double x, double y) {
    glm::vec3 p = glm::vec3(2.0 * x / screen_width - 1.0, 2.0 * y / screen_height - 1.0, 0.0); // Normalize to [-1, +1]
    p.y = -p.y; // Invert Y since screen coordinate and OpenGL coordinates have different Y directions.

    float mag2 = p.x * p.x + p.y * p.y;
    if (mag2 <= 1.0f)
        p.z = sqrtf(1.0f - mag2);
    else
        p = glm::normalize(p); // Nearest point, close to the sides of the trackball
    return p;
}

//..
