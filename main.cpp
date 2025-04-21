#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <memory>

#include "IvbtangOpenGLPlayground.h"
#include "vbtangOpenGLPlaygroundManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    const std::string activePlayground = VbtangOpenGLPlaygroundManager::getInstance()->getActivePlayground();
    std::shared_ptr<IVbtangOpenGLPlayground> playground = VbtangOpenGLPlaygroundManager::getInstance()->getPlayground(activePlayground);
    if (nullptr != playground)
    {
        playground->onSizeChanged(window);
    }
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 初始化GLFW
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    // create window
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window(glfwCreateWindow(800, 600, "vbtangOpenGLPlayground", NULL, NULL), glfwDestroyWindow);
    if (window == NULL) 
    {
        std::cout << "创建GLFW窗口失败" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window.get());

    // 初始化GLAD
    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置视口
    // set viewport
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);

    // 初始化ImGui
    // initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // 加载中文字体
    // load font for chinese characters
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    ImGui::StyleColorsDark();
    
    // 设置ImGui平台/渲染器后端
    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // Init all playgrounds
    InitPlaygrounds();

    // 一些示例变量
    // bool show_demo_window = true;
    float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    // 渲染循环
    // render loop
    while(!glfwWindowShouldClose(window.get())) {
        // 输入处理
        // process input
        processInput(window.get());

        const std::string activePlayground = VbtangOpenGLPlaygroundManager::getInstance()->getActivePlayground();
        std::shared_ptr<IVbtangOpenGLPlayground> playground = VbtangOpenGLPlaygroundManager::getInstance()->getPlayground(activePlayground);

        // 开始ImGui帧
        // start of ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 显示ImGui演示窗口
        // show IGUI sample win 
        // if (show_demo_window)
        //     ImGui::ShowDemoWindow(&show_demo_window);

        // 创建一个简单的ImGui窗口
        // create a sample ImGui window
        {
            ImGui::Begin("Control panel");

            const auto playgroundNames = VbtangOpenGLPlaygroundManager::getInstance()->getPlaygroundNames();
            std::vector<const char*> playgroundItems;
            for (const auto& name : playgroundNames)
            {
                playgroundItems.push_back(name.c_str());
            }

            static int currentPlaygroundIndex = 0;
            ImGui::Combo("Playgrounds", &currentPlaygroundIndex, playgroundItems.data(), static_cast<int>(playgroundItems.size()));
            std::string activePlayground = VbtangOpenGLPlaygroundManager::getInstance()->getActivePlayground();
            if (activePlayground != playgroundNames[currentPlaygroundIndex])
            {
                VbtangOpenGLPlaygroundManager::getInstance()->setActivePlayground(playgroundNames[currentPlaygroundIndex]);
            }
            
            // 获取当前可用区域（避免被后面的控件影响）
            // get the current available region (to avoid being affected by the following controls)
            ImVec2 avail = ImGui::GetContentRegionAvail();

            // 预留下面两个控件的高度（一个大概值）
            // reserve the height for the bottom two controls (a rough value manually)
            float reserved_height = ImGui::GetFrameHeight() * 2.5f; // 粗略估计 colorEdit 和 fps 的高度
            avail.y -= reserved_height;

            ImGui::BeginChild("Playground Properties", avail, true);
            ImGui::Text("Playground Properties");
            if (nullptr != playground)
            {
                playground->updateUI();
            }
            ImGui::EndChild();

            ImGui::ColorEdit3("Background Color", clear_color);
            ImGui::Text("Avg frame time %.3f ms/frame (%.1f FPS)", 
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 渲染
        // ImGUI Render
        ImGui::Render();
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // 在此处渲染防止 ImGUI 被我们的绘制覆盖
        // render playground here to prevent ImGUI from being covered by our drawing
        if (nullptr != playground)
        {
            playground->init(window.get());
            playground->render(window.get());
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲并查询IO事件
        // swap buffers and query IO events
        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    // 清理所有OpenGL playgrounds
    // cleanup all OpenGL playgrounds
    const auto playgroudNames = VbtangOpenGLPlaygroundManager::getInstance()->getPlaygroundNames();
    for (const auto& name : playgroudNames)
    {
        std::shared_ptr<IVbtangOpenGLPlayground> playground = VbtangOpenGLPlaygroundManager::getInstance()->getPlayground(name);
        if (nullptr != playground)
        {
            playground->cleanup();
        }

        VbtangOpenGLPlaygroundManager::getInstance()->unregisterPlayground(name);
    }

    // 清理ImGui
    // clean up ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 清理GLFW
    // clean up GLFW
    glfwTerminate();
    return 0;
}

