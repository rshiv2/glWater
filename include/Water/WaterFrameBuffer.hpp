#ifndef WATER_FBO_H
#define WATER_FBO_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class WaterFrameBuffer
{
    public:
        WaterFrameBuffer(GLFWwindow* window);
        ~WaterFrameBuffer();

        void bindReflectionFrameBuffer();
        void bindRefractionFrameBuffer();
        void unbindReflectionFrameBuffer(GLFWwindow* window);
        void unbindRefractionFrameBuffer(GLFWwindow* window);

        GLuint getReflectionColorTexture() const;
        GLuint getRefractionColorTexture() const;
        GLuint getRefractionDepthTexture() const;


    private:
        void initReflectionFrameBuffer(GLFWwindow* window);
        void initRefractionFrameBuffer(GLFWwindow* window);
        GLuint createColorTextureAttachment(int width, int height);
        GLuint createDepthTextureAttachment(int width, int height);
        GLuint createDepthBufferAttachment(int width, int height);

        void destroyReflectionFrameBuffer();
        void destroyRefractionFrameBuffer();
    private:
        GLuint reflectionFrameBuffer;
        GLuint reflectionColorTexture;
        GLuint reflectionDepthBuffer;
        unsigned int reflectionBufferWidth = 640;
        unsigned int reflectionBufferHeight = 480;

        
        GLuint refractionFrameBuffer;
        GLuint refractionColorTexture;
        GLuint refractionDepthTexture;
        unsigned int refractionBufferWidth = 640;
        unsigned int refractionBufferHeight = 480;
        
};

WaterFrameBuffer::WaterFrameBuffer(GLFWwindow* window)
{
    initReflectionFrameBuffer(window);
    initRefractionFrameBuffer(window);
}

WaterFrameBuffer::~WaterFrameBuffer()
{
    destroyReflectionFrameBuffer();
    destroyRefractionFrameBuffer();
}

void WaterFrameBuffer::initReflectionFrameBuffer(GLFWwindow* window)
{
    glGenFramebuffers(1, &reflectionFrameBuffer);
    bindReflectionFrameBuffer();
    reflectionColorTexture = createColorTextureAttachment(reflectionBufferWidth, reflectionBufferHeight);
    reflectionDepthBuffer = createDepthBufferAttachment(reflectionBufferWidth, reflectionBufferHeight);
    unbindReflectionFrameBuffer(window); 
}

void WaterFrameBuffer::initRefractionFrameBuffer(GLFWwindow* window)
{
    glGenFramebuffers(1, &refractionFrameBuffer);
    bindRefractionFrameBuffer();
    refractionColorTexture = createColorTextureAttachment(refractionBufferWidth, refractionBufferHeight);
    refractionDepthTexture = createDepthTextureAttachment(refractionBufferWidth, refractionBufferHeight);
    unbindReflectionFrameBuffer(window); 
}

GLuint WaterFrameBuffer::createColorTextureAttachment(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);  // reduces dudv artifacts when reading off edge of texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    return texture;
}

GLuint WaterFrameBuffer::createDepthTextureAttachment(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    return texture;
}

GLuint WaterFrameBuffer::createDepthBufferAttachment(int width, int height)
{
    GLuint buffer;
    glGenRenderbuffers(1, &buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, reflectionBufferWidth, reflectionBufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer);
    return buffer;
}

void WaterFrameBuffer::bindReflectionFrameBuffer()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
    glViewport(0, 0, reflectionBufferWidth, reflectionBufferHeight);
}

void WaterFrameBuffer::unbindReflectionFrameBuffer(GLFWwindow* window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int fWidth, fHeight;
    glfwGetFramebufferSize(window, &fWidth, &fHeight);
    glViewport(0,0,fWidth,fHeight);
}

void WaterFrameBuffer::bindRefractionFrameBuffer()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
    glViewport(0, 0, refractionBufferWidth, refractionBufferHeight);
}

void WaterFrameBuffer::unbindRefractionFrameBuffer(GLFWwindow* window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int fWidth, fHeight;
    glfwGetFramebufferSize(window, &fWidth, &fHeight);
    glViewport(0,0,fWidth,fHeight);
}

GLuint WaterFrameBuffer::getReflectionColorTexture() const
{
    return reflectionColorTexture;
}

GLuint WaterFrameBuffer::getRefractionColorTexture() const
{
    return refractionColorTexture;
}

GLuint WaterFrameBuffer::getRefractionDepthTexture() const
{
    return refractionDepthTexture;
}

void WaterFrameBuffer::destroyReflectionFrameBuffer()
{
    glDeleteFramebuffers(1, &reflectionFrameBuffer);
}

void WaterFrameBuffer::destroyRefractionFrameBuffer()
{
    glDeleteFramebuffers(1, &refractionFrameBuffer);
}
#endif // WATER_FBO_H