#include "TriangleTessellationPlayground.h"

#include <optional>
#include "tessellator.hpp"

constexpr unsigned int MAX_TESS_NUM_PER_EDGE = 64;
constexpr unsigned int MAX_TESS_INDEX_NUM = 18432;

TriangleTessellationPlayground::TriangleTessellationPlayground()
: m_useGLTess(false)
{
    m_tessLevelOuter = {5.0f, 5.0f, 5.0f};
    m_tessLevelInner = 5.0f;
}

TriangleTessellationPlayground::~TriangleTessellationPlayground()
{

}


int TriangleTessellationPlayground::initImpl(GLFWwindow* window)
{
    if (!m_glTessShader.isInitialized())
    {
        std::map<GLenum, std::string> shaderFileMap = 
        {
            {GL_VERTEX_SHADER, R"(../playgrounds/concretePlaygrouds/TriangleTessellationPlayground/vertex.glsl)"},
            {GL_FRAGMENT_SHADER, R"(../playgrounds/concretePlaygrouds/TriangleTessellationPlayground/fragment.glsl)"},
            {GL_TESS_CONTROL_SHADER, R"(../playgrounds/concretePlaygrouds/TriangleTessellationPlayground/tess_control_shader.glsl)"},
            {GL_TESS_EVALUATION_SHADER, R"(../playgrounds/concretePlaygrouds/TriangleTessellationPlayground/tess_eval_shader.glsl)"}
        };

        if (m_glTessShader.loadShaderFromFile(shaderFileMap))
        {
            return -1;
        }

        std::map<GLenum, std::string> shaderFileMap1 =
        {
            {GL_VERTEX_SHADER, R"(../playgrounds/concretePlaygrouds/TriangleTessellationPlayground/vertex.glsl)"},
            {GL_FRAGMENT_SHADER, R"(../playgrounds/concretePlaygrouds/TriangleTessellationPlayground/fragment.glsl)"}
        };
        if (m_cpuTessShader.loadShaderFromFile(shaderFileMap1))
        {
            return -1;
        }


        // 
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        // Vertex Buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, MAX_TESS_NUM_PER_EDGE * MAX_TESS_NUM_PER_EDGE * MAX_TESS_NUM_PER_EDGE * sizeof(float), nullptr, GL_STATIC_DRAW);

        // Element Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_TESS_INDEX_NUM * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

        // Vertex Attribute: position (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // 解绑 VAO（可选）
    glBindVertexArray(0);

    return 0;
}

int TriangleTessellationPlayground::onSizeChangedImpl(GLFWwindow* window)
{ 
    return 0;
}

int TriangleTessellationPlayground::renderImpl(GLFWwindow* window)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(m_vao);

    std::array<glm::vec3, 3> trianglePoints = 
    {
        //glm::vec3(-0.5f, -0.5f, 0.0f), // left  
        //glm::vec3(0.5f, -0.5f, 0.0f), // right 
        //glm::vec3(/*-0.75f*/ 0,  0.5f, 0.0f)  // top
        glm::vec3(0.0f, 0.0f, 0.0f), // left  
        glm::vec3(1.0f, 0.0f, 0.0f), // right 
        glm::vec3(/*-0.75f*/ 0.0f,  1.0f, 0.0f)  // top
    };

    if (m_useGLTess)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(trianglePoints), trianglePoints.data(), GL_STATIC_DRAW);

        glm::ivec3 index{0, 1, 2};
        // Element Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), glm::value_ptr(index), GL_STATIC_DRAW);
    }
    else
    {
        tesselletor.Init(PIPE_TESSELLATOR_PARTITIONING_INTEGER, PIPE_TESSELLATOR_OUTPUT_TRIANGLE_CW);
        tesselletor.TessellateTriDomain(m_tessLevelOuter[0], m_tessLevelOuter[1], m_tessLevelOuter[2], m_tessLevelInner);
        
        std::vector<DOMAIN_POINT> points(tesselletor.GetPointCount());
        DOMAIN_POINT* tessPoints = tesselletor.GetPoints();
        std::copy(tessPoints, tessPoints + tesselletor.GetPointCount(), points.begin());
        // flat vertex data
        std::vector<float> vertexData;
        for (const DOMAIN_POINT& p : points/*m_tessellations.vertices*/)
        {
            glm::vec3 pt = p.u*trianglePoints[0] + p.v*trianglePoints[1] + (1 - p.u - p.v) * trianglePoints[2];

            vertexData.push_back(pt.x);
            vertexData.push_back(pt.y);
            vertexData.push_back(pt.z);
        }

        // flat indexs
        std::vector<unsigned int> indexData;
        //for (const std::array<int, 3>& face : m_tessellations.faces) 
        for (int idx=0; idx<tesselletor.GetIndexCount(); ++idx)
        {
            indexData.push_back(tesselletor.GetIndices()[idx]);
        }
        
        // Vertex Buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        // Element Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);
    }


    if (m_useGLTess)
    {
        m_glTessShader.use();
        m_glTessShader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        m_glTessShader.setVec3("tessLevelOuter", m_tessLevelOuter);
        m_glTessShader.setFloat("tessLevelInner", m_tessLevelInner);
        glDrawArrays(GL_PATCHES, 0, 3);
        m_glTessShader.unuse();
    }
    else
    {
        m_cpuTessShader.use();
        m_cpuTessShader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        glDrawElements(GL_TRIANGLES, tesselletor.GetIndexCount(), GL_UNSIGNED_INT, 0);
        m_cpuTessShader.unuse();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return 0;
}

int TriangleTessellationPlayground::updateUIImpl()
{
    ImGui::Checkbox("useGLTess", &m_useGLTess);
    ImGui::InputFloat3("tessLevelOuter", glm::value_ptr(m_tessLevelOuter));
    ImGui::InputFloat("tessLevelInner", &m_tessLevelInner);

    return 0;
}

int TriangleTessellationPlayground::cleanupImpl()
{
    return 0;
}