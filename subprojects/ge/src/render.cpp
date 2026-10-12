#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtc/quaternion.hpp"
#include <iostream>
#include <netdb.h>
#include <render.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct MeshShaderText {
    constexpr static char* const vs = (char* const)R"(
        #version 330 core

        layout(location = 0) in vec3 aPos;

        out vec4 fragColor;

        void main() {
            gl_Position = vec4(aPos, 1.0);
            fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
        )";
    constexpr static char* const fs = (char* const)R"(
        #version 330 core

        in vec4 fragColor;

        out vec4 color;

        void main() {
            color = fragColor;
        }
        )";
};

struct MeshBasicShaderText {
    constexpr static char* const vs = (char* const)R"(
        #version 330 core

        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNorm;
        layout(location = 2) in vec2 aCoord;

        uniform mat4 model;
        uniform mat4 proj;
        uniform mat4 view;
        uniform vec3 uColor;

        out vec2 coord;
        out vec3 normal;
        out vec3 color;

        void main() {
            coord = aCoord;
            normal = aNorm;
            color = uColor;
            gl_Position = proj * view * model * vec4(aPos, 1.0);
        }
        )";
    constexpr static char* const nlntfs = (char* const)R"(
        #version 330 core

        in vec3 color;

        out vec4 fragColor;

        void main() {
            fragColor = vec4(color, 1.0);
        }
    )";

    constexpr static char* const nltfs = (char* const)R"(
        #version 330 core

        in vec2 coord;

        uniform sampler2D uTexture;

        out vec4 color;

        void main() {
            color = texture(uTexture, coord);
        }

    )";

};

namespace ge {
    Texture::Texture(const char* fileName) {
        int w, h, channels;

        stbi_set_flip_vertically_on_load(true);
        unsigned char* d = stbi_load(fileName, &w, &h, &channels, 0);



        if (!d) throw std::runtime_error("it appears that the image didnt FUCKING work :3\n");

        data = d;
        height = h;
        width = w;
        chan = channels;

        if (channels == 3) {
            mode = GL_RGB;
        } else if (channels == 4) {
            mode = GL_RGBA;
        } else {
            //std::stringstream s;
            //s << "yo, this image is fucken goofy man. i was expecting RGB or RGBA but this has fucken " << channels << " channels\n";
            throw std::runtime_error("yo this image is goofy man\n");
        }
    }

    Texture::~Texture() {
        stbi_image_free(data);
    }

    Model::Model(const char* filename) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
        if (scene == NULL) throw std::runtime_error("oh fuck, your model didnt load, idk bro\n");



        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];

            if (!mesh->HasTextureCoords(0)) throw std::runtime_error("this class expects texcoords and this model does not have any\n");
            unsigned int offset = vertices.size() / 5;
            for (int i = 0; i < mesh->mNumVertices; i++) {
                vertices.push_back(mesh->mVertices[i].x);
                vertices.push_back(mesh->mVertices[i].y);
                vertices.push_back(mesh->mVertices[i].z);

                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }

            for (int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];

                for (int i = 0; i < face.mNumIndices; i++) {
                    indices.push_back(face.mIndices[i] + offset);
                }
            }
        }
        importer.FreeScene();
    }


    Shader::Shader(const char* __vertText, const char* __fragText) {
        _ShaderProgram = glCreateProgram();

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vs, 1, &__vertText, NULL);
        glShaderSource(fs, 1, &__fragText, NULL);

        glCompileShader(vs);
        glCompileShader(fs);

        glAttachShader(_ShaderProgram, vs);
        glAttachShader(_ShaderProgram, fs);

        glLinkProgram(_ShaderProgram);

        int success;
        char infoLog[1024];
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE) {

            glGetShaderInfoLog(vs, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
            infoLog << std::endl;

        }

        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE) {

            glGetShaderInfoLog(fs, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
            infoLog << std::endl;

        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void Shader::use() {
        glUseProgram(_ShaderProgram);
    }
    void Shader::unuse() {
        glUseProgram(0);
    }

    void Shader::sendMat4(glm::mat4 __mat, const char* __name_s) {
        GLint ul = glGetUniformLocation(_ShaderProgram, __name_s);
        glUniformMatrix4fv(ul, 1, GL_FALSE, glm::value_ptr(__mat));
    }
    void Shader::sendVec3(glm::vec3 __vec, const char* __name_s) {
        GLint ul = glGetUniformLocation(_ShaderProgram, __name_s);
        glUniform3fv(ul, 1, glm::value_ptr(__vec));
    }

    MeshBasicCon::MeshBasicCon() {
        _Position = glm::vec3(0);
        _Rotation = glm::quat(1, 0, 0, 0);
        _Scale = glm::vec3(1);
    }

    void MeshBasicCon::move(glm::vec3 __vec) {
        _Position += __vec;
    }
    void MeshBasicCon::rotate(glm::vec3 __vec) {
        glm::quat q1 = glm::angleAxis(glm::radians(__vec.x), glm::vec3(1, 0, 0));
        glm::quat q2 = glm::angleAxis(glm::radians(__vec.y), glm::vec3(0, 1, 0));
        glm::quat q3 = glm::angleAxis(glm::radians(__vec.z), glm::vec3(0, 0, 1));

        _Rotation *= q1 * q2 * q3;
    }
    void MeshBasicCon::scale(glm::vec3 __vec) {
        _Scale += __vec;
    }

    void MeshBasicCon::setColor(glm::vec3 __vec) {
        _Color = __vec;
    }
    void MeshBasicCon::setPosition(glm::vec3 __vec) {
        _Position = __vec;
    }
    glm::vec3 MeshBasicCon::getPosition() {
        return _Position;
    }
    void MeshBasicCon::update() {
        glm::mat4 rot = glm::mat4_cast(_Rotation);

        glm::mat4 trans = glm::translate(glm::mat4(1), _Position);

        glm::mat4 sca = glm::scale(glm::mat4(1), _Scale);

        _Model = trans * rot * sca;
    }
    MeshBasicTexCon::MeshBasicTexCon() {
        _Position = glm::vec3(0);
        _Rotation = glm::quat(1, 0, 0, 0);
        _Scale = glm::vec3(1);

        _Model = glm::mat4(1);
    }

    void MeshBasicTexCon::move(glm::vec3 __vec) {
        _Position += __vec;
    }
    void MeshBasicTexCon::rotate(glm::vec3 __vec) {
        glm::quat q1 = glm::angleAxis(glm::radians(__vec.x), glm::vec3(1, 0, 0));
        glm::quat q2 = glm::angleAxis(glm::radians(__vec.y), glm::vec3(0, 1, 0));
        glm::quat q3 = glm::angleAxis(glm::radians(__vec.z), glm::vec3(0, 0, 1));

        _Rotation *= q1 * q2 * q3;
    }
    void MeshBasicTexCon::scale(glm::vec3 __vec) {
        _Scale += __vec;
    }
    void MeshBasicTexCon::setPosition(glm::vec3 __vec) {
        _Position = __vec;
    }
    glm::vec3 MeshBasicTexCon::getPosition() {
        return _Position;
    }
    void MeshBasicTexCon::update() {
        glm::mat4 rot = glm::mat4_cast(_Rotation);

        glm::mat4 trans = glm::translate(glm::mat4(1), _Position);

        glm::mat4 sca = glm::scale(glm::mat4(1), _Scale);

        _Model = trans * rot * sca;
    }

    CameraCon::CameraCon() {
        _Position = glm::vec3(0, 0, 0);
        _Rotation = glm::quat(1, 0, 0, 0);
    }

    void CameraCon::setPosition(glm::vec3 __vec) {
        _Position = __vec;
    }
    void CameraCon::move(glm::vec3 __vec) {
        _Position += __vec;
    }

    const glm::vec3& CameraCon::getPosition() const {
        return _Position;
    }

    void CameraCon::rotMove(glm::vec3 __vec) {
        _Position += _Rotation * __vec;
    }

    void CameraCon::setFov(float __fov) {
        fov = __fov;

        _Proj = glm::perspective(glm::radians(fov), aspect, near, far);
    }

    void CameraCon::lookAt(glm::vec3 __pos, glm::vec3 vec, glm::vec3 up) {
        _View = glm::lookAt(__pos, vec, up);
    }

    void CameraCon::setRotation(glm::vec3 __vec) {
        glm::quat qx = glm::angleAxis(glm::radians(__vec.x), glm::vec3(1, 0, 0));
        glm::quat qy = glm::angleAxis(glm::radians(__vec.y), glm::vec3(0, 1, 0));
        glm::quat qz = glm::angleAxis(glm::radians(__vec.z), glm::vec3(0, 0, 1));

        _Rotation = qx * qy * qz;
    }

    void CameraCon::rotate(glm::vec3 __vec) {
        glm::quat qx = glm::angleAxis(glm::radians(__vec.x), glm::vec3(1, 0, 0));
        glm::quat qy = glm::angleAxis(glm::radians(__vec.y), glm::vec3(0, 1, 0));
        glm::quat qz = glm::angleAxis(glm::radians(__vec.z), glm::vec3(0, 0, 1));

        _Rotation *= qx * qy * qz;
    }

    void CameraCon::update() {
        glm::mat4 rot = glm::mat4_cast(_Rotation);

        glm::mat4 trans = glm::translate(glm::mat4(1), _Position);

        _View = glm::inverse(trans * rot);
    }

    void Renderer::initGraphicsApi() {
        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    }
    Renderer::Renderer(Window* __window, RenderMode __renderMode) {
        _Window_p = __window;
        glViewport(0, 0, _Window_p->getWidth(), _Window_p->getHeight());

        _RenderMode = __renderMode;

        if (__renderMode == GE_RENDERMODE_3D) {
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
        }
    }
    /*void Renderer::manualInit(Window* __window, RenderMode __renderMode) {
        _Window_p = __window;
        glViewport(0, 0, _Window_p->getWidth(), _Window_p->getHeight());

        _RenderMode = __renderMode;

        if (__renderMode == GE_RENDERMODE_3D) {
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
        }
    }
    Renderer::Renderer() {
        _Window_p = nullptr;
    }*/

    Renderer::~Renderer() {
        std::cout << "hello \n";
        if (_Shaders.size() != 0) {
            for (auto& [type, shader] : _Shaders) {
                glDeleteProgram(shader._ShaderProgram);
            }
        }

        for (Mesh* i : _Meshs) {
            glDeleteBuffers(1, &i->_Vbo);
            glDeleteVertexArrays(1, &i->_Vao);
        }

        for (MeshBasic* i : _MeshsBasic) {
            glDeleteBuffers(1, &i->_Vbo);
            glDeleteVertexArrays(1, &i->_Vao);
        }

        for (MeshBasicTex* i : _MeshsBasicTex) {
            glDeleteBuffers(1, &i->_Ebo);
            glDeleteBuffers(1, &i->_Vbo);
            glDeleteVertexArrays(1, &i->_Vao);

            glDeleteTextures(1, &i->_Texture);
        }
    }

    void Renderer::createShader(ShaderType __sType) {
        switch (__sType) {
            case GE_SHADERTYPE_TESTMESH:
            {
                Shader shader(MeshShaderText::vs, MeshShaderText::fs);

                _Shaders.insert({GE_SHADERTYPE_TESTMESH, shader});
            } break;

            case GE_SHADERTYPE_BASIC_NOCOL_2D:
                std::cout << "also havent done this one yet\n";
                break;

            case GE_SHADERTYPE_BASIC:
            {
                Shader shader(MeshBasicShaderText::vs, MeshBasicShaderText::nlntfs);

                _Shaders.insert({GE_SHADERTYPE_BASIC, shader});
            } break;
            case GE_SHADERTYPE_BASIC_TEXTURED:
            {
                Shader shader(MeshBasicShaderText::vs, MeshBasicShaderText::nltfs);

                _Shaders.insert({GE_SHADERTYPE_BASIC_TEXTURED, shader});
            } break;

        }
    }
    void Renderer::destroyShader(ShaderType sType) {
        auto tmp = _Shaders.find(sType);

        if (tmp != _Shaders.end()) {
            Shader* shader = &tmp->second;

            glDeleteProgram(shader->_ShaderProgram);

            _Shaders.erase(sType);
        }
    }

    void Renderer::createRedTestTriangle(Mesh* __mesh_p) {
        float vertices[] = {-.5f, -.5f, 0.f,
                            0.f, .5f, 0.f,
                            .5f, -.5f, 0.f
        };

        auto tmp = _Shaders.find(GE_SHADERTYPE_TESTMESH);
        if (tmp == _Shaders.end()) {
            Renderer::createShader(GE_SHADERTYPE_TESTMESH);
        }

        tmp = _Shaders.find(GE_SHADERTYPE_TESTMESH);
        __mesh_p->_Shader_p = &tmp->second;

        glGenVertexArrays(1, &__mesh_p->_Vao);
        glGenBuffers(1, &__mesh_p->_Vbo);

        glBindVertexArray(__mesh_p->_Vao);
        glBindBuffer(GL_ARRAY_BUFFER, __mesh_p->_Vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        _Meshs.insert(__mesh_p);
    }

    void Renderer::destroyMesh(Mesh* __mesh_p) {
        _Meshs.erase(__mesh_p);

        glDeleteBuffers(1, &__mesh_p->_Vbo);
        glDeleteVertexArrays(1, &__mesh_p->_Vao);
    }

    void Renderer::createMeshBasic(MeshBasic* __mesh_p, const std::vector<float>& vertices) {
        auto tmp = _Shaders.find(GE_SHADERTYPE_BASIC);

        if (tmp == _Shaders.end()) {
            Shader shader(MeshBasicShaderText::vs, MeshBasicShaderText::nlntfs);
            _Shaders.insert({GE_SHADERTYPE_BASIC, shader});
        }

        tmp = _Shaders.find(GE_SHADERTYPE_BASIC);

        __mesh_p->_Shader_p = &tmp->second;

        __mesh_p->_VertexCount = vertices.size() / 3;



        glGenVertexArrays(1, &__mesh_p->_Vao);
        glGenBuffers(1, &__mesh_p->_Vbo);

        glBindVertexArray(__mesh_p->_Vao);
        glBindBuffer(GL_ARRAY_BUFFER, __mesh_p->_Vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STREAM_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        __mesh_p->_Color = glm::vec3(1, 1, 1);
        __mesh_p->_Model = glm::mat4(1);


        _MeshsBasic.insert(__mesh_p);
    }
    void Renderer::destroyMeshBasic(MeshBasic* __mesh_p) {
        _MeshsBasic.erase(__mesh_p);

        glDeleteBuffers(1, &__mesh_p->_Vbo);
        glDeleteVertexArrays(1, &__mesh_p->_Vao);
    }

    void Renderer::createMeshBasicTex(MeshBasicTex* __mesh_p, const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const Texture* texture) {

        auto tmp = _Shaders.find(GE_SHADERTYPE_BASIC_TEXTURED);

        if (tmp == _Shaders.end()) {
            Shader shader(MeshBasicShaderText::vs, MeshBasicShaderText::nltfs);
            _Shaders.insert({GE_SHADERTYPE_BASIC_TEXTURED, shader});
        }

        tmp = _Shaders.find(GE_SHADERTYPE_BASIC_TEXTURED);

        __mesh_p->_Shader_p = &tmp->second;


        __mesh_p->_IndexCount = indices.size();
        __mesh_p->_VertexCount = vertices.size() / 5;

        glGenVertexArrays(1, &__mesh_p->_Vao);

        glGenBuffers(1, &__mesh_p->_Vbo);
        glGenBuffers(1, &__mesh_p->_Ebo);
        glGenTextures(1, &__mesh_p->_Texture);

        glBindVertexArray(__mesh_p->_Vao);

        glBindBuffer(GL_ARRAY_BUFFER, __mesh_p->_Vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __mesh_p->_Ebo);

        glBindTexture(GL_TEXTURE_2D, __mesh_p->_Texture);


        if (texture == NULL) {
            unsigned char data[] = {255, 0, 178, 255};

            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

        } else {
            if (texture->chan < 4) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            else glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            glTexImage2D(GL_TEXTURE_2D, 0, texture->mode, texture->width, texture->height, 0, texture->mode, GL_UNSIGNED_BYTE, texture->data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }




        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STREAM_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(2);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        _MeshsBasicTex.insert(__mesh_p);
    }
    void Renderer::destroyMeshBasicTex(MeshBasicTex* __mesh_p) {
        glDeleteBuffers(1, &__mesh_p->_Ebo);
        glDeleteBuffers(1, &__mesh_p->_Vbo);
        glDeleteTextures(1, &__mesh_p->_Texture);
        glDeleteVertexArrays(1, &__mesh_p->_Vao);

        _MeshsBasicTex.erase(__mesh_p);
    }

    void Renderer::createCamera(Camera* __camera_p) {
        switch (_RenderMode) {
            case GE_RENDERMODE_2D:
                __camera_p->_Proj = glm::ortho(0.f, (float)_Window_p->getWidth(), 0.f, (float)_Window_p->getHeight());
                __camera_p->_View = glm::mat4(1);
                break;

            case GE_RENDERMODE_3D:
                __camera_p->fov = 90.f;
                __camera_p->near = .01f;
                __camera_p->far = 100.f;
                __camera_p->aspect = (float)_Window_p->getWidth()/(float)_Window_p->getHeight();

                __camera_p->_Proj = glm::perspective(glm::radians(__camera_p->fov), __camera_p->aspect, __camera_p->near, __camera_p->far);
                __camera_p->_View = glm::mat4(1);
        }
    }

    void Renderer::drawMesh(Mesh* __mesh_p) {
        glBindVertexArray(__mesh_p->_Vao);
        __mesh_p->_Shader_p->use();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        Shader::unuse();
        glBindVertexArray(0);
    }

    void Renderer::drawMeshBasic(MeshBasic* __mesh_p, const Camera& __camera_r) {
        glBindVertexArray(__mesh_p->_Vao);
        __mesh_p->_Shader_p->use();
        #include "GLFW/glfw3.h"
        __mesh_p->_Shader_p->sendMat4(__mesh_p->_Model, "model");
        __mesh_p->_Shader_p->sendVec3(__mesh_p->_Color, "uColor");

        __mesh_p->_Shader_p->sendMat4(__camera_r._View, "view");
        __mesh_p->_Shader_p->sendMat4(__camera_r._Proj, "proj");


        glDrawArrays(GL_TRIANGLES, 0, __mesh_p->_VertexCount);

        glBindVertexArray(0);
        Shader::unuse();
    }

    void Renderer::drawMeshBasicTex(MeshBasicTex* __mesh_p, const Camera& __camera_r) {
        glBindVertexArray(__mesh_p->_Vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, __mesh_p->_Texture);

        __mesh_p->_Shader_p->use();

        __mesh_p->_Shader_p->sendMat4(__mesh_p->_Model, "model");
        __mesh_p->_Shader_p->sendMat4(__camera_r._Proj, "proj");
        __mesh_p->_Shader_p->sendMat4(__camera_r._View, "view");

        glDrawElements(GL_TRIANGLES, __mesh_p->_IndexCount, GL_UNSIGNED_INT, 0);

        Shader::unuse();

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::setClearColor(float r, float g, float b, float a) {
        if (r > 1) throw std::runtime_error("R value of Renderer::setClearColor is not normalized");
        if (g > 1) throw std::runtime_error("G value of Renderer::setClearColor is not normalized");
        if (b > 1) throw std::runtime_error("B value of Renderer::setClearColor is not normalized");
        if (a > 1) throw std::runtime_error("A value of Renderer::setClearColor is not normalized");

        glClearColor(r, g, b, a);
    }

    void Renderer::clear() {
        switch (_RenderMode) {
            case GE_RENDERMODE_3D:
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            case GE_RENDERMODE_2D:
                glClear(GL_COLOR_BUFFER_BIT);
        }
    }

    void Renderer::setViewport(int px, int py, int sx, int sy) {
        glViewport(px, py, sx, sy);
    }
}
