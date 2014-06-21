#include <D3DGameApplication.hpp>

#include <kT/Core/Clock.hpp>
#include <kT/Math/Matrix4.hpp>

#include <kT/Window/Window.hpp>
#include <kT/Graphics/D3D11Device/D3D11Device.hpp>
#include <kT/Graphics/D3D11Device/D3D11ImmediateContext.hpp>
#include <kT/Graphics/D3D11Device/D3D11InputLayout.hpp>
#include <kT/Graphics/D3D11Device/D3D11HardwareBuffer.hpp>
#include <kT/Graphics/D3D11Device/D3D11SwapChain.hpp>
#include <kT/Graphics/D3D11Device/D3D11DepthStencilBuffer.hpp>
#include <kT/Graphics/D3D11Device/D3D11Shader.hpp>
#include <kT/Graphics/D3D11Device/D3D11RasterizerState.hpp>
#include <kT/Graphics/GraphicsDevice/ViewportDesc.hpp>


#define N 20
#define M 20

struct Matrices
{
	kT::Matrix4f32 Proj;
	kT::Matrix4f32 ModelView;
};

struct vert{
	kT::Vector4f32 pos;
	//kT::Vector4f32 col;
};

static float frand(int *seed)
{
	union
	{
		float fres;
		unsigned int ires;
	};

	seed[0] *= 16807;

	ires = ((((unsigned int)seed[0]) >> 9) | 0x3f800000);
	return fres - 1.0f;
}

D3DGameApplication::D3DGameApplication() :
fenetreD3D(0),
myD3DDevice(0),
myD3DContext(0),
myD3DSwapChain(0),
myD3DDepthBuffer(0),
myD3DLayout(0),
myD3DVertexBuffer(0),
myD3DCenterBuffer(0),
myD3DIndexBuffer(0),
d3dvs(0),
d3dps(0),
myD3DConstantBuffer(0),
myRSState(0)
{
	fenetreD3D = new kT::Window("A D3D", kT::Sizeui32(800, 600), kT::Window::Overlapped);
	fenetreD3D->Show();

	myD3DDevice = new kT::D3D11Device(kT::GraphicsDevice< kTD3D11DeviceTemplateListLineTypes >::HardwareProcessing, true);
	myD3DContext = myD3DDevice->GetImmediateContext();

	myD3DSwapChain = new kT::D3D11SwapChain(myD3DDevice->GetHandle(), fenetreD3D, kT::PixelFormat::RGBA8_UNORM, true, 1);
	myD3DDepthBuffer = new kT::D3D11DepthStencilBuffer(myD3DDevice->GetHandle(), kT::PixelFormat::DEPTH, myD3DSwapChain->GetTexture()->GetSize());

	kT::Vector4f32 vertices[72] = {

		// Front face [ 0, 3 ]
		kT::Vector4f32(-1.f, -1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, 1.0f, 0.f),// kT::Vector4f32( 1.0f, 1.0f, 1.0f, 1.0f ),
		kT::Vector4f32(1.f, -1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, 1.0f, 0.f),// kT::Vector4f32( 1.0f, 1.0f, 1.0f, 1.0f ),
		kT::Vector4f32(1.f, 1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, 1.0f, 0.f),// kT::Vector4f32( 1.0f, 1.0f, 1.0f, 1.0f ),
		kT::Vector4f32(-1.f, -1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, 1.0f, 0.f),// kT::Vector4f32( 1.0f, 1.0f, 1.0f, 1.0f ),
		kT::Vector4f32(1.f, 1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, 1.0f, 0.f),// kT::Vector4f32( 1.0f, 1.0f, 1.0f, 1.0f ),
		kT::Vector4f32(-1.f, 1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, 1.0f, 0.f),// kT::Vector4f32( 1.0f, 1.0f, 1.0f, 1.0f ),


		// Right face [ 4, 7 ]
		kT::Vector4f32(1.f, -1.f, -1.f, 1.f), kT::Vector4f32(1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( 1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, 1.f, 1.f, 1.f), kT::Vector4f32(1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( 1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, -1.f, 1.f, 1.f), kT::Vector4f32(1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( 1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, -1.f, -1.f, 1.f), kT::Vector4f32(1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( 1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, 1.f, -1.f, 1.f), kT::Vector4f32(1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( 1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, 1.f, 1.f, 1.f), kT::Vector4f32(1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( 1.0f, 0.0f, 0.0f, 1.0f ),


		// Back face [ 8, 11 ]
		kT::Vector4f32(-1.f, 1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, -1.0f, 0.f),// kT::Vector4f32( 0.0f, 0.0f, -1.0f , 0.f ),
		kT::Vector4f32(1.f, 1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, -1.0f, 0.f),// kT::Vector4f32( 0.0f, 0.0f, -1.0f , 0.f ),
		kT::Vector4f32(1.f, -1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, -1.0f, 0.f),// kT::Vector4f32( 0.0f, 0.0f, -1.0f , 0.f ),
		kT::Vector4f32(-1.f, 1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, -1.0f, 0.f),// kT::Vector4f32( 0.0f, 0.0f, -1.0f , 0.f ),
		kT::Vector4f32(1.f, -1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, -1.0f, 0.f),// kT::Vector4f32( 0.0f, 0.0f, -1.0f , 0.f ),
		kT::Vector4f32(-1.f, -1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 0.0f, -1.0f, 0.f),// kT::Vector4f32( 1.0f, 1.0f, 0.0f, 1.0f ),


		// Left face [ 12, 15 ]
		kT::Vector4f32(-1.f, 1.f, 1.f, 1.f), kT::Vector4f32(-1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( -1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, 1.f, -1.f, 1.f), kT::Vector4f32(-1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( -1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, -1.f, -1.f, 1.f), kT::Vector4f32(-1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( -1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, 1.f, 1.f, 1.f), kT::Vector4f32(-1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( -1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, -1.f, -1.f, 1.f), kT::Vector4f32(-1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( -1.0f, 0.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, -1.f, 1.f, 1.f), kT::Vector4f32(-1.0f, 0.0f, 0.0f, 0.f),// kT::Vector4f32( -1.0f, 0.0f, 0.0f , 0.f ),

		// Top face [ 16, 19 ]
		kT::Vector4f32(-1.f, 1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, 1.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, 1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, 1.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, 1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, 1.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, 1.f, -1.f, 1.f), kT::Vector4f32(0.0f, 1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, 1.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, 1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, 1.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, 1.f, 1.f, 1.f), kT::Vector4f32(0.0f, 1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, 0.0f, 1.0f, 1.0f ),

		// Bottom face [ 20, 23 ]
		kT::Vector4f32(-1.f, -1.f, -1.f, 1.f), kT::Vector4f32(0.0f, -1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, -1.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, -1.f, 1.f, 1.f), kT::Vector4f32(0.0f, -1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, -1.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, -1.f, 1.f, 1.f), kT::Vector4f32(0.0f, -1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, -1.0f, 0.0f , 0.f ),
		kT::Vector4f32(-1.f, -1.f, -1.f, 1.f), kT::Vector4f32(0.0f, -1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, -1.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, -1.f, -1.f, 1.f), kT::Vector4f32(0.0f, -1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, -1.0f, 0.0f , 0.f ),
		kT::Vector4f32(1.f, -1.f, 1.f, 1.f), kT::Vector4f32(0.0f, -1.0f, 0.0f, 0.f),// kT::Vector4f32( 0.0f, -1.0f, 0.0f , 0.f ),
	};

	myD3DVertexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice->GetHandle(),
		kT::D3D11HardwareBuffer::VertexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(kT::Vector4f32),
		72,
		vertices);

	unsigned indices[36];
	for (size_t i = 0; i < 36; i++)
		indices[i] = i;

	myD3DIndexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice->GetHandle(),
		kT::D3D11HardwareBuffer::IndexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(unsigned),
		36,
		indices);

	numParticles = 10000;

	vert *inParticles = new vert[numParticles];

	int seed = 1214;
	for (size_t i = 0; i < numParticles; i++)
	{
		float decision = frand(&seed);
		/*
		if (decision < 0.1f)
			inParticles[i].col = kT::Vector4f32(1.f, 0.0f, 1.f, 1.0f);
		else if (decision < 0.2f)
			inParticles[i].col = kT::Vector4f32(0.0f, 1.0f, 1.0f, 1.0f);
		else if (decision < 0.3f)
			inParticles[i].col = kT::Vector4f32(0.0f, 1.0f, 0.0f, 1.0f);
		else if (decision < 0.4f)
			inParticles[i].col = kT::Vector4f32(1.0f, 1.0f, 0.0f, 1.0f);
		else
			inParticles[i].col = kT::Vector4f32(1.0f, 1.0f, 1.0f, 1.f);
		*/

		inParticles[i].pos = kT::Vector4f32(20.0f*(frand(&seed) - 0.5f), 20.0f*(frand(&seed) - 0.5f), 20.0f*(frand(&seed) - 0.5f), 1.0f);
	}

	myD3DCenterBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice->GetHandle(),
		kT::D3D11HardwareBuffer::VertexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(vert),
		numParticles,
		inParticles);

	delete[] inParticles;

	LoadShaders();

	float zNear = 1.0f;
	float zFar = 100.0f;

	projection.BuildPerspectiveProjectionFOVRH(70.0f * kT::DegToRad32, (float)800.0f / 600.0f, zNear, zFar);
	modelview.BuildLookAtRH(kT::Vector3f32(0.f, 0.f, -10.f), kT::Vector3f32(0.0f, 0.0f, 0.0f), kT::Vector3f32(0.f, 1.f, 0.f));

	kT::RasterizerStateDesc rsdesc;
	rsdesc.frontCounterClockwise = true;
	rsdesc.cullMode = kT::RasterizerStateDesc::CullBack;
	myRSState = new kT::D3D11RasterizerState(myD3DDevice->GetHandle(), rsdesc);
	myD3DContext->RSSetState(myRSState);
}

D3DGameApplication::~D3DGameApplication()
{
	DeleteShaders();
}

void D3DGameApplication::LoadShaders()
{

	d3dps = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/ps.hlsl", kT::D3D11Shader::PixelShader, kT::D3D11Shader::Profile4, "main");
	d3dvs = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/vs.hlsl", kT::D3D11Shader::VertexShader, kT::D3D11Shader::Profile4, "main");

	kT::InputElementDesc delementsDesc[] = {
		{ 0, "POSITION", 0, kT::PixelFormat::RGBA32_FLOAT, 0, kT::InputElementClass::PerVertex, 0 },
		{ 0, "NORMAL", 0, kT::PixelFormat::RGBA32_FLOAT, sizeof(kT::Vector4f32), kT::InputElementClass::PerVertex, 0 },
		{ 1, "TEXCOORD", 0, kT::PixelFormat::RGBA32_FLOAT, 0, kT::InputElementClass::PerInstance, 1 },
		//{ 1, "COLOR", 0, kT::PixelFormat::RGBA32_FLOAT, sizeof(glm::vec4), kT::InputElementClass::PerInstance, 1 }
	};

	kT::InputLayoutDesc dlayoutDesc = {
		3,
		delementsDesc
	};

	myD3DLayout = new kT::D3D11InputLayout(myD3DDevice, d3dvs, dlayoutDesc);
	myD3DLayout->ComputeStrides(myStrides);
	memset(myOffsets, 0, sizeof(size_t)* 16);
	myD3DConstantBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice->GetHandle(),
		kT::D3D11HardwareBuffer::ConstantBuffer,
		kT::D3D11HardwareBuffer::DefaultBufferUsage,
		0,
		sizeof(Matrices),
		1,
		0);

}

void D3DGameApplication::DeleteShaders()
{
}

void D3DGameApplication::Run()
{
	kT::Clock clk;
	clk.Reset();

	bool terminer = false;
	while (!terminer)
	{
		kT::GUIEvent evenements;
		fenetreD3D->ProcessEvents();
		while (fenetreD3D->GetEvent(evenements))
		{
			if (evenements.Type == kT::GUIEvent::KeyPressed
				&& evenements.Key.Code == kT::Keyboard::Escape )
			{
				terminer = true;
				break;
			}

			if (evenements.Type == kT::GUIEvent::Resized)
			{
				delete myD3DDepthBuffer;
				myD3DDepthBuffer = new kT::D3D11DepthStencilBuffer(myD3DDevice->GetHandle(), kT::PixelFormat::DEPTH, myD3DSwapChain->GetTexture()->GetSize());
			}
		}

		float dt = clk.GetTimeElapsedReset().AsSeconds();
		Update(dt);
		Draw(dt);
		clk.Reset();
	}
}

void D3DGameApplication::Update(float dt)
{
}

void D3DGameApplication::Draw(float dt)
{
	Matrices mat;
	mat.Proj = projection;
	mat.ModelView = modelview;
	myD3DConstantBuffer->WriteData(myD3DContext, 0, sizeof(Matrices), &mat);

	kT::D3D11Texture* targets[] = {
		myD3DSwapChain->GetTexture()
	};

	// Set the viewport
	kT::Sizeui32 ssize = myD3DSwapChain->GetTexture()->GetSize();

	kT::ViewportDesc vp(0.0f, 0.0f, static_cast<float>(ssize.Width), static_cast<float>(ssize.Height));
	myD3DContext->RSSetViewports(1, &vp);
	myD3DContext->IASetPrimitiveTopology(kT::PrimitiveTopology::TriangleList);

	myD3DContext->OMSetRenderTargets(1, targets, myD3DDepthBuffer);
	myD3DContext->ClearRenderTarget(myD3DSwapChain->GetTexture(), kT::Vector4f32(0.0f, 0.0f, 0.0f, 0.0f));
	myD3DContext->ClearDepthStencilBuffer(myD3DDepthBuffer, kT::ClearDepth);

	myD3DContext->VSSetShader(d3dvs);
	myD3DContext->PSSetShader(d3dps);
	myD3DContext->IASetInputLayout(myD3DLayout);
	myD3DContext->IASetIndexBuffer(myD3DIndexBuffer, kT::PixelFormat::R32_UINT, 0);

	kT::D3D11HardwareBuffer* dbuffers[] = {
		myD3DVertexBuffer,
		myD3DCenterBuffer
	};

	myD3DContext->IASetVertexBuffers(0, 2, dbuffers, myStrides, myOffsets);

	myD3DContext->VSSetConstantBuffers(0, 1, &myD3DConstantBuffer);

	myD3DContext->DrawIndexedInstanced(36, numParticles, 0, 0, 0);
	myD3DSwapChain->Present();

	kT::D3D11HardwareBuffer* n = 0;
	myD3DContext->VSSetConstantBuffers(0, 1, &n );
}