#ifndef ENGINE_H
#define ENGINE_H

#include <render.hpp>



namespace ge {

    class Engine {
    private:
        Renderer _Render;
        Window _Window;

    public:
        Engine(int w, int h, const char* name, WindowMode, RenderMode);

        void update();
        bool isOpen();
    };

}

#endif
