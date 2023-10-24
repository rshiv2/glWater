#ifndef CAMERA_H
#define CAMERA_H

#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Common.hpp"

class Camera {
    public:
        Camera(const glm::vec3& position, const glm::vec3& up, const glm::vec3& front) 
            : _position(position), _up(up), _front(glm::normalize(front)) {
                _pitch = glm::degrees(glm::asin(_front.y)); 
                _pitch = common::clamp(_pitch, _minPitch, _maxPitch);
                _yaw = glm::degrees(glm::acos(_front.x / cos(glm::radians(_pitch))));
  
            }

        glm::mat4 lookAt() const {
            return glm::lookAt(_position, _position + _front, _up);
        }

        /* Return new camera reflected about a plane */
        Camera reflect(glm::vec4 plane) const {

            glm::vec3 planeNormal(plane);   // extract xyz components 

            glm::vec3 planeProj = _front - glm::dot(_front, planeNormal) * planeNormal;
            glm::vec3 reflectedDir = 2.0f * planeProj - _front;

            glm::vec3 pointOnPlane = glm::vec3(0.0f, 0.0f, plane.w == 0 ? 0.0f : -plane.w / plane.z);
            glm::vec3 rayCameraToPlane = _position - pointOnPlane;
            glm::vec3 reflectedPosition = _position - 2 * glm::dot(rayCameraToPlane, planeNormal) * planeNormal;

            Camera reflectedCamera = *this;
            reflectedCamera.setPosition(reflectedPosition);
            reflectedCamera.setUp(-_up);
            reflectedCamera.setFront(reflectedDir);

            return reflectedCamera;
        }

        /*** Getters ***/
        glm::vec3 getPosition() const {
            return _position;
        }

        glm::vec3 getUp() const {
            return _up;
        }

        glm::vec3 getFront() const {
            return _front;
        }

        float getFov() const {
            return _fov;
        }

        float getYaw() const {
            return _yaw;
        }

        float getPitch() const {
            return _pitch;
        }
    
        /*** Setters ***/
        void setPosition(const glm::vec3& v) {
            _position = v;
        }

        void setUp(const glm::vec3& v) {
            _up = v;
        }

        void setFront(const glm::vec3& v) {
            _front = glm::normalize(v);
        }

        void setFov(const float fov) {
            _fov = std::min(_maxFov, std::max(_minFov, fov));
        }

        void setYaw(const float yaw) {
            _yaw = yaw;
        }

        void setPitch(const float pitch) {
            _pitch = std::min(_maxPitch, std::max(_minPitch, pitch));
        }

        void setMoveSensitivity(const float s) {
            _moveSensitivity = std::max(s, 0.001f);
        }
        void scaleSpeed(const float speed) {
            _moveSensitivityScale = speed;
        }

        /*** Event listeners ***/
        void process_input(GLFWwindow* window) {

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS 
                || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
                    
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
                    _position += _moveSensitivity * _up;
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                    _position -= _moveSensitivity * _up;

            } 

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
                _position += _moveSensitivity * _moveSensitivityScale * glm::vec3(_front.x, 0.0f, _front.z);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                _position -= _moveSensitivity * _moveSensitivityScale * glm::vec3(_front.x, 0.0f, _front.z);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                _position -= glm::normalize(glm::cross(_front, _up)) * _moveSensitivity * _moveSensitivityScale;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                _position += glm::normalize(glm::cross(_front, _up)) * _moveSensitivity * _moveSensitivityScale;
        }

        void mouse_callback(double xOffset, double yOffset) {
            xOffset *= _lookSensitivity;
            yOffset *= _lookSensitivity;

            _yaw += xOffset;
            _pitch += yOffset;

            _pitch = common::clamp(_pitch, _minPitch, _maxPitch);

            glm::vec3 direction;
            direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
            direction.y = sin(glm::radians(_pitch));
            direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
            _front = direction;
        }

        void scroll_callback(double xOffset, double yOffset) {
            _fov = common::clamp(_fov - _scrollSensitivity * static_cast<float>(yOffset), _minFov, _maxFov);
        }

    private:
        glm::vec3 _position;
        glm::vec3 _up;
        glm::vec3 _front;   // use this to construct lookAt vector

        float _fov = 45.0f;
        const float _maxFov = 150.0f;
        const float _minFov = 5.0f;

        float _yaw = -90.0f;
        float _pitch = 0.0f;
        const float _maxPitch = 89.9f, _minPitch = -89.9f;

        float _moveSensitivity = 0.5f;
        float _moveSensitivityScale = 1.0f;
        float _lookSensitivity = 0.1f;
        float _scrollSensitivity = 1.0f;
};



#endif // CAMERA_H