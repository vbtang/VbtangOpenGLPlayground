#pragma once
#ifndef _DUAL_DEPTH_PEELING_PLAYGROUND_H_
#define _DUAL_DEPTH_PEELING_PLAYGROUND_H_

#include "IVbtangOpenGLPlayground.h"

class DualDepthPeelingPlayground : public IVbtangOpenGLPlayground
{
public:
    DualDepthPeelingPlayground();
    ~DualDepthPeelingPlayground();

protected:
    int initImpl(GLFWwindow* window) override;
    int onSizeChangedImpl(GLFWwindow* window) override;
    int renderImpl(GLFWwindow* window) override;
    int updateUIImpl() override;

private:
    GLuint m_dualDepthTexId[2];
    GLuint m_dualFrontBlenderTexId[2];
    GLuint m_dualBackBlenderTexId;
    GLuint m_dualPeelingSingleFboId;
    std::vector<GLuint> m_drawBuffers;
    GLuint m_dualPeelingFboId[2];
    GLuint m_dualBackBlenderFboId;

    Shader m_shaderDualInit;
    Shader m_shaderDualPeeling;
    Shader m_shaderDualBackBlender;
    Shader m_shaderDualBlend;
    Shader m_shaderDualFinal;

    float m_opacity;
    GLuint m_dualBackTempTexId[2];
    GLuint m_queryId;

    bool m_useOQ;
    int m_numPasses;

    int m_imageWidth;
    int m_imageHeight;

    GLuint m_quadVAO;
    GLuint m_quadVBO;
    Shader m_quadShader;
};

#endif
