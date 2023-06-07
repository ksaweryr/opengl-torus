#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define extern_shader(name) extern "C" unsigned char _ ## name ## _src[];\
const char* name ## _src = reinterpret_cast<const char*>(_ ## name ## _src);

extern_shader(vertex)
extern_shader(geometry)
extern_shader(fragment)

constexpr int R = 360;
constexpr int r = 20;
constexpr int framerate = 144;

class Program {
private:
    inline static std::shared_ptr<Program> instance = nullptr;
    GLFWwindow* window;
    int width, height;
    GLuint vao, vbo, shaderProgram;
public:
    static std::shared_ptr<Program> getInstance() {
        if(instance.get() == nullptr) {
            instance = std::make_shared<Program>();
        }

        return instance;
    }

    Program() : width(500), height(500) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(500, 500, "Torus", nullptr, nullptr);
        if(window == nullptr) {
            const char* msg = nullptr;
            glfwGetError(&msg);
            throw std::runtime_error(std::string(msg));
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowSizeCallback(window, [](__attribute__((unused)) GLFWwindow* _, int width, int height) { Program::getInstance()->onResize(width, height); });
        glViewport(0, 0, width, height);
        GLenum err = glewInit();

        if(err != GLEW_OK) {
            throw std::runtime_error(reinterpret_cast<const char*>(glewGetErrorString(err)));
        }

        initializeGlArrays();
        initializeGlShaders();
    }

    void onResize(int width, int height) {
        this->width = width;
        this->height = height;
        int size = std::min(width, height);
        glViewport((width - size) >> 1, (height - size) >> 1, size, size);
    }

    void initializeGlArrays() {
        GLuint dummy = 0;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint), &dummy, GL_STATIC_DRAW);
    }

    void initializeGlShaders() {
        GLuint vertex_shader, fragment_shader, geometry_shader;
        GLint ok;
        char log[512];

        vertex_shader = compileGlShader(GL_VERTEX_SHADER, vertex_src);
        geometry_shader = compileGlShader(GL_GEOMETRY_SHADER, geometry_src);
        fragment_shader = compileGlShader(GL_FRAGMENT_SHADER, fragment_src);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertex_shader);
        glAttachShader(shaderProgram, geometry_shader);
        glAttachShader(shaderProgram, fragment_shader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
        if(!ok) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
            throw std::runtime_error(std::string(log));
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(geometry_shader);
        glDeleteShader(fragment_shader);
    }

    GLuint compileGlShader(GLenum type, const char* code) {
        GLuint shader = glCreateShader(type);
        GLint len = std::strlen(code);
        glShaderSource(shader, 1, &code, &len);
        glCompileShader(shader);

        GLint ok;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if(!ok) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            throw std::runtime_error(std::string(log));
        }

        return shader;
    }

    void run() {
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            glfwSwapBuffers(window);

            glClearColor(.0f, .0f, .0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shaderProgram);
            glBindVertexArray(vao);
            setUniform("time", static_cast<float>(glfwGetTime()));
            setUniform("R", R);
            setUniform("r", r);
            glDrawArraysInstanced(GL_POINTS, 0, 2, R * r);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / framerate));
        }
    }

    void setUniform(const char* name, int value) {
        GLint loc = glGetUniformLocation(shaderProgram, name);
        glUniform1i(loc, value);
    }

    void setUniform(const char* name, float value) {
        GLint loc = glGetUniformLocation(shaderProgram, name);
        glUniform1f(loc, value);
    }

    ~Program() {
        if(window != nullptr) {
            glfwDestroyWindow(window);
        }

        glfwTerminate();
    }
};

int main() {
    try {
        Program::getInstance()->run();
    } catch(std::runtime_error const& e) {
        std::cerr << "An error occured! " << e.what() << std::endl;
    }
}