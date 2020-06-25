#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "OpenGLShader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

OpenGLShader::OpenGLShader(const std::string& filepath)
{

}

OpenGLShader::OpenGLShader(const std::string& _name, const std::string& vertexPath, const std::string& fragmentPath)
    : name(_name)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        // TODO Add an error shader in case something doesnt compile?
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    Compile(vertexCode, fragmentCode);
}

void OpenGLShader::Compile(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    const char* vertSrc = vertexSrc.c_str();
    const char* fragSrc = fragmentSrc.c_str();

    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertSrc, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragSrc, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");
    // shader Program
    renderID = glCreateProgram();
    glAttachShader(renderID, vertex);
    glAttachShader(renderID, fragment);
    glLinkProgram(renderID);
    CheckCompileErrors(renderID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// TODO use enums for shader types
void OpenGLShader::CheckCompileErrors(uint32_t id, const std::string& type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(id, 1024, NULL, infoLog);
            //std::cout << "(" << vertexName << ", " << fragName << "): " << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 1024, NULL, infoLog);
            //std::cout << "(" << vertexName << ", " << fragName << "): " << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}


void OpenGLShader::SetBool(const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(renderID, name.c_str()), static_cast<int>(value));
}

void OpenGLShader::SetInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(renderID, name.c_str()), value);
}

void OpenGLShader::SetIntArray(const std::string &name, int* values, uint32_t count)
{

}

void OpenGLShader::SetFloat (const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(renderID, name.c_str()), value);
}

void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3& value)
{
    glUniform3f(glGetUniformLocation(renderID, name.c_str()), value[0], value[1], value[2]);
}

void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4& value)
{
    glUniform4f(glGetUniformLocation(renderID, name.c_str()), value[0], value[1], value[2], value[3]);
}

void OpenGLShader::SetMat4(const std::string &name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(renderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLShader::Bind()
{
    glUseProgram(renderID);
}

void OpenGLShader::Unbind()
{
}


