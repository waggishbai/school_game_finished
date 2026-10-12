#include <engine.hpp>

namespace ge {

    Engine::Engine(int w, int h, const char* name, WindowMode wm, RenderMode rm) {
        Window::initWindowApi();
        _Window.manualInit(w, h, name, wm);
        _Window.focusContext();

        Renderer::initGraphicsApi();
        _Render.manualInit(&_Window, rm);
    }

    void Engine::update() {
        _Window.update();
        _Render.clear();
    }

    bool Engine::isOpen() {
        return _Window.isOpen();
    }
}
