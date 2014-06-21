#ifndef __D3DPARTICLEAPP_HPP__
#define __D3DPARTICLEAPP_HPP__

#include <vector>

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
	class D3D11BlendState;
	class D3D11RasterizerState;
	class D3D11Texture;
}

struct particle
{
	kT::Vector3f32 pos;
	kT::Vector3f32 speed;
	kT::Vector4f32 color;
	float lifeTime;
};

class D3DParticleApp
{
public:

	D3DParticleApp();

	~D3DParticleApp();

	void LoadShaders();

	void DeleteShaders();

	void Run();

	void Update(float dt);

	void Draw(float dt);

protected:

	void GenerateColorMap();

	void UpdateParticles(float dt);
	void SortParticles();

	std::vector<particle> particles;

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
	kT::D3D11HardwareBuffer* myD3DInstanceVertexBuffer;
	kT::D3D11HardwareBuffer* myD3DIndexBuffer;

	kT::D3D11Shader* d3dvs;
	kT::D3D11Shader* d3dps;
	kT::D3D11HardwareBuffer* myD3DConstantBuffer;

	kT::D3D11BlendState* myBlendState;
	kT::D3D11RasterizerState* myRSState;

	kT::D3D11Texture* myColorMap;

	size_t numParticles;
};

#endif // __D3DPARTICLEAPP_HPP__