#pragma once
#ifndef _IVBTANG_OPENGL_PLAYGROUND_H_
#define _IVBTANG_OPENGL_PLAYGROUND_H_

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "tools/shader.h"

class IVbtangOpenGLPlayground 
{
public:
    IVbtangOpenGLPlayground() = default;
    virtual ~IVbtangOpenGLPlayground() {};
    
public:
    void init(GLFWwindow* window)
    {
        // if the playground initialization failed, do not try to initialize it again
        if (isInitializedFailed)
        {
            return;
        }

        if (isInitialized)
        {
            return;
        }

        if (initImpl(window) != 0)
        {
            std::cout<<"Failed to initialize the playground" << std::endl;
            isInitializedFailed = true;

            return;
        }

        isInitialized = true;
    }

    void onSizeChanged(GLFWwindow* window)
    {
        if (!isInitialized)
        {
            return;
        }

        onSizeChangedImpl(window);
    }

    void render(GLFWwindow* window)
    {
        if (!isInitialized)
        {
            return;
        }

        renderImpl(window);
    }

    void updateUI()
    {
        if (!isInitialized)
        {
            return;
        }

        updateUIImpl();
    }

    void cleanup()
    {
        if (!isInitialized)
        {
            return;
        }

        cleanupImpl();
    }


protected:
    // initialize all OpenGL resources at the beginning
    virtual int initImpl(GLFWwindow* window) = 0;
    // update the OpenGL resources when the window is resized
    virtual int onSizeChangedImpl(GLFWwindow* window) = 0;
    // render
    // glClearColor(GL_COLOR_BUFFER_BIT) won't work since we clear the screen in the main loop
    virtual int renderImpl(GLFWwindow* window) = 0;
    // bind uniforms with ImGUI controls in this function so we can interact with the OpenGL
    virtual int updateUIImpl() = 0;
    // cleanup the OpenGL resources
    virtual int cleanupImpl() = 0;

private:
    bool isInitialized = false;
    bool isInitializedFailed = false;
};

#endif // _IVBTANG_OPENGL_PLAYGROUND_H_
