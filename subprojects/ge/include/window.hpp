#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

#include <map>
#include <vector>

enum WindowMode {
    GE_WINDOWMODE_WINDOWED,
    GE_WINDOWMODE_FULLSCREEN
};

namespace ge {
    struct Key {
        bool isPressed;
        bool isJustPressed;
        bool isJustReleased;
    };

    class Window {
    private:
        GLFWwindow* _Window;

        unsigned int _Width;
        unsigned int _Height;

        const char* _Title;

        std::map<int, Key> _KeysToCheck;

    public:
        static void initWindowApi();
        static void killWindowApi();

        Window(unsigned int __width, unsigned int __height, const char* __title, WindowMode);
        /*Window();
        void manualInit(unsigned int __width, unsigned int __height, const char* __title, WindowMode);*/

        ~Window();

        void focusContext();

        unsigned int getWidth();
        unsigned int getHeight();

        void update();

        bool isOpen();
        //this function sets another function to be called when the window is resized. this lets you reset the render size to the window size.
        void setResizeCallback(GLFWframebuffersizefun function);

        //these functions take in glfw key codes which look like GLFW_KEY_KEYNAME, you can look them up in the glfw3.h file. (i just didnt want to make a giant translation table)
        void addKeys(std::vector<int>);
        void removeKeys(std::vector<int>);

        const Key& getKey(int key);

        void setTitle(const char*);
    };
}

#endif
