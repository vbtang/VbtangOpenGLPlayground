#pragma once
#ifndef TRIANGLE_TESSELLATION_PLAYGROUND_H
#define TRIANGLE_TESSELLATION_PLAYGROUND_H

#include <array>
#include <optional>

#include "IVbtangOpenGLPlayground.h"
#include "tessellator.hpp"

class TriangleTessellationPlayground : public IVbtangOpenGLPlayground
{
public:
    TriangleTessellationPlayground();
    ~TriangleTessellationPlayground();

protected:
    int initImpl(GLFWwindow* window) override;
    int onSizeChangedImpl(GLFWwindow* window) override;
    int renderImpl(GLFWwindow* window) override;
    int updateUIImpl() override;
    int cleanupImpl() override;

private:
    bool m_useGLTess;

    float m_tessLevelInner;
    glm::vec3 m_tessLevelOuter;

    CHWTessellator tesselletor;

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    Shader m_glTessShader;
    Shader m_cpuTessShader;
};

#endif // TRIANGLE_TESSELLATION_PLAYGROUND_H

