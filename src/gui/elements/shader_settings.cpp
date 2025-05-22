#include <fstream>
#include <sstream>
#include <gui/elements.hpp>
#include <gui/context.hpp>
#include <config.hpp>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <thread>
#include <hooks.hpp>
#include <cstring>
#include <iostream>

namespace SSTux::GUI
{
    static char shaderPathBuffer[512] = "";
    static std::string shaderStatus = "No shader loaded.";
    static GLuint customShaderProgram = 0;

    static void Log(const std::string &message)
    {
        std::cerr << "[SSTux] " << message << std::endl;
    }

    void RenderShaderSettings()
    {
        if (customShaderProgram)
        {
            glUseProgram(customShaderProgram);
        }

        ImGui::Begin("Shader Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::InputText("Shader Path", shaderPathBuffer, sizeof(shaderPathBuffer));
        if (ImGui::Button("Load Shader"))
        {
            Log("Attempting to load shader from: " + std::string(shaderPathBuffer));
            std::ifstream file(shaderPathBuffer);
            if (!file)
            {
                shaderStatus = "Failed to open file.";
                Log(shaderStatus);
            }
            else
            {
                std::stringstream shaderSource;
                shaderSource << file.rdbuf();
                std::string source = shaderSource.str();
                Log("Shader source loaded, compiling...");

                GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
                const char *src = source.c_str();
                glShaderSource(shader, 1, &src, nullptr);
                glCompileShader(shader);

                GLint success = 0;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    char infoLog[512];
                    glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
                    shaderStatus = std::string("Shader compile error: ") + infoLog;
                    Log(shaderStatus);
                    glDeleteShader(shader);
                }
                else
                {
                    Log("Shader compiled successfully.");

                    if (customShaderProgram)
                        glDeleteProgram(customShaderProgram);

                    customShaderProgram = glCreateProgram();
                    glAttachShader(customShaderProgram, shader);
                    glLinkProgram(customShaderProgram);
                    glDeleteShader(shader);

                    GLint linkSuccess = 0;
                    glGetProgramiv(customShaderProgram, GL_LINK_STATUS, &linkSuccess);
                    if (!linkSuccess)
                    {
                        char infoLog[512];
                        glGetProgramInfoLog(customShaderProgram, sizeof(infoLog), nullptr, infoLog);
                        shaderStatus = std::string("Shader link error: ") + infoLog;
                        Log(shaderStatus);
                        glDeleteProgram(customShaderProgram);
                        customShaderProgram = 0;
                    }
                    else
                    {
                        shaderStatus = "Shader loaded and linked successfully.";
                        Log(shaderStatus);
                    }
                }
            }
        }

        ImGui::TextWrapped("Status: %s", shaderStatus.c_str());
        ImGui::End();
    }
}
