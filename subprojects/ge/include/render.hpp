//this is something called a header file, the compiler (thing that makes it so we can run the code) basically sticks this at the top of the file its included in, like those #include statements down there. this contains the structure of my code.

#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include <window.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <set>
#include <map>


enum ShaderType {
    GE_SHADERTYPE_BASIC_NOCOL_2D,
    GE_SHADERTYPE_TESTMESH,
    GE_SHADERTYPE_BASIC,
    GE_SHADERTYPE_BASIC_TEXTURED
};

enum RenderMode {
    GE_RENDERMODE_2D,
    GE_RENDERMODE_3D
};



namespace ge {

    //this is the vertex data for a square, the static constexpr means that its hardcoded into the compiled code rather than being loaded into ram as a normal variable.
    struct TexturedQuad {
        static constexpr float vertices[] = {
            -.5f, -.5f, 0.f,   0.f, 0.f, //bottom left
            .5f, -.5f, 0.f,   1.f, 0.f, //bottom right
            .5f, .5f, 0.f,   1.f, 1.f, //top right
            -.5f, .5f, 0.f,   0.f, 1.f //top left
        };

        static constexpr unsigned int indices[] = {
            0, 1, 2,
            0, 3, 2
        };
    };

    struct Rect2D {
        static constexpr float vertices[] = {
            0.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            1.f, 1.f, 0.f,
            0.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            1.f, 1.f, 0.f
        };
    };

    class Model {
    public:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        Model(const char* filename);
    };

    //this is a temporary class that holds image data while it is loading into the renderer. also it makes it less of a pain to load and pass the data as the user.

    class Texture {
    public:
        int width;
        int height;
        int chan;

        unsigned char* data;

        GLenum mode;

        Texture(const char* filename);

        ~Texture();
    };



    //this class is used to keep track of and use shaders, which are special scripts that run on the gpu which is a specialized processor thats really good at doing tons of small operations at the exact same time.
    class Shader {
        friend class Renderer;
    private:
        GLuint _ShaderProgram;

        Shader(const char* __vertText, const char* __fragText);

        void use();
        static void unuse();

        void sendMat4(glm::mat4 __mat, const char* __name_s);
        void sendVec3(glm::vec3 __vec, const char* __name_s);
    };


    //WARNING this applies for both Mesh and MeshBasic and their other versions, THIS DOES NOT HAVE RAII FUNCTIONALITY, IF IT GOES OUT OF SCOPE WITHOUT BEING EXPLICITLY FREED IT WILL LEAK OPENGL MEMORY AND CAUSE NULLPTR DEREF WHEN Renderer GOES OUT OF SCOPE.
    //this is used to keep track of the mesh data and shaders of a specific object, it stores a pointer (hard to explain) to a Shader object (that class above this one) as well as the opengl buffers that hold the 3d model.
    struct Mesh {
        friend class Renderer;
    private:
        GLuint _Vao;
        GLuint _Vbo;
    protected:
        Shader* _Shader_p;
    };

    //this is an extended version of the Mesh struct that contains things that allow me to move, rotate, scale, and maybe at textures to it. (textures are just images)
    struct MeshBasic {
        friend class Renderer;
    private:
        GLuint _Vao;
        GLuint _Vbo;
        GLuint _Texture;

        size_t _VertexCount;
    protected:
        Shader* _Shader_p;
        glm::mat4 _Model;
        glm::vec3 _Color;
    };


    //this is a class that inherits the MeshBasic struct. inheritance is weird so just think of it as: this thing now has all of the stuff that MeshBasic has, and it can be treated like it is itself a MeshBasic, which is very useful for me. this class allows the user to easily apply transformations to the mesh at runtime, like movement, scaling and rotating
    class MeshBasicCon : public MeshBasic {
    private:
        glm::vec3 _Position;
        glm::quat _Rotation;
        glm::vec3 _Scale;


    public:
        MeshBasicCon();

        void move(glm::vec3 __vec);
        void rotate(glm::vec3 __vec);
        void scale(glm::vec3 __vec);

        void setColor(glm::vec3 __vec);

        void setPosition(glm::vec3 __vec);
        glm::vec3 getPosition();

        void update();
    };

    //the newest edition to the Mesh collection :3. this one holds texture and index data along with the stuff from erlier iterations, and is designed to use vertex data that has uv data inside it.
    struct MeshBasicTex {
        friend class Renderer;
    private:
        GLuint _Vao;
        GLuint _Vbo;
        GLuint _Ebo;

        GLuint _Texture;

        size_t _VertexCount;
        size_t _IndexCount;

        Shader* _Shader_p;
    protected:

        glm::mat4 _Model;
    };

    class MeshBasicTexCon : public MeshBasicTex{
    private:
        glm::vec3 _Position;
        glm::quat _Rotation;
        glm::vec3 _Scale;


    public:
        MeshBasicTexCon();

        void move(glm::vec3 __vec);
        void rotate(glm::vec3 __vec);
        void scale(glm::vec3 __vec);

        void setPosition(glm::vec3 __vec);
        glm::vec3 getPosition();

        void update();
    };
    //this contains the data relevent to a camera. the view matrix holds the transformation data and the proj hold the matrix that projects all the shapes onto the screen.
    struct Camera {
        friend class Renderer;
    protected:
        glm::mat4 _View;
        glm::mat4 _Proj;

        float fov;
        float aspect;

        float near;
        float far;
    };

    //this is the same as MeshBasicCon but for the camera
    //oh my god im so stupid (this is like 2 weeks after i wrote the above comment). i did not need to make it this way, it doesnt access any gpu resources ;-; god im dumb. oh but it accesses the render mode in the creation, hell. whatever, im keeping it.
    class CameraCon : public Camera {
    private:
        glm::vec3 _Position;
        glm::quat _Rotation;

    public:
        CameraCon();

        void setPosition(glm::vec3 __vec);
        void move(glm::vec3 __vec);

        const glm::vec3& getPosition() const;

        void rotMove(glm::vec3 __vec);

        //warning, this uses euler angles and can cause gimbal lock
        void setRotation(glm::vec3 __vec);
        void rotate(glm::vec3 __vec);

        void setFov(float __fov);

        void lookAt(glm::vec3 __pos, glm::vec3 __vec, glm::vec3 __up);

        void update();
    };
    //this is the renderer class. i designed it to be sort of added to a Window object. it manages all of the opengl memory allocation (memory is what your computer stores things in. because im working at a low level, i have to manually manage the memory that my program is using) as well as the drawing of objects (putting it on the screen).
	class Renderer {
    private:

        Window* _Window_p;

        std::set<Mesh*> _Meshs;

        std::set<MeshBasic*> _MeshsBasic;

        std::set<MeshBasicTex*> _MeshsBasicTex;

        std::map<ShaderType, Shader> _Shaders;

        RenderMode _RenderMode;

    public:
        static void initGraphicsApi();

        Renderer(Window*, RenderMode);
        /*Renderer();
        void manualInit(Window*, RenderMode);*/

        ~Renderer();

        void createShader(ShaderType);
        void destroyShader(ShaderType);

        void createRedTestTriangle(Mesh* __mesh_p);
        void destroyMesh(Mesh* __mesh_p);

        void createMeshBasic(MeshBasic* __mesh_p, const std::vector<float>& vertices);
        void destroyMeshBasic(MeshBasic* __mesh_p);

        void createMeshBasicTex(MeshBasicTex* __mesh_p, const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const Texture* texture);
        void destroyMeshBasicTex(MeshBasicTex* __mesh_p);

        void createCamera(Camera* __camera_p);
        void destroyCamera(Camera* __camera_p);

        //please for the love of god dont call these functions on meshes initialized by a different renderer, the results will be extremely weird and opengl will not tell you about it.
        void drawMesh(Mesh* __mesh_p);

        void drawMeshBasic(MeshBasic* __mesh_p, const Camera& __camera_r);

        void drawMeshBasicTex(MeshBasicTex* __mesh_p, const Camera& __camera_r);

        void setClearColor(float, float, float, float);
        void clear();

        static void setViewport(int px, int py, int sx, int sy);
    };
}

#endif
