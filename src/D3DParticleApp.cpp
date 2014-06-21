#include <D3DParticleApp.hpp>

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
#include <kT/Graphics/D3D11Device/D3D11BlendState.hpp>
#include <kT/Graphics/D3D11Device/D3D11RasterizerState.hpp>
#include <kT/Graphics/GraphicsDevice/ViewportDesc.hpp>

#include <vector>
#include <algorithm>

struct Matrices
{
	kT::Matrix4f32 Proj;
	kT::Matrix4f32 ModelView;
};

struct vert{
	kT::Vector2f32 pos;
	kT::Vector2f32 uv;
};

D3DParticleApp::D3DParticleApp() :
fenetreD3D(0),
myD3DDevice(0),
myD3DContext(0),
myD3DSwapChain(0),
myD3DDepthBuffer(0),
myD3DLayout(0),
myD3DVertexBuffer(0),
myD3DInstanceVertexBuffer(0),
myD3DIndexBuffer(0),
d3dvs(0),
d3dps(0),
myD3DConstantBuffer(0),
myBlendState(0),
myRSState(0)
{
	fenetreD3D = new kT::Window("A D3D Particles", kT::Sizeui32(800, 600), kT::Window::Overlapped);
	fenetreD3D->Show();

	myD3DDevice = new kT::D3D11Device(kT::GraphicsDevice< kTD3D11DeviceTemplateListLineTypes >::HardwareProcessing, true);
	myD3DContext = myD3DDevice->GetImmediateContext();

	myD3DSwapChain = new kT::D3D11SwapChain(myD3DDevice, fenetreD3D, kT::PixelFormat::RGBA8_UNORM, true, 1);
	myD3DDepthBuffer = new kT::D3D11DepthStencilBuffer(myD3DDevice, kT::PixelFormat::DEPTH, myD3DSwapChain->GetTexture()->GetSize());

	
	numParticles = 10000;

	particles.resize(numParticles);

	// Generate the particles
	float dt = float(numParticles - 1);
	for (size_t i = 0; i < numParticles; i++)
	{
		particle& p = particles[i];

		p.pos = kT::Vector3f32(0.f, 0.f, 0.f);
		p.speed = kT::Vector3f32(0.f, 5.0f, 0.f);
		p.color = kT::Vector4f32(1.0f, 1.0f, 0.2f, 1.0f);
		p.lifeTime = 5.0f * float(i) / dt;
	}

	myD3DVertexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::VertexBuffer,
		kT::D3D11HardwareBuffer::DefaultBufferUsage,
		0,
		sizeof(particle),
		particles.size(),
		&particles[0]);

	// instance vertices
	std::vector<vert> instanceVertices;
	instanceVertices.resize(4);

	instanceVertices[0].pos = kT::Vector2f32(-1.f, 1.f);
	instanceVertices[0].uv = kT::Vector2f32(0.f, 0.f);

	instanceVertices[1].pos = kT::Vector2f32(-1.f, -1.f);
	instanceVertices[1].uv = kT::Vector2f32(0.f, 1.f);

	instanceVertices[2].pos = kT::Vector2f32(1.f, -1.f);
	instanceVertices[2].uv = kT::Vector2f32(1.f, 1.f);

	instanceVertices[3].pos = kT::Vector2f32(1.f, 1.f);
	instanceVertices[3].uv = kT::Vector2f32(1.f, 0.f);

	myD3DInstanceVertexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::VertexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(vert),
		instanceVertices.size(),
		&instanceVertices[0]);

	// Generate the indices

	std::vector<unsigned> indices;
	indices.resize(2*3);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	myD3DIndexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::IndexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(unsigned),
		indices.size(),
		&indices[0]);

	// Generate the color map
	GenerateColorMap();

	LoadShaders();

	float zNear = 1.0f;
	float zFar = 100.0f;

	projection.BuildPerspectiveProjectionFOVRH(70.0f * kT::DegToRad32, (float)800.0f / 600.0f, zNear, zFar);
	modelview.BuildLookAtRH(kT::Vector3f32(0.f, 0.f, 10.f), kT::Vector3f32(0.0f, 0.0f, 0.0f), kT::Vector3f32(0.f, 1.f, 0.f));

	kT::RasterizerStateDesc rsdesc;
	rsdesc.frontCounterClockwise = true;
	rsdesc.cullMode = kT::RasterizerStateDesc::CullBack;
	myRSState = new kT::D3D11RasterizerState(myD3DDevice->GetHandle(), rsdesc);

	myBlendState = kT::D3D11BlendState::CreateAlphaBlendingState(myD3DDevice);
}

D3DParticleApp::~D3DParticleApp()
{
	DeleteShaders();
	delete myColorMap;
	delete myRSState;
	delete myBlendState;
	delete myD3DConstantBuffer;
	delete myD3DIndexBuffer;
	delete myD3DVertexBuffer;
	delete myD3DInstanceVertexBuffer;
	delete myD3DLayout;
	delete myD3DDepthBuffer;
	delete myD3DSwapChain;
	delete myD3DDevice;
	delete fenetreD3D;
}

void D3DParticleApp::GenerateColorMap()
{
	kT::Uint32 width = 64, height = 64;

	float du = 1.0f / float(width - 1);
	float dv = 1.0f / float(height - 1);

	std::vector<kT::Vector4ui8> texels;
	texels.resize(width*height);

	for (size_t i = 0; i < height; i++)
		for (size_t j = 0; j < width; j++)
		{
			float u = float(i) * du;
			float v = float(j) * dv;

			kT::Vector2f32 uv(u, v);

			float d = (uv - kT::Vector2f32(0.5f, 0.5f)).Length();
			float c = d > 0.5f ? 0.0f : 1.0f - d;

			kT::Vector4f32 col(c,c,c,c);
			col *= 255.f;

			texels[i * width + j] = kT::Vector4ui8(kT::Uint8(col.x), kT::Uint8(col.y), kT::Uint8(col.z), kT::Uint8(col.w));
		}

	myColorMap = new kT::D3D11Texture(
		myD3DDevice,
		kT::Texture::ShaderInput,
		kT::PixelFormat::RGBA8_UNORM,
		kT::Sizeui32(width, height),
		&texels[0]);
}

void D3DParticleApp::LoadShaders()
{

	DeleteShaders();

	d3dps = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/particles/ps.hlsl", kT::D3D11Shader::PixelShader, kT::D3D11Shader::Profile4, "main");
	d3dvs = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/particles/vs.hlsl", kT::D3D11Shader::VertexShader, kT::D3D11Shader::Profile4, "main");

	kT::InputElementDesc delementsDesc[] = {
		{ 0, "POSITION", 0, kT::PixelFormat::RG32_FLOAT, 0, kT::InputElementClass::PerVertex, 0 },
		{ 0, "TEXCOORD", 0, kT::PixelFormat::RG32_FLOAT, sizeof(kT::Vector2f32), kT::InputElementClass::PerVertex, 0 },
		{ 1, "TEXCOORD", 1, kT::PixelFormat::RGB32_FLOAT, 0, kT::PerInstance, 1 },//Particle pos
		{ 1, "TEXCOORD", 2, kT::PixelFormat::RGB32_FLOAT, sizeof(kT::Vector3f32), kT::PerInstance, 1 },//Particle speed
		{ 1, "TEXCOORD", 3, kT::PixelFormat::RGBA32_FLOAT, 2*sizeof(kT::Vector3f32), kT::PerInstance, 1 },//Particle color
		{ 1, "TEXCOORD", 4, kT::PixelFormat::R32_FLOAT, 2 * sizeof(kT::Vector3f32) + sizeof(kT::Vector4f32), kT::PerInstance, 1 }
	};

	kT::InputLayoutDesc dlayoutDesc = {
		6,
		delementsDesc
	};

	myD3DLayout = new kT::D3D11InputLayout(myD3DDevice, d3dvs, dlayoutDesc);
	myD3DLayout->ComputeStrides(myStrides);
	memset(myOffsets, 0, sizeof(size_t)* 16);
	myD3DConstantBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::ConstantBuffer,
		kT::D3D11HardwareBuffer::DefaultBufferUsage,
		0,
		sizeof(Matrices),
		1,
		0);

}

void D3DParticleApp::DeleteShaders()
{
	if (d3dps)
		delete d3dps;

	if (d3dvs)
		delete d3dvs;
}

void D3DParticleApp::Run()
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
				&& evenements.Key.Code == kT::Keyboard::Escape)
			{
				terminer = true;
				break;
			}

			if (evenements.Type == kT::GUIEvent::Resized)
			{
				delete myD3DDepthBuffer;
				myD3DDepthBuffer = new kT::D3D11DepthStencilBuffer(myD3DDevice, kT::PixelFormat::DEPTH, myD3DSwapChain->GetTexture()->GetSize());
			}
		}

		float dt = clk.GetTimeElapsedReset().AsSeconds();
		Update(dt);
		Draw(dt);
		clk.Reset();
	}
}

void D3DParticleApp::UpdateParticles(float dt)
{
	for (size_t i = 0; i < numParticles; i++)
	{
		particle& p = particles[i];
		p.pos += dt * p.speed;
		//p.speed *= (1.0f - dt);
		p.lifeTime += dt;

		if (p.lifeTime >= 5.0f)
		{
			p.pos = kT::Vector3f32(0.f, 0.f, 0.f);
			p.speed = kT::Vector3f32(0.f, 5.0f, 0.f);
			p.color = kT::Vector4f32(1.0f, 1.0f, 0.2f, 1.0f);
			p.lifeTime = 0.f;
		}
	}
}

void D3DParticleApp::SortParticles()
{
	//std::sort(particles.begin(), particles.end() );
}

void D3DParticleApp::Update(float dt)
{
	UpdateParticles(dt);
	SortParticles();

	myD3DVertexBuffer->WriteData(myD3DContext, 0, sizeof(particle)* numParticles, &particles[0]);

}

void D3DParticleApp::Draw(float dt)
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
	myD3DContext->RSSetState(myRSState);

	myD3DContext->IASetPrimitiveTopology(kT::PrimitiveTopology::TriangleList);

	myD3DContext->OMSetBlendState(myBlendState);
	myD3DContext->OMSetRenderTargets(1, targets, 0);// myD3DDepthBuffer);
	myD3DContext->ClearRenderTarget(myD3DSwapChain->GetTexture(), kT::Vector4f32(0.0f, 0.0f, 0.0f, 0.0f));
	myD3DContext->ClearDepthStencilBuffer(myD3DDepthBuffer, kT::ClearDepth);

	myD3DContext->VSSetShader(d3dvs);
	myD3DContext->PSSetShader(d3dps);
	myD3DContext->IASetInputLayout(myD3DLayout);
	myD3DContext->IASetIndexBuffer(myD3DIndexBuffer, kT::PixelFormat::R32_UINT, 0);

	kT::D3D11HardwareBuffer* dbuffers[] = {
		myD3DInstanceVertexBuffer,
		myD3DVertexBuffer
	};

	myD3DContext->IASetVertexBuffers(0, 2, dbuffers, myStrides, myOffsets);

	myD3DContext->VSSetConstantBuffers(0, 1, &myD3DConstantBuffer);

	kT::D3D11ShaderResource* srvs[] = {
		myColorMap
	};

	myD3DContext->PSSetShaderResources(0, 1, srvs);

	myD3DContext->DrawIndexedInstanced(2 * 3, numParticles, 0, 0, 0);
	myD3DSwapChain->Present();

	kT::D3D11HardwareBuffer* n = 0;
	myD3DContext->VSSetConstantBuffers(0, 1, &n);
}