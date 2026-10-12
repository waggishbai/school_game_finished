//#include "GLFW/glfw3.h"
#include <iostream>
#include <window.hpp>


namespace ge {
    void Window::initWindowApi() {
        if (!glfwInit()) {
            std::cerr << "glewInit failed\n";
        }
    }
    void Window::killWindowApi() {
        glfwTerminate();
    }

    Window::Window(unsigned int __width, unsigned int __height, const char* __title, WindowMode __windowMode) {
        switch (__windowMode) {
            case GE_WINDOWMODE_WINDOWED:
                _Window = glfwCreateWindow(__width, __height, __title, NULL, NULL);
                _Width = __width;
                _Height = __height;
                _Title = __title;
                break;
            case GE_WINDOWMODE_FULLSCREEN:
                std::cout << "fullscreen is not supported yet\n";
                _Window = nullptr;
                break;
        }

    }

    Window::~Window() {
        glfwDestroyWindow(_Window);
        std::cout << "uhh\n";
    }

    void Window::focusContext() {
        glfwMakeContextCurrent(_Window);
    }

    unsigned int Window::getWidth() {
        return _Width;
    }
    unsigned int Window::getHeight() {
        return _Height;
    }

    void Window::update() {
        glfwPollEvents();
        glfwSwapBuffers(_Window);

        for (auto& i : _KeysToCheck) {
            int state = glfwGetKey(_Window, i.first);

            if (state == GLFW_PRESS) {

                if (i.second.isJustPressed) {
                    i.second.isJustPressed = false;
                }
                if (!i.second.isPressed) {
                    i.second.isJustPressed = true;
                }

                i.second.isPressed = true;
            }

            if (state == GLFW_RELEASE) {

                if (i.second.isJustReleased) {
                    i.second.isJustReleased = false;
                }
                if (i.second.isPressed) {
                    i.second.isJustReleased = true;
                }

                i.second.isPressed = false;
            }
        }
    }

    bool Window::isOpen() {
        return !glfwWindowShouldClose(_Window);
    }
    void Window::setResizeCallback(GLFWframebuffersizefun function) {
        glfwSetFramebufferSizeCallback(_Window, function);
    }
    //these functions take in glfw key codes which look like GLFW_KEY_KEYNAME, you can look them up in the glfw3.h file. (i just didnt want to make a giant translation table)
    void Window::addKeys(std::vector<int> keys) {
        for (int i = 0; i < keys.size(); i++) {
            Key key;
            key.isJustPressed = false;
            key.isJustReleased = false;
            key.isPressed = false;

            auto fmt = _KeysToCheck.find(keys[i]);
            if (fmt == _KeysToCheck.end()) {
                _KeysToCheck.insert({keys[i], key});
            }
        }
    }
    void Window::removeKeys(std::vector<int> keys) {
        for (int i = 0; i < keys.size(); i++) {
            _KeysToCheck.erase(keys[i]);
        }
    }

    const Key& Window::getKey(int key) {
        try {
            const Key& k = _KeysToCheck.at(key);
            return k;
        } catch (std::out_of_range) {
            throw std::runtime_error("requested uninitialized key\n");
        }
    }

    void Window::setTitle(const char* __title) {
        glfwSetWindowTitle(_Window, __title);
    }
}
