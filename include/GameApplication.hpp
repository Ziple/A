#ifndef __GAMEAPP_HPP__
#define __GAMEAPP_HPP__

#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace kT
{
    class OGL3Device;
    class OGL3ImmediateContext;
    class OGL3Shader;
    class OGL3Program;
    class OGL3HardwareBuffer;
    class OGL3InputLayout;
    class OGL3Texture;
}

class GameApplication
{
public:

    GameApplication();

    ~GameApplication();

    void LoadShaders();

    void DeleteShaders();

    void Run();

    void Update( float dt );

    void Draw( float dt );

protected:

    SDL_Window *fenetre;
    SDL_GLContext contexteOpenGL;

    kT::OGL3Device* myDevice;
    kT::OGL3ImmediateContext* myContext;

    kT::OGL3InputLayout* myLayout;
    size_t myOffsets[16];
    size_t myStrides[16];

    kT::OGL3HardwareBuffer* myVertexBuffer;
    kT::OGL3HardwareBuffer* myCenterBuffer;
    kT::OGL3HardwareBuffer* myIndexBuffer;

    glm::mat4x4 modelview;
    glm::mat4x4 projection;

    kT::OGL3Shader* vs;
    kT::OGL3Shader* ps;
    kT::OGL3Program* prog;

    GLuint myModelviewUniformIndex;
    GLuint myProjUniformIndex;

    size_t numParticles;
};

#endif // __GAMEAPP_HPP__