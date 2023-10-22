#ifndef WATER_H
#define WATER_H

#include <vector>
#include <algorithm>
#include <chrono>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

#include "Shader.hpp"
#include "Camera.hpp"
#include "Water/WaterFrameBuffer.hpp"

class Water
{
    public:

        /**
         * Water is represented as a quad located at "center". Its extent is parameterized by
         * dx and dy. The four corners of the quad are computed as center +/- dx +/- dy.
         */
        Water(GLFWwindow* window, const glm::vec3& center, const glm::vec3& dx, const glm::vec3& dy);
        ~Water();

        void draw(Shader* shader, const Camera* camera, const int viewportWidth, const int viewportHeight);

        WaterFrameBuffer* getWaterFrameBuffer() {
            return _waterFrameBuffer;
        }

        glm::vec3 getNormal() const {
            glm::vec3 normal = glm::normalize(glm::cross(_dx, _dy));
            return normal;
        }

        /* Expresses water as a plane with equation Ax + By + Cz + D = 0.        
         * Returns glm::vec4(A, B, C, D)
         */
        glm::vec4 getPlaneEquation() const {

            glm::vec3 normal = getNormal();
            float distFromOrigin = glm::dot(_center, normal);
            return glm::vec4(normal, distFromOrigin);

        }

        void setWaveDirection(glm::vec2 v) {

            _waveDirection = v;
        }

    private:
        GLuint initWaterVAO();
        Shader* initWaterShader();
        GLuint initWaterDuDvMap();

    private:
        glm::vec3 _center;
        glm::vec3 _dx;
        glm::vec3 _dy;

        std::chrono::time_point<std::chrono::high_resolution_clock> _timeAtCtor;    // a higher level class should manage this in the future
        GLuint _waterVAO, _waterVBO, _waterEBO;
        GLuint _waterDuDvMap;
        GLuint _waterNormalMap;
        
        const std::string _dudvMapPath = "../include/Water/dudv.png";
        const std::string _normalMapPath = "../include/Water/normals.png";

        WaterFrameBuffer* _waterFrameBuffer;

        const float _refractiveIndex = 1.33f;
        glm::vec2 _waveDirection;
        float _waveSpeed = 0.05f;
        float _waveMoveFactor = 0.0f;

};

Water::Water(GLFWwindow* window, const glm::vec3& center, const glm::vec3& dx, const glm::vec3& dy)
    : _center(center), _dx(dx), _dy(dy)
{
    _waterFrameBuffer = new WaterFrameBuffer(window);
    _waterVAO = initWaterVAO();
    _waterDuDvMap = initWaterDuDvMap();
    setWaveDirection(glm::vec2(0.0f, -1.0f));
    _timeAtCtor = std::chrono::high_resolution_clock::now();
}

Water::~Water()
{
    delete _waterFrameBuffer;
    glDeleteBuffers(1, &_waterVBO);
    glDeleteBuffers(1, &_waterEBO);
    glDeleteTextures(1, &_waterDuDvMap);
    glDeleteVertexArrays(1, &_waterVAO);
}

GLuint Water::initWaterVAO()
{
    // define quad vertices
    const int numVertices = 4, floatsPerVertex = 3;
    float vertexData[numVertices * floatsPerVertex];

    glm::vec3 vertices[numVertices];
    vertices[0] = _center - _dx - _dy;
    vertices[1] = _center + _dx - _dy;
    vertices[2] = _center - _dx + _dy;
    vertices[3] = _center + _dx + _dy;

    for (int i = 0; i < numVertices; i++) {
        vertexData[i * floatsPerVertex + 0] = vertices[i].x;
        vertexData[i * floatsPerVertex + 1] = vertices[i].y;
        vertexData[i * floatsPerVertex + 2] = vertices[i].z;
    }

    GLuint vertexIndices[] = { 
        0, 1, 2,
        1, 3, 2
    };

    // create vertex array
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &_waterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &_waterEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

GLuint Water::initWaterDuDvMap()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    unsigned char* data = stbi_load(_dudvMapPath.c_str(), &width, &height, &numChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
        std::cout << "Water::initWaterDuDvMap::ERROR: Failed to load image." << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

void Water::draw(Shader* shader, const Camera* camera, const int viewportWidth, const int viewportHeight)
{
    glBindVertexArray(_waterVAO);
    shader->use();

    glm::mat4 view = camera->lookAt(); 
    glm::mat4 projection = glm::perspective(glm::radians(camera->getFov()), (float)(viewportWidth) / viewportHeight, 0.1f, 100.0f); 
    shader->setFloat("nearPlane", 0.1f);
    shader->setFloat("farPlane", 100.0f);

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    shader->setInt("reflectionTexture", 0);
    shader->setInt("refractionTexture", 1);
    shader->setInt("dudvMap", 2);
    shader->setInt("depthTexture", 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer->getReflectionColorTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer->getRefractionColorTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _waterDuDvMap); 
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer->getRefractionDepthTexture());

    // compute fresnel factor using Schlick's approximation
    float R0 = (1 - _refractiveIndex) / (1 + _refractiveIndex) * (1 - _refractiveIndex) / (1 + _refractiveIndex);
    float R = R0 + (1 - R0) * (1 - std::max(glm::dot(-camera->getFront(), this->getNormal()), 0.0f));
    shader->setFloat("fresnelFactor", 1);

    // update wave velocity and time uniforms
    auto currTime = std::chrono::high_resolution_clock::now();
    float sec = std::chrono::duration_cast<std::chrono::seconds>(currTime - _timeAtCtor).count() / 1000.0f;
    _waveMoveFactor += _waveSpeed * sec;
    float moveFactorFractionalPart = _waveMoveFactor - (int)(_waveMoveFactor);
    shader->setFloat("waveMoveFactor", moveFactorFractionalPart);
    shader->setVec2("waveDir", _waveDirection);

    // draw call
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (void*)0);
    glDisable(GL_BLEND);
    
}

#endif // WATER_H