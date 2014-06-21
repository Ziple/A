#include <D3DProceduralApp.hpp>

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
#include <kT/Core/Exceptions.hpp>

#include <iostream>
#include <vector>


D3DProceduralApp::D3DProceduralApp() :
fenetreD3D(0),
myD3DDevice(0),
myD3DContext(0),
myD3DSwapChain(0),
myD3DLayout(0),
myD3DVertexBuffer(0),
myD3DIndexBuffer(0),
d3dvs(0),
d3dps(0),
myRSState(0)
{
	fenetreD3D = new kT::Window("A D3D Procedural", kT::Sizeui32(800, 600), kT::Window::Overlapped);
	fenetreD3D->Show();

	myD3DDevice = new kT::D3D11Device(kT::GraphicsDevice< kTD3D11DeviceTemplateListLineTypes >::HardwareProcessing, true);
	myD3DContext = myD3DDevice->GetImmediateContext();

	myD3DSwapChain = new kT::D3D11SwapChain(myD3DDevice, fenetreD3D, kT::PixelFormat::RGBA8_UNORM, true, 1);

	std::vector<kT::Vector3f32> vertices;
	vertices.resize(4);
	vertices[0] = kT::Vector3f32(-1.f, 1.f, 0.0f);
	vertices[1] = kT::Vector3f32(-1.f, -1.f, 0.0f);
	vertices[2] = kT::Vector3f32(1.f, -1.f, 0.0f);
	vertices[3] = kT::Vector3f32(1.f, 1.f, 0.0f);

	myD3DVertexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::VertexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(kT::Vector3f32),
		4,
		&vertices[0]);

	std::vector<kT::Uint16> indices;
	indices.resize(6);
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
		sizeof(kT::Uint16),
		6,
		&indices[0]);

	LoadShaders();

	kT::RasterizerStateDesc rsdesc;
	rsdesc.frontCounterClockwise = true;
	rsdesc.cullMode = kT::RasterizerStateDesc::CullBack;
	myRSState = new kT::D3D11RasterizerState(myD3DDevice->GetHandle(), rsdesc);
	myD3DContext->RSSetState(myRSState);

	psconstants = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::ConstantBuffer,
		kT::D3D11HardwareBuffer::DefaultBufferUsage,
		0,
		sizeof(kT::Vector4f32),
		1,
		0);
}

D3DProceduralApp::~D3DProceduralApp()
{
	DeleteShaders();
	delete psconstants;
	delete myRSState;
	delete myD3DIndexBuffer;
	delete myD3DVertexBuffer;
	delete myD3DSwapChain;
	delete myD3DDevice;
	delete fenetreD3D;
}

void D3DProceduralApp::LoadShaders()
{
	kT::D3D11Shader* tempps;
	try{
		 tempps = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/proc/ps.hlsl", kT::D3D11Shader::PixelShader, kT::D3D11Shader::Profile4, "main");
	}catch (const kT::Exception& e){
		std::cout << e.what() << std::endl;
		return ;
	}

	kT::D3D11Shader* tempvs;
	try{
		tempvs = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/proc/vs.hlsl", kT::D3D11Shader::VertexShader, kT::D3D11Shader::Profile4, "main");
	}
	catch (const kT::Exception& e){
		std::cout << e.what() << std::endl;
		return;
	}

	std::cout << "[Shaders loaded]" << std::endl;

	DeleteShaders();
	
	d3dps = tempps;
	d3dvs = tempvs;

	kT::InputElementDesc delementsDesc[] = {
		{ 0, "POSITION", 0, kT::PixelFormat::RGB32_FLOAT, 0, kT::InputElementClass::PerVertex, 0 }
	};

	kT::InputLayoutDesc dlayoutDesc = {
		1,
		delementsDesc
	};

	myD3DLayout = new kT::D3D11InputLayout(myD3DDevice, d3dvs, dlayoutDesc);
	myD3DLayout->ComputeStrides(myStrides);
	memset(myOffsets, 0, sizeof(size_t)* 16);
}

void D3DProceduralApp::DeleteShaders()
{
	if (myD3DLayout != 0)
	{
		delete myD3DLayout;
		myD3DLayout = 0;
	}

	if (d3dps != 0)
	{
		delete d3dps;
		d3dps = 0;
	}

	if (d3dvs != 0)
	{
		delete d3dvs;
		d3dvs = 0;
	}
}

void D3DProceduralApp::Run()
{
	kT::Clock totalTimeClk;
	kT::Clock clk;
	clk.Reset();
	totalTimeClk.Reset();

	t = 0.f;

	bool terminer = false;
	while (!terminer)
	{
		kT::GUIEvent evenements;
		fenetreD3D->ProcessEvents();
		while (fenetreD3D->GetEvent(evenements) )
		{
			if (evenements.Type == kT::GUIEvent::KeyPressed)
			{
				if( evenements.Key.Code == kT::Keyboard::Escape)
				{
					terminer = true;
					break;
				}

				if (evenements.Key.Code == kT::Keyboard::R)
				{
					LoadShaders();
				}
			}

			if (evenements.Type == kT::GUIEvent::Closed)
			{
				terminer = true;
				break;
			}
		}

		float dt = clk.GetTimeElapsedReset().AsSeconds();
		Update(dt);
		Draw(dt);
		clk.Reset();

		t = totalTimeClk.GetTimeElapsed().AsSeconds();
	}
}

void D3DProceduralApp::Update(float dt)
{
}

void D3DProceduralApp::Draw(float dt)
{
	kT::Vector4f32 cst;
	cst.x = t;
	psconstants->WriteData(myD3DContext, 0, sizeof(cst), &cst.x);
	myD3DContext->PSSetConstantBuffers(0, 1, &psconstants);

	kT::D3D11Texture* targets[] = {
		myD3DSwapChain->GetTexture()
	};

	// Set the viewport
	kT::Sizeui32 ssize = myD3DSwapChain->GetTexture()->GetSize();

	kT::ViewportDesc vp(0.0f, 0.0f, static_cast<float>(ssize.Width), static_cast<float>(ssize.Height));
	myD3DContext->RSSetViewports(1, &vp);
	myD3DContext->IASetPrimitiveTopology(kT::PrimitiveTopology::TriangleList);

	myD3DContext->OMSetRenderTargets(1, targets, 0);
	myD3DContext->ClearRenderTarget(myD3DSwapChain->GetTexture(), kT::Vector4f32(0.0f, 0.0f, 0.0f, 0.0f));

	myD3DContext->VSSetShader(d3dvs);
	myD3DContext->PSSetShader(d3dps);
	myD3DContext->IASetInputLayout(myD3DLayout);
	myD3DContext->IASetIndexBuffer(myD3DIndexBuffer, kT::PixelFormat::R16_UINT, 0);

	kT::D3D11HardwareBuffer* dbuffers[] = {
		myD3DVertexBuffer
	};

	myD3DContext->IASetVertexBuffers(0, 1, dbuffers, myStrides, myOffsets);
	myD3DContext->DrawIndexed(6, 0, 0);
	myD3DSwapChain->Present(true);
}