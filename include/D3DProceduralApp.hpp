#ifndef __D3DPROCEDURALAPP_HPP__
#define __D3DPROCEDURALAPP_HPP__

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
	class D3D11Texture;
}

class D3DProceduralApp
{
public:

	D3DProceduralApp();

	~D3DProceduralApp();

	void LoadShaders();

	void DeleteShaders();

	void Run();

	void Update(float dt);

	void Draw(float dt);

protected:

	void GenerateHeightMap();

	size_t myOffsets[16];
	size_t myStrides[16];

	kT::Window *fenetreD3D;

	kT::D3D11Device* myD3DDevice;
	kT::D3D11ImmediateContext* myD3DContext;
	kT::D3D11SwapChain* myD3DSwapChain;

	kT::D3D11InputLayout* myD3DLayout;

	kT::D3D11HardwareBuffer* myD3DVertexBuffer;
	kT::D3D11HardwareBuffer* myD3DIndexBuffer;

	kT::D3D11Shader* d3dvs;
	kT::D3D11Shader* d3dps;
	kT::D3D11HardwareBuffer* psconstants;

	kT::D3D11RasterizerState* myRSState;

	float t;
};

#endif // __D3DPROCEDURALAPP_HPP__