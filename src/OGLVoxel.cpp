#include <OGLVoxel.hpp>

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

struct Matrices
{
	glm::mat4 Proj;
	glm::mat4 ModelView;
};

struct vert{
    glm::vec4 pos;
    glm::vec4 col;
};

OGLVoxel::OGLVoxel():
    fenetre(0),
    contexteOpenGL(0),
 myContext( 0 ),
 myVertexBuffer(0),
 myCenterBuffer(0),
 myIndexBuffer(0),
 myLayout( 0 ),
 vs(0),
 ps(0),
 prog(0),
 myModelviewUniformIndex(0),
 myProjUniformIndex(0),
 numCubesX(0),
 numCubesY(0),
 numCubesZ(0)
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

    SDL_GL_MakeCurrent( fenetre, contexteOpenGL );

    initializeGL();
    resizeGL( 800, 600 );
}

OGLVoxel::~OGLVoxel()
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

void OGLVoxel::initializeGL()
{
    if( glewInit() != GLEW_OK)
        throw ;

    myContext = new kT::OGL3ImmediateContext(0);

    glDebugMessageCallbackAMD( debugCallBack, reinterpret_cast<GLvoid*>(this) );

    // a cube
    glm::vec4 vertices[36] = {
            
            // Front face [ 0, 3 ]
         glm::vec4(-1.f, -1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(-1.f, -1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
             glm::vec4(-1.f, 1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, 1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ),
            
            
            // Right face [ 4, 7 ]
             glm::vec4(1.f, -1.f, -1.f, 1.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, -1.f, 1.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ),// glm::vec4( 1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f ),
            

            // Back face [ 8, 11 ]
             glm::vec4(-1.f, 1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(1.f, 1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(-1.f, 1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 0.0f, -1.0f , 0.f ),// glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f ),
            

            // Left face [ 12, 15 ]
             glm::vec4(-1.f, 1.f, 1.f, 1.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, -1.f, 1.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, 1.f, 1.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, 1.f, 1.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),// glm::vec4( -1.0f, 0.0f, 0.0f , 0.f ),
            
            // Top face [ 16, 19 ]
             glm::vec4(-1.f, 1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, -1.f, 1.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, 1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, 1.f, 1.f, 1.f ),// glm::vec4( 0.0f, 1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ),
            
            // Bottom face [ 20, 23 ]
             glm::vec4(-1.f, -1.f, -1.f, 1.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, 1.f, 1.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(-1.f, -1.f, -1.f, 1.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, -1.f, 1.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
             glm::vec4(1.f, -1.f, 1.f, 1.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),// glm::vec4( 0.0f, -1.0f, 0.0f , 0.f ),
        };

    myVertexBuffer = new kT::OGL3HardwareBuffer(
        0,
        kT::OGL3HardwareBuffer::VertexBuffer,
        kT::OGL3HardwareBuffer::ImmutableBufferUsage,
        0,
        sizeof(glm::vec4),
        36,
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
        sizeof(indices)/sizeof(unsigned),
        indices );

    LoadShaders();

    projection = glm::perspective(70.0, (double) 800/600, 1.0, 100.0);
    modelview = glm::lookAt( glm::vec3(4.f,0.f, 4.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.f, 1.f, 0.f) );

    numCubesX = 10;
    numCubesY = 10;
    numCubesZ = 10;
    SetupCubes(10,10,10);
}

void OGLVoxel::resizeGL( int width, int height )
{
    double fwidth = static_cast< double >( width );
    double fheight = static_cast< double >( height );
    projection = glm::perspective(70.0, fwidth/fheight, 1.0, 100.0);
}

void OGLVoxel::paintGL()
{
    glEnable( GL_DEPTH_TEST );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    GLsizei numInstances = numCubesX * numCubesY * numCubesZ;

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
    GLuint base = 0;
    myContext->DrawIndexedInstanced( 36, numInstances, 0, base, 0 );

    SDL_GL_SwapWindow(fenetre);
}

void OGLVoxel::LoadShaders()
{
    const char psSrc[] =
        "in vec4 color;\n"
        "\n"
        "out vec4 out_Color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   out_Color.rgb = color.rgb;\n"
        "	out_Color.a = 1.0f;\n"
        "}";

    ps = new kT::OGL3Shader(kT::OGL3Shader::PixelShader, kT::OGL3Shader::Profile4, psSrc, sizeof(psSrc)/sizeof(char) );

    if( ps->CompilationPassed() == false )
    {
        std::cerr<<ps->GetErrorLog()<<std::endl;
        throw ;
    }
    
    const char vsSrc[] =
        "uniform mat4 modelview;\n"
        "uniform mat4 projection;\n"
        "\n"
        "in vec4 in_Vertex;\n"
        "in vec4 in_Center;\n"
        "in vec4 in_Color;\n"
        "\n"
        "out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 worldPos = modelview * (in_Vertex + in_Center);\n"
        "    gl_Position = projection * worldPos;\n"
        "    color = in_Color;\n"
        "}";
    
    vs = new kT::OGL3Shader(kT::OGL3Shader::VertexShader, kT::OGL3Shader::Profile4, vsSrc, sizeof(vsSrc)/sizeof(char) );

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
        { 1, "in_Center", 0, kT::PixelFormat::RGBA32_FLOAT, 0, kT::InputElementClass::PerInstance, 1},
        { 1, "in_Color",  0, kT::PixelFormat::RGBA32_FLOAT, sizeof(glm::vec4), kT::InputElementClass::PerInstance, 1}
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

void OGLVoxel::DeleteShaders()
{
    delete prog;
    delete ps;
    delete vs;
}

void OGLVoxel::updateStruct()
{
    if( true )
    {
        std::cout<<"Updated"<<std::endl;
        size_t structSizeX = 100;
        size_t structSizeY = structSizeX;// currently the same size in X, Y, and Z, but it is going to change...
        size_t structSizeZ = structSizeX;

        if( (numCubesX != structSizeX) || (numCubesY != structSizeY) || (numCubesZ != structSizeZ) ){
            SetupCubes(structSizeX, structSizeY, structSizeZ);
        }
    }
}

void OGLVoxel::SetupCubes(size_t numCubesX_, size_t numCubesY_, size_t numCubesZ_ )
{
    std::cout<<"Setup"<<std::endl;

    if( myCenterBuffer != 0 )
        delete myCenterBuffer;

    size_t sizes[] = { numCubesX_, numCubesY_, numCubesZ_ };
    size_t numCubes = numCubesX_ * numCubesY_ * numCubesZ_;

    vert* centers = new vert[ numCubes ];

    for( size_t x = 0; x < numCubesX_; x++ ){
        for( size_t y = 0; y < numCubesY_; y++ ){
            for( size_t z = 0; z < numCubesZ_; z++ ){
                glm::vec4 center = glm::vec4(
                    (static_cast<float>(x) / static_cast<float>(numCubesX-1)) - 0.5f,
                    (static_cast<float>(y) / static_cast<float>(numCubesY-1)) - 0.5f,
                    (static_cast<float>(z) / static_cast<float>(numCubesZ-1)) - 0.5f,
                    0.0f
                );
                
                glm::vec4 color = glm::vec4(
                    0.5f + 0.5f * (static_cast<float>(x) / static_cast<float>(numCubesX-1)),
                    0.5f + 0.5f * (static_cast<float>(y) / static_cast<float>(numCubesY-1)),
                    0.5f + 0.5f * (static_cast<float>(z) / static_cast<float>(numCubesZ-1)),
                    1.0f
                );
                
                centers[ (x * numCubesX_ + y) * numCubesY_ + z ].pos = center;
                centers[ (x * numCubesX_ + y) * numCubesY_ + z ].col = color;
            }
        }
    }

    myCenterBuffer = new kT::OGL3HardwareBuffer(
        0,
        kT::OGL3HardwareBuffer::VertexBuffer,
        kT::OGL3HardwareBuffer::DefaultBufferUsage,// we update it every frame
        0,
        sizeof(vert),
        numCubes,
        centers );

    delete[] centers;

    numCubesX = numCubesX_;
    numCubesY = numCubesY_;
    numCubesZ = numCubesZ_;
}

void OGLVoxel::Run()
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

void OGLVoxel::Update( float dt )
{
}

void OGLVoxel::Draw( float dt )
{
    paintGL();
}