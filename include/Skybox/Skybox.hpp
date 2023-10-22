#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Camera.hpp"


class Skybox 
{
    public:

        /**
         * @brief Construct a new Skybox object. 
         * 
         * @param faces Vector of 6 image paths. Expected order of paths is
         *                  GL_TEXTURE_CUBE_MAP_POSITIVE_X
         *                  GL_TEXTURE_CUBE_MAP_NEGATIVE_X
         *                  GL_TEXTURE_CUBE_MAP_POSITIVE_Y
         *                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
         *                  GL_TEXTURE_CUBE_MAP_POSITIVE_Z
         *                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
         */
        Skybox(const std::vector<std::string>& faces);

        ~Skybox();

        /**
         * @brief Draw skybox.
         * 
         * @param camera Scene camera.
         */
        void draw(Shader* shader);


    private:
        GLuint initCubeMap(const std::vector<std::string>& faces);
        GLuint initVertices();
        void updateUniforms(Shader* shader);

    private:
        GLuint cubeMapVAO, cubeMapVBO;

        GLuint cubeMapTexture;

        std::vector<float> skyBoxVertices = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
};

Skybox::Skybox(const std::vector<std::string>& faces)
{
    for (auto& elem : skyBoxVertices)
        elem *= 0.99f;
        
    cubeMapTexture = initCubeMap(faces);
    cubeMapVAO = initVertices();
}

Skybox::~Skybox()
{
    glDeleteBuffers(1, &cubeMapVBO);
    glDeleteVertexArrays(1, &cubeMapVAO);
}

GLuint Skybox::initCubeMap(const std::vector<std::string>& faces)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, numChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &numChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;

}

GLuint Skybox::initVertices()
{
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &cubeMapVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeMapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skyBoxVertices.size(), (void*)(skyBoxVertices.data()), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

void Skybox::updateUniforms(Shader* shader)
{
    shader->setFloat("exposure", 0.2);
}

void Skybox::draw(Shader* shader)
{
    updateUniforms(shader);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    shader->use();

    // bind and draw
    glBindVertexArray(cubeMapVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

}
#endif // SKYBOX_H