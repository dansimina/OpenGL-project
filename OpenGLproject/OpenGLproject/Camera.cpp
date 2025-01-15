#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        updateCameraVectors();
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 cameraTargetAux = cameraTarget;

        switch (direction)
        {
        case gps::MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
            break;
        case gps::MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
            break;
        case gps::MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
            break;
        case gps::MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
            break;
        default:
            break;
        }

        if (cameraPosition.y <= 0.2) {
            cameraPosition.y = 0.2;
            cameraTarget.y = cameraTargetAux.y;
        }

        //printf("pos %f %f %f\n", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        cameraTarget.x = cameraPosition.x + sin(yaw);
        cameraTarget.y = cameraPosition.y + sin(-pitch);
        cameraTarget.z = cameraPosition.z - cos(yaw) * cos(-pitch);

        updateCameraVectors();
    }

    void Camera::updateCameraVectors()
    {
        const glm::vec3 up(0.0f, 1.0f, 0.0f);
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, up));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    void Camera::setCamera(glm::vec3 cameraPosition, glm::vec3 cameraTarget) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        updateCameraVectors();
    }
}