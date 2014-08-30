#include <GameApplication.hpp>

#include <kT/Graphics/OGL3Device/OGL3Device.hpp>
#include <kT/Graphics/OGL3Device/OGL3ImmediateContext.hpp>
#include <kT/Graphics/OGL3Device/OGL3HardwareBuffer.hpp>
#include <kT/Graphics/OGL3Device/OGL3Shader.hpp>
#include <kT/Graphics/OGL3Device/OGL3InputLayout.hpp>
#include <kT/Graphics/OGL3Device/OGL3Texture.hpp>

#include <kT/Core/Clock.hpp>
#include <kT/Math/Matrix4.hpp>

#include <GL/glew.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <kT/Window/Window.hpp>
#include <kT/Graphics/D3D11Device/D3D11Device.hpp>
#include <kT/Graphics/D3D11Device/D3D11ImmediateContext.hpp>
#include <kT/Graphics/D3D11Device/D3D11InputLayout.hpp>
#include <kT/Graphics/D3D11Device/D3D11HardwareBuffer.hpp>
#include <kT/Graphics/D3D11Device/D3D11SwapChain.hpp>
#include <kT/Graphics/D3D11Device/D3D11DepthStencilBuffer.hpp>
#include <kT/Graphics/D3D11Device/D3D11Shader.hpp>
#include <kT/Graphics/GraphicsDevice/ViewportDesc.hpp>

static void APIENTRY debugCallBack(
	GLenum source,
	GLenum type,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	void* user)
{
	std::cout << "[DEBUG]" << message << std::endl;
}

#define N 20
#define M 20

struct Matrices
{
	glm::mat4 Proj;
	glm::mat4 ModelView;
};

struct vert{
    glm::vec4 pos;
    //glm::vec4 col;
};

static float frand( int *seed )
{
    union
    {
        float fres;
        unsigned int ires;
    };

    seed[0] *= 16807;

    ires = ((((unsigned int)seed[0])>>9 ) | 0x3f800000);
    return fres - 1.0f;
}

GameApplication::GameApplication():
    fenetre(0),
    contexteOpenGL(0),
    myDevice( 0 ),
    myContext( 0 ),
    myVertexBuffer(0),
    myIndexBuffer(0),
    myLayout( 0 ),
    vs(0),
    ps(0),
    prog(0),
    myModelviewUniformIndex(0),
    myProjUniformIndex(0)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        throw ;

    fenetre = SDL_CreateWindow("A OGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if(fenetre == 0)
        throw ;

    // Version d'OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    // Double Buffer, depth buffer
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#ifndef NDEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    contexteOpenGL = SDL_GL_CreateContext(fenetre);

    if(contexteOpenGL == 0)
        throw ;

    if( glewInit() != GLEW_OK)
        throw ;

    SDL_GL_MakeCurrent( fenetre, contexteOpenGL );

	glDebugMessageCallbackAMD( debugCallBack, reinterpret_cast<GLvoid*>(this) );

    myContext = new kT::OGL3ImmediateContext(0);

    glm::vec4 vertices[72] = {
            
            // Front face [ 0, 3 ]
         glm::vec4(-1.f, -1.f, 1.f, 1.f ), glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ), glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ), glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(-1.f, -1.f, 1.f, 1.f ), glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ), glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(-1.f, 1.f, 1.f, 1.f ), glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
            
            
            // Right face [ 4, 7 ]
             glm::vec4(1.f, -1.f, -1.f, 1.f ), glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ), glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ), glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ), glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, -1.f, 1.f ), glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ), glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f ),
            

            // Back face [ 8, 11 ]
             glm::vec4(-1.f, 1.f, -1.f, 1.f ), glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(1.f, 1.f, -1.f, 1.f ), glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ), glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(-1.f, 1.f, -1.f, 1.f ), glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ), glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ), glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f ),
            

            // Left face [ 12, 15 ]
             glm::vec4(-1.f, 1.f, 1.f, 1.f ), glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, -1.f, 1.f ), glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ), glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, 1.f, 1.f ), glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ), glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, 1.f, 1.f ), glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
            
            // Top face [ 16, 19 ]
             glm::vec4(-1.f, 1.f, -1.f, 1.f ), glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ), glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, -1.f, 1.f ), glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, -1.f, 1.f ), glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, 1.f, 1.f ), glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ), glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ),
            
            // Bottom face [ 20, 23 ]
             glm::vec4(-1.f, -1.f, -1.f, 1.f ), glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ), glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, 1.f, 1.f ), glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ), glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ), glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ), glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
        };

    myVertexBuffer = new kT::OGL3HardwareBuffer(
        0,
        kT::OGL3HardwareBuffer::VertexBuffer,
        kT::OGL3HardwareBuffer::ImmutableBufferUsage,
        0,
        sizeof(glm::vec4),
        72,
        vertices );

    unsigned indices[36];
    for( size_t i = 0; i < 36; i++ )
        indices[i] = i;

    myIndexBuffer = new kT::OGL3HardwareBuffer(
        0,
        kT::OGL3HardwareBuffer::IndexBuffer,
        kT::OGL3HardwareBuffer::ImmutableBufferUsage,
        0,
        sizeof(unsigned),
        36,
        indices );

    numParticles = 10000;

    vert *inParticles = new vert[ numParticles ];

    int seed = 1214;
    for( size_t i = 0; i < numParticles; i++ )
    {
        float decision = frand(&seed);
		/*
        if( decision < 0.1f )
            inParticles[i].col = glm::vec4( 1.f, 0.0f, 1.f, 1.0f );
        else if( decision < 0.2f )
            inParticles[i].col = glm::vec4( 0.0f, 1.0f, 1.0f, 1.0f );
        else if( decision < 0.3f )
            inParticles[i].col = glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f );
        else if( decision < 0.4f )
            inParticles[i].col = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
        else
            inParticles[i].col = glm::vec4( 1.0f, 1.0f, 1.0f, 1.f );
			*/
        inParticles[i].pos = glm::vec4( 20.0f*(frand(&seed)-0.5f), 20.0f*(frand(&seed)-0.5f), 20.0f*(frand(&seed)-0.5f), 1.0f );
    }

    myCenterBuffer = new kT::OGL3HardwareBuffer(
        0,
        kT::OGL3HardwareBuffer::VertexBuffer,
        kT::OGL3HardwareBuffer::ImmutableBufferUsage,
        0,
        sizeof(vert),
        numParticles,
        inParticles );

    delete[] inParticles;

    LoadShaders();

    projection = glm::perspective(70.0, (double) 800/600, 1.0, 100.0);
    modelview = glm::lookAt( glm::vec3(0.f,0.f, -10.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.f, 1.f, 0.f) );
}

GameApplication::~GameApplication()
{
    DeleteShaders();

    delete myCenterBuffer;
    delete myVertexBuffer;
    delete myIndexBuffer;
    delete myLayout;
    delete myContext;

    SDL_GL_DeleteContext(contexteOpenGL);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
}

void GameApplication::LoadShaders()
{
    ps = kT::OGL3Shader::LoadFromFile( "shaders/glsl/ps.glsl", kT::OGL3Shader::PixelShader, kT::OGL3Shader::Profile4 );

    if( ps->CompilationPassed() == false )
    {
        std::cerr<<ps->GetErrorLog()<<std::endl;
        throw ;
    }
    
    vs = kT::OGL3Shader::LoadFromFile( "shaders/glsl/vs.glsl", kT::OGL3Shader::VertexShader, kT::OGL3Shader::Profile4  );

    if( vs->CompilationPassed() == false )
    {
        std::cerr<<vs->GetErrorLog()<<std::endl;
        throw ;
    }

    prog = new kT::OGL3Program( vs, ps );

    if( prog->LinkPassed() == false )
    {
        std::cerr<<prog->GetErrorLog()<<std::endl;
        throw ;
    }
    
    kT::InputElementDesc elementsDesc[] = {
        { 0, "in_Vertex", 0, kT::PixelFormat::RGBA32_FLOAT, 0, kT::InputElementClass::PerVertex, 0},
        { 0, "in_Normal", 0, kT::PixelFormat::RGBA32_FLOAT, sizeof(glm::vec4), kT::InputElementClass::PerVertex, 0},
        { 1, "in_Center", 0, kT::PixelFormat::RGBA32_FLOAT, 0, kT::InputElementClass::PerInstance, 1},
        //{ 1, "in_Color",  0, kT::PixelFormat::RGBA32_FLOAT, sizeof(glm::vec4), kT::InputElementClass::PerInstance, 1}
    };

    kT::InputLayoutDesc layoutDesc = {
        3,
        elementsDesc
    };

    myLayout = new kT::OGL3InputLayout( prog, layoutDesc );
	myLayout->ComputeStrides( myStrides );
    memset( myOffsets, 0, sizeof(size_t)*16 );

    myModelviewUniformIndex = prog->GetUniformIndexByName( "modelview" );
    myProjUniformIndex = prog->GetUniformIndexByName( "projection" );
}

void GameApplication::DeleteShaders()
{
    delete prog;
    delete ps;
    delete vs;
}

void GameApplication::Run()
{
    kT::Clock clk;
    clk.Reset();

    bool terminer = false;
    while(!terminer)
    {
        SDL_Event evenements;
        while( SDL_PollEvent(&evenements) )
        {
            if(evenements.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                terminer = true;
                break;
            }
        }

        float dt = clk.GetTimeElapsedReset().AsSeconds();
        Update(dt);
        Draw(dt);
        clk.Reset();
    }
}

void GameApplication::Update( float dt )
{
}

void GameApplication::Draw( float dt )
{
	//myContext->OMSetRenderTargets(1, textures, depthBuffer);
    glEnable( GL_DEPTH_TEST );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    myContext->SetProgram( prog );
    myContext->IASetInputLayout( myLayout );
    myContext->IASetIndexBuffer( myIndexBuffer, kT::PixelFormat::R32_UINT, 0 );

    kT::OGL3HardwareBuffer* buffers[] = {
        myVertexBuffer,
        myCenterBuffer
    };

    myContext->IASetVertexBuffers( 0, 2, buffers, myStrides, myOffsets );

    glUniformMatrix4fv( myModelviewUniformIndex , 1, GL_FALSE, glm::value_ptr( modelview ) );
    glUniformMatrix4fv( myProjUniformIndex, 1, GL_FALSE, glm::value_ptr( projection ) );
    
    GLsizei numInstances = numParticles;
    GLuint base = 0;
    myContext->DrawIndexedInstanced( 36, numInstances, 0, base, 0 );

    SDL_GL_SwapWindow(fenetre);
}