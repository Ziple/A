#ifndef __D3DGAMEAPP_HPP__
#define __D3DGAMEAPP_HPP__

#include <kT/Math/Matrix4.hpp>

namespace kT
{
	class Window;
	class D3D11Device;
	class D3D11ImmediateContext;
	class D3D11HardwareBuffer;
	class D3D11InputLayout;
	class D3D11Shader;
	class D3D11SwapChain;
	class D3D11DepthStencilBuffer;
	class D3D11RasterizerState;
}

class D3DGameApplication
{
public:

	D3DGameApplication();

	~D3DGameApplication();

	void LoadShaders();

	void DeleteShaders();

	void Run();

	void Update(float dt);

	void Draw(float dt);

protected:

	size_t myOffsets[16];
	size_t myStrides[16];

	kT::Matrix4f32 modelview;
	kT::Matrix4f32 projection;

	kT::Window *fenetreD3D;

	kT::D3D11Device* myD3DDevice;
	kT::D3D11ImmediateContext* myD3DContext;
	kT::D3D11SwapChain* myD3DSwapChain;
	kT::D3D11DepthStencilBuffer* myD3DDepthBuffer;

	kT::D3D11InputLayout* myD3DLayout;

	kT::D3D11HardwareBuffer* myD3DVertexBuffer;
	kT::D3D11HardwareBuffer* myD3DCenterBuffer;
	kT::D3D11HardwareBuffer* myD3DIndexBuffer;

	kT::D3D11Shader* d3dvs;
	kT::D3D11Shader* d3dps;
	kT::D3D11HardwareBuffer* myD3DConstantBuffer;

	kT::D3D11RasterizerState* myRSState;
	size_t numParticles;
};

#endif // __DDGAMEAPP_HPP__