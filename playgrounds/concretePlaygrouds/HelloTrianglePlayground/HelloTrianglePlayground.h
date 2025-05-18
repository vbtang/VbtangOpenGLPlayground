#pragma once
#ifndef _HELLO_TRIANGLE_PLAYGROUND_H_
#define _HELLO_TRIANGLE_PLAYGROUND_H_


#include "IVbtangOpenGLPlayground.h"

class HelloTrianglePlayground : public IVbtangOpenGLPlayground
{
public:
    HelloTrianglePlayground();
    ~ HelloTrianglePlayground();

protected:
    int initImpl(GLFWwindow* window) override;
    int onSizeChangedImpl(GLFWwindow* window) override;
    int renderImpl(GLFWwindow* window) override;
    int updateUIImpl() override;
    int cleanupImpl() override;

private:
    GLuint VAO;
    GLuint VBO;
    Shader shader;

    glm::vec4 color;
};



#endif // _HELLO_TRIANGLE_PLAYGROUND_H_

