#include "HelloTrianglePlayground.h"

HelloTrianglePlayground::HelloTrianglePlayground()
{
    VAO = 0;
    VBO = 0;
    color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // 初始化为白色
}

HelloTrianglePlayground::~HelloTrianglePlayground()
{

}

int HelloTrianglePlayground::initImpl(GLFWwindow* window)
{
    std::map<GLenum, std::string> shaderFileMap = 
    {
        {GL_VERTEX_SHADER, R"(../playgrounds/concretePlaygrouds/HelloTrianglePlayground/vertex_shader.glsl)"},
        {GL_FRAGMENT_SHADER, R"(../playgrounds/concretePlaygrouds/HelloTrianglePlayground/fragment_shader.glsl)"}
    };

    if (shader.loadShaderFromFile(shaderFileMap))
    {
        return -1;
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    //unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    return 0;
}

int HelloTrianglePlayground::onSizeChangedImpl(GLFWwindow* window)
{
    return 0;
}

int HelloTrianglePlayground::renderImpl(GLFWwindow* window)
{
    shader.use();
    glBindVertexArray(VAO);
    shader.setVec4("color", color);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return 0;
}

int HelloTrianglePlayground::updateUIImpl()
{
    ImGui::ColorEdit4("Triangle Color", glm::value_ptr(color));

    return 0;
}

int HelloTrianglePlayground::cleanupImpl()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    return 0;
}

