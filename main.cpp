#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern "C" unsigned char _vertex_src[];
extern "C" unsigned int _vertex_src_len;
extern "C" unsigned char _fragment_src[];
extern "C" unsigned int _fragment_src_len;

const char* vertex_src = reinterpret_cast<const char*>(_vertex_src);
int vertex_src_len = _vertex_src_len;
const char* fragment_src = reinterpret_cast<const char*>(_fragment_src);
int fragment_src_len = _fragment_src_len;

constexpr int R = 360;
constexpr int r = 20;

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
        glViewport(0, 0, width, height);
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

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_src, &vertex_src_len);
        glCompileShader(vertex_shader);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
        if(!ok) {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, log);
            throw std::runtime_error(std::string(log));
        }

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_src, &fragment_src_len);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ok);
        if(!ok) {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, log);
            throw std::runtime_error(std::string(log));
        }

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
            setUniform("width", width);
            setUniform("height", height);
            glDrawArrays(GL_POINTS, 0, R * r);
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