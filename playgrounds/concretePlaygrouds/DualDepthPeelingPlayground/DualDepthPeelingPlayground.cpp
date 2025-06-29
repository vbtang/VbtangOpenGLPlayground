#include "DualDepthPeelingPlayground.h"

DualDepthPeelingPlayground::DualDepthPeelingPlayground()
{

}

DualDepthPeelingPlayground::~DualDepthPeelingPlayground()
{

}

int DualDepthPeelingPlayground::initImpl(GLFWwindow* window)
{
    if ( nullptr == window )
    {
        return -1;
    }

    glfwGetWindowSize(window, &m_imageWidth, &m_imageHeight);

        float vertices[] = 
        {
            0.0f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f 
         }; 

    //unsigned int VBO, VAO;
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    m_drawBuffers = {GL_COLOR_ATTACHMENT0,
                        GL_COLOR_ATTACHMENT1,
                        GL_COLOR_ATTACHMENT2,
                        GL_COLOR_ATTACHMENT3,
                        GL_COLOR_ATTACHMENT4,
                        GL_COLOR_ATTACHMENT5,
                        GL_COLOR_ATTACHMENT6};

    glGenTextures(2, m_dualDepthTexId);
    glGenTextures(2, m_dualFrontBlenderTexId);
    glGenTextures(2, m_dualBackTempTexId);
    glGenFramebuffers(1, &m_dualPeelingSingleFboId);
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE, m_dualDepthTexId[i]);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG32F, m_imageWidth, m_imageHeight,
                     0, GL_RGB, GL_FLOAT, 0);

        glBindTexture(GL_TEXTURE_RECTANGLE, m_dualFrontBlenderTexId[i]);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_imageWidth, m_imageHeight,
                     0, GL_RGBA, GL_FLOAT, 0);

        glBindTexture(GL_TEXTURE_RECTANGLE, m_dualBackTempTexId[i]);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, m_imageWidth, m_imageHeight,
                     0, GL_RGBA, GL_FLOAT, 0);
    }

    glGenTextures(1, &m_dualBackBlenderTexId);
    glBindTexture(GL_TEXTURE_RECTANGLE, m_dualBackBlenderTexId);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, m_imageWidth, m_imageHeight,
                 0, GL_RGB, GL_FLOAT, 0);

    glGenFramebuffers(1, &m_dualBackBlenderFboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_dualBackBlenderFboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_RECTANGLE, m_dualBackBlenderTexId, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_dualPeelingSingleFboId);

    int j = 0;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_RECTANGLE, m_dualDepthTexId[j], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                  GL_TEXTURE_RECTANGLE, m_dualFrontBlenderTexId[j], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                                  GL_TEXTURE_RECTANGLE, m_dualBackTempTexId[j], 0);

    j = 1;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
                                  GL_TEXTURE_RECTANGLE, m_dualDepthTexId[j], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4,
                                  GL_TEXTURE_RECTANGLE, m_dualFrontBlenderTexId[j], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5,
                                  GL_TEXTURE_RECTANGLE, m_dualBackTempTexId[j], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6,
                              GL_TEXTURE_RECTANGLE, m_dualBackBlenderTexId, 0);
    return 0;
}

int DualDepthPeelingPlayground::onSizeChangedImpl(GLFWwindow* window)
{
    return 0;
}

int DualDepthPeelingPlayground::renderImpl(GLFWwindow* window)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // ---------------------------------------------------------------------
    // 1. Initialize Min-Max Depth Buffer
    // ---------------------------------------------------------------------

    glBindFramebuffer(GL_FRAMEBUFFER, m_dualPeelingSingleFboId);

    // Render targets 1 and 2 store the front and back colors
    // Clear to 0.0 and use MAX blending to filter written color
    // At most one front color and one back color can be written every pass
    glDrawBuffers(2, &m_drawBuffers[1]);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
    glDrawBuffer(m_drawBuffers[0]);
    constexpr float MAX_DEPTH = 1.0f;
    glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendEquation(GL_MAX);

    m_shaderDualInit.use();
    //DrawModel();
    m_shaderDualInit.unuse();

    // ---------------------------------------------------------------------
    // 2. Dual Depth Peeling + Blending
    // ---------------------------------------------------------------------

    // Since we cannot blend the back colors in the geometry passes,
    // we use another render target to do the alpha blending
    //glBindFramebuffer(GL_FRAMEBUFFER, m_dualBackBlenderFboId);
    glDrawBuffer(m_drawBuffers[6]);
    glClearColor(1.0, 1.0, 1.0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    int currId = 0;

    for (int pass = 1; m_useOQ || pass < m_numPasses; pass++) {
        currId = pass % 2;
        int prevId = 1 - currId;
        int bufId = currId * 3;
        
        //glBindFramebuffer(GL_FRAMEBUFFER, m_dualPeelingFboId[currId]);

        glDrawBuffers(2, &m_drawBuffers[bufId+1]);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawBuffer(m_drawBuffers[bufId+0]);
        glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render target 0: RG32F MAX blending
        // Render target 1: RGBA MAX blending
        // Render target 2: RGBA MAX blending
        glDrawBuffers(3, &m_drawBuffers[bufId+0]);
        glBlendEquation(GL_MAX);

        m_shaderDualPeeling.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_RECTANGLE, m_dualDepthTexId[prevId]);
        m_shaderDualPeeling.setInt("DepthBlenderTex", 0);
        glActiveTexture(GL_TEXTURE0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_RECTANGLE, m_dualFrontBlenderTexId[prevId]);
        m_shaderDualPeeling.setInt("FrontBlenderTex", 1);
        glActiveTexture(GL_TEXTURE0);
        m_shaderDualPeeling.setFloat("Alpha", m_opacity);
        
        
        //DrawModel();
        glActiveTexture(GL_TEXTURE0);
        m_shaderDualPeeling.unuse();

        // CHECK_GL_ERRORS;

        // Full screen pass to alpha-blend the back color
        glDrawBuffer(m_drawBuffers[6]);

        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (m_useOQ)
        {
            glBeginQuery(GL_SAMPLES_PASSED, m_queryId);
        }

        m_shaderDualBlend.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_RECTANGLE, m_dualBackTempTexId[currId]);
        m_shaderDualBlend.setInt("TempTex", 0);
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 4);
        glBindVertexArray(0);

        m_shaderDualBlend.unuse();

        //CHECK_GL_ERRORS;

        if (m_useOQ) 
        {
            glEndQuery(GL_SAMPLES_PASSED);
            GLuint sample_count;
            glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, &sample_count);
            if (sample_count == 0) 
            {
                break;
            }
        }
    }

    glDisable(GL_BLEND);

    // ---------------------------------------------------------------------
    // 3. Final Pass
    // ---------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    m_shaderDualFinal.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, m_dualDepthTexId[currId]);
    m_shaderDualFinal.setInt("DepthBlenderTex",  0);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_RECTANGLE, m_dualFrontBlenderTexId[currId]);
    m_shaderDualFinal.setInt("FrontBlenderTex", 1);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_RECTANGLE, m_dualBackBlenderTexId);
    m_shaderDualFinal.setInt("BackBlenderTex", 2);
    glActiveTexture(GL_TEXTURE0);
    //glCallList(m_quadDisplayList);
    m_shaderDualFinal.unuse();

	return 0;
}

int DualDepthPeelingPlayground::updateUIImpl()
{
    return 0;
}




