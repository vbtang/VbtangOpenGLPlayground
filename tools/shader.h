#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <map>

class Shader
{
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader() : programID(0), initialized(false)
    {
    }

    ~Shader()
    {
        glDeleteProgram(programID);
    }

    int loadShaderFromFile(const std::map<GLenum, std::string>& shaderFileMap)
    {
        std::map<GLenum, std::string> shaderSrcMap;
        for (const auto &[shaderType, shaderPath] : shaderFileMap)
        {
            shaderSrcMap[shaderType] = loadStringFromFile(shaderPath);
        }

        return loadShaderFromMemory(shaderSrcMap);
    }

    int loadShaderFromMemory(const std::map<GLenum, std::string>& shaderSrcMap)
    {
        if (initialized)
        {
            std::cout << "INFO::SHADER::ALREADY_INITIALIZED" << std::endl;
            return -1;
        }

        programID = glCreateProgram();
        std::vector<unsigned int> shaders;
        for (const auto& [shaderType, shaderSrc] : shaderSrcMap)
        {
            unsigned int shaderID = glCreateShader(shaderType);
            const char* shaderSrcPtr = shaderSrc.c_str();
            glShaderSource(shaderID, 1, &shaderSrcPtr, nullptr);
            glCompileShader(shaderID);
            checkCompileErrors(shaderID, shaderTypeToString(shaderType));

            glAttachShader(programID, shaderID);

            shaders.emplace_back(shaderID);
        }

        glLinkProgram(programID);
        checkCompileErrors(programID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        for (auto shaderID : shaders)
        {
            glDeleteShader(shaderID);
        }

        initialized = true;

        return 0;
    }

    bool isInitialized()
    {
        return programID != 0;
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    { 
        glUseProgram(programID); 
    }

    void unuse() const
    {
        glUseProgram(0);
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(programID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(programID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    std::string shaderTypeToString(GLenum shaderType) 
    {
        switch (shaderType) 
        {
            case GL_VERTEX_SHADER: return "Vertex Shader";
            case GL_FRAGMENT_SHADER: return "Fragment Shader";
            case GL_GEOMETRY_SHADER: return "Geometry Shader";
            case GL_TESS_CONTROL_SHADER: return "Tessellation Control Shader";
            case GL_TESS_EVALUATION_SHADER: return "Tessellation Evaluation Shader";
            case GL_COMPUTE_SHADER: return "Compute Shader";
            default: return "Unknown Shader Type";
        }
    }

    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

private:
    std::string loadStringFromFile(const std::string& filepath)
    {
        std::string fileContent;
        std::ifstream ifstreamFile;

#ifdef DEBUG
        std::cout << "Absolute shader file path is: " << std::filesystem::absolute(filepath) << std::endl;
#endif

        if(!std::filesystem::exists(filepath))
        {
            std::cout << "ERROR::SHADER::FILE: " << filepath << " NOT_FOUND" << std::endl;
            return "";
        }

        // ensure ifstream objects can throw exceptions:
        ifstreamFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            ifstreamFile.open(filepath);
            std::stringstream fileStringStream;
            // read file's buffer contents into streams
            fileStringStream << ifstreamFile.rdbuf();
            // close file handlers
            ifstreamFile.close();
            // convert stream into string
            fileContent = fileStringStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout <<"ERROR::SHADER::FILE: " << filepath << "/t NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        return fileContent;
    }

private:
    GLuint programID;
    bool initialized;
};
#endif