#include "VbtangOpenGLPlaygroundManager.h"
#include "IVbtangOpenGLPlayground.h"

#include "concretePlaygrouds/HelloTrianglePlayground/HelloTrianglePlayground.h"
#include "concretePlaygrouds/TriangleTessellationPlayground/TriangleTessellationPlayground.h"

void InitPlaygrounds()
{
    std::unordered_map<std::string, std::shared_ptr<IVbtangOpenGLPlayground>> playgrounds
    {
        {"HelloTrianglePlayground", std::make_shared<HelloTrianglePlayground>()},
        {"HelloTrianglePlayground2", std::make_shared<HelloTrianglePlayground>()},
        {"TriangleTessellationPlayground", std::make_shared<TriangleTessellationPlayground>()},
    };

    for (auto& [name, playground] : playgrounds)
    {
        VbtangOpenGLPlaygroundManager::getInstance()->registerPlayground(name, playground);
    }
}

VbtangOpenGLPlaygroundManager::VbtangOpenGLPlaygroundManager() 
{

}

VbtangOpenGLPlaygroundManager::~VbtangOpenGLPlaygroundManager()
{

}

VbtangOpenGLPlaygroundManager* VbtangOpenGLPlaygroundManager::getInstance()
{
    static VbtangOpenGLPlaygroundManager s_openglPlaygroundManager;
    return &s_openglPlaygroundManager;
}

void VbtangOpenGLPlaygroundManager::registerPlayground(const std::string& name, std::shared_ptr<IVbtangOpenGLPlayground> playground)
{
    playgrounds[name] = std::move(playground);
    if (activePlayground.empty())
    {
        activePlayground = name;
    }
}

void VbtangOpenGLPlaygroundManager::unregisterPlayground(const std::string& name)
{
    playgrounds.erase(name);
}

std::shared_ptr<IVbtangOpenGLPlayground> VbtangOpenGLPlaygroundManager::getPlayground(const std::string& name)
{
    if (playgrounds.count(name) == 0)
    {
        return nullptr;
    }

    return playgrounds[name];
}

std::vector<std::string> VbtangOpenGLPlaygroundManager::getPlaygroundNames()
{
    std::vector<std::string> names;
    for (const auto& [name, playground] : playgrounds)
    {
        names.push_back(name);
    }

    return names;
}

std::string VbtangOpenGLPlaygroundManager::getActivePlayground()
{
    return activePlayground;
}

void VbtangOpenGLPlaygroundManager::setActivePlayground(const std::string& name)
{
    if (playgrounds.count(name) == 0)
    {
        return;
    }

    activePlayground = name;
}
