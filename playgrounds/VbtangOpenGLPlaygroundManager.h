#pragma once
#ifndef _IVBTANG_OPENGL_PLAYGROUND_MANAGER_H_
#define _IVBTANG_OPENGL_PLAYGROUND_MANAGER_H_

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

class IVbtangOpenGLPlayground;

class VbtangOpenGLPlaygroundManager 
{
public:
    static VbtangOpenGLPlaygroundManager* getInstance();
    ~VbtangOpenGLPlaygroundManager();

    void registerPlayground(const std::string& name, std::shared_ptr<IVbtangOpenGLPlayground> playground);
    void unregisterPlayground(const std::string& name);

    std::shared_ptr<IVbtangOpenGLPlayground> getPlayground(const std::string& name);
    std::vector<std::string> getPlaygroundNames();
    std::string getActivePlayground();
    void setActivePlayground(const std::string& name);

private:
    VbtangOpenGLPlaygroundManager();
    VbtangOpenGLPlaygroundManager(const VbtangOpenGLPlaygroundManager&) = delete;
    VbtangOpenGLPlaygroundManager& operator=(const VbtangOpenGLPlaygroundManager&) = delete;

private:
    std::unordered_map<std::string, std::shared_ptr<IVbtangOpenGLPlayground>> playgrounds;
    std::string activePlayground;
};

void InitPlaygrounds();

#endif // _IVBTANG_OPENGL_PLAYGROUND_MANAGER_H_
