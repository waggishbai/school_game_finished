#include <glm/geometric.hpp>
#include <render.hpp>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

constexpr glm::vec3 STARTPOS = {-2.23932, 0.5, 0.121695};
constexpr glm::vec3 ISTARTPOS = {8.01885, 0.5, -0.860417};

constexpr float ISTARTROT = 90;

constexpr glm::vec3 FLOOR2TEL = {8.36735, 0.5, 5.76408};

constexpr glm::vec3 UPTEL = {-8.54114, 0.5, -0.700417};

constexpr glm::vec3 DOWNTEL = {-8.55264, 0.5, 5.76408};

constexpr float FOV = 70;

constexpr glm::vec3 BPOS = {-14.48, .5, -52.43};

constexpr glm::vec3 APOS = {37.86, .5, -0.30};

constexpr glm::vec3 DPOS = {-14.87, 0.5, 52.35};

constexpr glm::vec3 CPOS = {-67.36, .5, 0.38};

constexpr glm::vec3 LETTERTEL[] = {{37.86, .5, -0.30}, {-14.48, .5, -52.43}, {-67.36, .5, 0.38}, {-14.87, 0.5, 52.35}};

constexpr float BSCALE = 5;
constexpr float ISCALE = -.4;


constexpr glm::vec3 FIRSTROOMS[] = {{6.25509f, 0.5f, 1.28901f}, {4.66224f, 0.5f, -2.81607f}, {2.99603f, 0.5f, 1.30428f}, {1.4541f, 0.5f, -2.76451f}, {-0.15889f, 0.5f, 1.30428f}, {-1.78538f, 0.5f, -2.78922f}, {-3.31421f, 0.5f, 1.36749f}, {-4.97638f, 0.5f, -2.78399f}, {-6.55188f, 0.5f, 1.29607f}};

constexpr glm::vec3 SECONDROOMS[] = {{6.23579, 0.5, 7.83716}, {4.61803, 0.5, 3.71341}, {3.04715, 0.5, 7.80233}, {1.38535, 0.5, 3.71654}, {-0.181083, 0.5, 7.80633}, {-1.77894, 0.5, 3.66018}, {-3.333, 0.5, 7.7849}, {-4.95785, 0.5, 3.71615}, {-6.53999, 0.5, 7.79379}};

constexpr float BCOLDIST = .5;
constexpr float RCOLDIST = .5;

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());


    ge::Window::initWindowApi();
    ge::Window window(800, 650, "", GE_WINDOWMODE_WINDOWED);
    window.focusContext();

    window.addKeys({GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, GLFW_KEY_LEFT_SHIFT});

    ge::Renderer::initGraphicsApi();
    ge::Renderer render(&window, GE_RENDERMODE_3D);



    ge::Texture tBuildings("windows.png");
    ge::Model mBuildings("buildings.obj");

    ge::MeshBasicTexCon buildings;

    render.createMeshBasicTex(&buildings, mBuildings.vertices, mBuildings.indices, &tBuildings);

    buildings.scale({BSCALE, BSCALE, BSCALE});
    buildings.update();



    ge::Texture tInterior("/home/waggishbai/projdir/blend/interior.png");
    ge::Model mInterior("/home/waggishbai/projdir/blend/interior.obj");

    ge::MeshBasicTexCon interior;

    render.createMeshBasicTex(&interior, mInterior.vertices, mInterior.indices, &tInterior);

    interior.scale({ISCALE, ISCALE, ISCALE});
    interior.update();



    ge::CameraCon cam;
    render.createCamera(&cam);



    auto camMoveLogic = [&window, &cam]() {

        const float bspeed = .04;
        const float rotSpeed = 2;


        const int sprint = 5;

        float speed = bspeed;

        if (window.getKey(GLFW_KEY_LEFT_SHIFT).isPressed) {
            speed = sprint * bspeed;
        } else {
            speed = bspeed;
        }


        if (window.getKey(GLFW_KEY_A).isPressed) {
            cam.rotMove({-speed, 0, 0});
        } else if (window.getKey(GLFW_KEY_D).isPressed) {
            cam.rotMove({speed, 0, 0});
        }

        if (window.getKey(GLFW_KEY_W).isPressed) {
            cam.rotMove({0, 0, -speed});
        } else if (window.getKey(GLFW_KEY_S).isPressed) {
            cam.rotMove({0, 0, speed});
        }

        if (window.getKey(GLFW_KEY_RIGHT).isPressed) {
            cam.rotate({0, -rotSpeed, 0});
        } else if (window.getKey(GLFW_KEY_LEFT).isPressed) {
            cam.rotate({0, rotSpeed, 0});
        }

        cam.update();
    };

    while (window.isOpen()) {

        unsigned int roomNum = gen() % 9;
        unsigned int floorNum = gen() % 2;
        unsigned int buildingNum = gen() % 4;

        std::stringstream roomStr;

        roomStr << floorNum << "0" << roomNum + 1 << " in building " << (char)(65 + buildingNum);

        std::string text = roomStr.str();

        const char* t = text.c_str();

        window.setTitle(t);

        render.setClearColor(1, 0, 0, 0);

        cam.setPosition(STARTPOS);
        cam.setFov(FOV);
        cam.setRotation({0, ISTARTROT, 0});

        cam.update();

        while (window.isOpen()) {

            render.clear();

            camMoveLogic();

            if (glm::distance(cam.getPosition(), LETTERTEL[buildingNum]) < BCOLDIST) {
                std::cout << "y\n";
                break;
            }

            //std::cout << glm::distance(cam.getPosition(), DPOS) << '\n';
            //const glm::vec3& campos = cam.getPosition();

            //std::cout << campos.x << ", " << campos.y << ", " << campos.z << "\n";


            render.drawMeshBasicTex(&buildings, cam);

            window.update();
        }






        cam.setPosition(ISTARTPOS);
        cam.setRotation({0, ISTARTROT, 0});
        cam.update();



        render.setClearColor(0, 0, 0, 1);

        while (window.isOpen()) {
            render.clear();

            camMoveLogic();

            if (floorNum == 0) {
                if (glm::distance(cam.getPosition(), FIRSTROOMS[roomNum]) < RCOLDIST) {
                    break;
                }
            } else {
                if (glm::distance(cam.getPosition(), SECONDROOMS[roomNum]) < RCOLDIST) {
                    break;
                }
            }

            if (glm::distance(cam.getPosition(), UPTEL) < .5) {
                cam.setPosition(FLOOR2TEL);
                cam.setRotation({0, ISTARTROT, 0});
                cam.update();
            }
            if (glm::distance(cam.getPosition(), DOWNTEL) < .5) {
                cam.setPosition(ISTARTPOS);
                cam.setRotation({0, ISTARTROT, 0});
                cam.update();
            }

            render.drawMeshBasicTex(&interior, cam);

            window.update();
        }
    }

}
