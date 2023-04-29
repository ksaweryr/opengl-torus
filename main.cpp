#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern "C" unsigned char _vertex_src[];
extern "C" unsigned char _fragment_src[];

const char* vertex_src = reinterpret_cast<const char*>(_vertex_src);
const char* fragment_src = reinterpret_cast<const char*>(_fragment_src);

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
        glfwSetWindowSizeCallback(window, [](GLFWwindow* _, int width, int height) { Program::getInstance()->onResize(width, height); });
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
        std::vector<GLuint> v(R * r * 2);

        for(int i = 0; i < R; i++) {
            for(int j = 0; j < r; j++) {
                int idx = (r * i + j) << 1;
                v[idx] = i;
                v[idx + 1] = j;
            }
        }

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(GLuint), v.data(), GL_STATIC_DRAW);
        glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, 2 * sizeof(GLuint), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void initializeGlShaders() {
        GLuint vertex_shader, fragment_shader;
        GLint ok;
        char log[512];

        vertex_shader = compileGlShader(GL_VERTEX_SHADER, vertex_src);
        fragment_shader = compileGlShader(GL_FRAGMENT_SHADER, fragment_src);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertex_shader);
        glAttachShader(shaderProgram, fragment_shader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
        if(!ok) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
            throw std::runtime_error(std::string(log));
        }

        glDeleteShader(vertex_shader);
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
            glDrawArrays(GL_POINTS, 0, R * r);
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