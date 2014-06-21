#include <D3DTerrainApp.hpp>

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

#include <vector>

struct Matrices
{
	kT::Matrix4f32 Proj;
	kT::Matrix4f32 ModelView;
};

struct vert{
	kT::Vector3f32 pos;
	kT::Vector2f32 uv;
};

static unsigned perm[] =
{ 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69,
142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219,
203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175,
74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132,
187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3,
64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227,
47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221,
153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185,
112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145,
235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45,
127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61,
156, 180 };

static float unit = 0.70710678118654f;//sqrt(2.f)*0.5f
static float gradient2[][2] = { { unit, unit }, { -unit, unit }, { unit, -unit }, { -unit, -unit }, { 1.0f, 0 }, { -1.0f, 0 }, { 0, 1.0f }, { 0, -1.0f } };

float Get2DPerlinNoiseValue(float x, float y, float ires)
{

	//Adapter pour la résolution
	x *= ires;
	y *= ires;

	//On récupère les positions de la grille associée à (x,y)
	int x0 = (int)(x);
	int y0 = (int)(y);
	float x00 = floorf(x);
	float y00 = floorf(y);

	//Masquage
	int ii = x0 % 255;
	int jj = y0 % 255;

	//Pour récupérer les vecteurs
	unsigned gi0 = perm[ii + perm[jj]] & 7;
	unsigned gi1 = perm[ii + 1 + perm[jj]] & 7;
	unsigned gi2 = perm[ii + perm[jj + 1]] & 7;
	unsigned gi3 = perm[ii + 1 + perm[jj + 1]] & 7;

	//on récupère les vecteurs et on pondère
	float tempX = x - x00;
	float tempY = y - y00;
	float s = gradient2[gi0][0] * tempX + gradient2[gi0][1] * tempY;

	tempX = x - (x00 + 1.f);
	float t = gradient2[gi1][0] * tempX + gradient2[gi1][1] * tempY;

	tempX = x - x00;
	tempY = y - (y00 + 1.f);
	float u = gradient2[gi2][0] * tempX + gradient2[gi2][1] * tempY;

	tempX = x - (x00 + 1.f);
	float v = gradient2[gi3][0] * tempX + gradient2[gi3][1] * tempY;


	//Lissage
	const float tmp1 = x - x0;
	const float tmp2 = y - y0;
	float Cx = 3.f * tmp1 * tmp1 - 2.f * tmp1 * tmp1 * tmp1;
	float Cy = 3.f * tmp2 * tmp2 - 2.f * tmp2 * tmp2 * tmp2;

	float Li1 = s + Cx*(t - s);
	float Li2 = u + Cx*(v - u);

	return Li1 + Cy*(Li2 - Li1);
}

D3DTerrainApp::D3DTerrainApp() :
fenetreD3D(0),
myD3DDevice(0),
myD3DContext(0),
myD3DSwapChain(0),
myD3DDepthBuffer(0),
myD3DLayout(0),
myD3DVertexBuffer(0),
myD3DIndexBuffer(0),
d3dvs(0),
d3dps(0),
myD3DConstantBuffer(0),
myRSState(0)
{
	fenetreD3D = new kT::Window("A D3D Terrain", kT::Sizeui32(800, 600), kT::Window::Overlapped);
	fenetreD3D->Show();

	myD3DDevice = new kT::D3D11Device(kT::GraphicsDevice< kTD3D11DeviceTemplateListLineTypes >::HardwareProcessing, true);
	myD3DContext = myD3DDevice->GetImmediateContext();

	myD3DSwapChain = new kT::D3D11SwapChain(myD3DDevice, fenetreD3D, kT::PixelFormat::RGBA8_UNORM, true, 1);
	myD3DDepthBuffer = new kT::D3D11DepthStencilBuffer(myD3DDevice, kT::PixelFormat::DEPTH, myD3DSwapChain->GetTexture()->GetSize());

	numLines = 1024;
	numRows = 1024;
	numTriangles = (numLines - 1)*(numRows - 1) * 2;

	GenerateHeightMap();

	float dpx = 0.05f;
	float dpz = 0.05f;
	float startx = -0.5f * (float)(numRows - 1)*dpx;
	float startz = -0.5f * (float)(numLines - 1)*dpz;
	float du = 1.0f / float(numRows - 1);
	float dv = 1.0f / float(numLines - 1);

	float yampl = 5.0f;

	float res = 20.f;
	float ires = 1.f / res;
	float dx = 1024.0f / ((float)numRows);
	float dy = 1024.0f / ((float)numLines);

	std::vector<vert> vertices;
	vertices.resize(numLines*numRows);

	// Generate the vertices
	for (size_t i = 0; i < numLines; i++)
		for (size_t j = 0; j < numRows; j++)
		{
			float y = 0.5f + 0.5f * Get2DPerlinNoiseValue((float)j * dx, (float)i*dy, ires);//[0,1]
			y *= yampl;

			vertices[i * numRows + j].pos = kT::Vector3f32(startx + (float)j*dpx, y, startz + (float)i*dpz);
			vertices[i * numRows + j].uv = kT::Vector2f32((float)j*du, (float)i*dv);
		}

	std::vector<unsigned> indices;
	indices.resize(numTriangles * 3);

	// Generate the indices
	for (size_t i = 0; i < numLines - 1; i++)
		for (size_t j = 0; j < numRows - 1; j++)
		{
			indices[6 * (i * (numRows - 1) + j) + 0] = (i * numRows + j);
			indices[6 * (i * (numRows - 1) + j) + 1] = ((i + 1) * numRows + j);
			indices[6 * (i * (numRows - 1) + j) + 2] = (i * numRows + j + 1);

			indices[6 * (i * (numRows - 1) + j) + 3] = (i * numRows + j + 1);
			indices[6 * (i * (numRows - 1) + j) + 4] = ((i + 1) * numRows + j);
			indices[6 * (i * (numRows - 1) + j) + 5] = ((i + 1) * numRows + j + 1);
		}


	myD3DVertexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::VertexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(vert),
		numLines*numRows,
		&vertices[0]);

	myD3DIndexBuffer = new kT::D3D11HardwareBuffer(
		myD3DDevice,
		kT::D3D11HardwareBuffer::IndexBuffer,
		kT::D3D11HardwareBuffer::ImmutableBufferUsage,
		0,
		sizeof(unsigned),
		numTriangles*3,
		&indices[0]);

	LoadShaders();

	float zNear = 1.0f;
	float zFar = 100.0f;

	projection.BuildPerspectiveProjectionFOVRH(70.0f * kT::DegToRad32, (float)800.0f / 600.0f, zNear, zFar);
	modelview.BuildLookAtRH(kT::Vector3f32(0.f, 50.f, 0.f), kT::Vector3f32(0.0f, 0.0f, 0.0f), kT::Vector3f32(1.f, 0.f, 0.f));

	kT::RasterizerStateDesc rsdesc;
	rsdesc.frontCounterClockwise = true;
	rsdesc.cullMode = kT::RasterizerStateDesc::CullBack;
	myRSState = new kT::D3D11RasterizerState(myD3DDevice->GetHandle(), rsdesc);
	myD3DContext->RSSetState(myRSState);
}

D3DTerrainApp::~D3DTerrainApp()
{
	DeleteShaders();
	delete myNormalMap;
	delete myRSState;
	delete myD3DConstantBuffer;
	delete myD3DIndexBuffer;
	delete myD3DVertexBuffer;
	delete myD3DLayout;
	delete myD3DDepthBuffer;
	delete myD3DSwapChain;
	delete myD3DDevice;
	delete fenetreD3D;
}

void D3DTerrainApp::GenerateHeightMap()
{
	size_t width = 4*numRows;
	size_t height = 4*numLines;

	float yampl = 5.0f;

	float res = 20.f;
	float ires = 1.f / res;
	float dx = 1024.0f / ((float)width);
	float dy = 1024.0f / ((float)height);

	std::vector<kT::Vector3f32> vertices;
	vertices.resize(width*height);

	float dpx = 1.0f / (float(width));
	float dpz = 1.0f / (float(height));

	// Generate the vertices
	for (size_t i = 0; i < height; i++)
		for (size_t j = 0; j < width; j++)
		{
			float y = 0.5f + 0.5f * Get2DPerlinNoiseValue((float)j * dx, (float)i*dy, ires);//[0,1]
			y *= yampl;

			vertices[i * width + j] = kT::Vector3f32((float)j*dpx, y, (float)i*dpz);
		}

	std::vector<kT::Vector3f32> normals;
	normals.resize(width*height);
	std::vector<kT::Vector4ui8> normalMap;
	normalMap.resize(width*height);
	memset(&normals[0], 0, sizeof(kT::Vector3f32)*normals.size());

	// Generate the normals
	for (size_t i = 0; i < height - 1; i++)
		for (size_t j = 0; j < width - 1; j++)
		{
			kT::Vector3f32 A1 = vertices[((i + 1) * width + j)] - vertices[(i * width + j)];
			kT::Vector3f32 B1 = vertices[(i * width + j + 1)] - vertices[(i * width + j)];
			kT::Vector3f32 N1 = A1.Cross(B1);

			normals[(i * width + j)] += N1;
			normals[((i + 1) * width + j)] += N1;
			normals[(i * width + j + 1)] += N1;

			kT::Vector3f32 A2 = vertices[((i + 1) * width + j)] - vertices[(i * width + j + 1)];
			kT::Vector3f32 B2 = vertices[((i + 1) * width + j + 1)] - vertices[(i * width + j + 1)];
			kT::Vector3f32 N2 = A2.Cross(B2);

			normals[(i * width + j + 1)] += N2;
			normals[((i + 1) * width + j)] += N2;
			normals[((i + 1) * width + j + 1)] += N2;
		}

	// normalize the normals
	for (size_t i = 0; i < width*height; i++)
	{
		normals[i].Normalize();
		kT::Vector3f32 temp = 0.5f * normals[i] + kT::Vector3f32( 0.5f, 0.5f, 0.5f);
		normalMap[i] = kT::Vector4ui8(temp.x*float(0xFF), temp.y*float(0xFF), temp.z*float(0xFF), 0);
	}

	myNormalMap = new kT::D3D11Texture(
		myD3DDevice,
		kT::Texture::ShaderInput,
		kT::PixelFormat::RGBA8_UNORM,
		kT::Sizeui32(width, height),
		&normalMap[0],
		false);
}

void D3DTerrainApp::LoadShaders()
{

	d3dps = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/terrain/ps.hlsl", kT::D3D11Shader::PixelShader, kT::D3D11Shader::Profile4, "main");
	d3dvs = kT::D3D11Shader::LoadFromFile(myD3DDevice->GetHandle(), "shaders/hlsl/terrain/vs.hlsl", kT::D3D11Shader::VertexShader, kT::D3D11Shader::Profile4, "main");

	kT::InputElementDesc delementsDesc[] = {
		{ 0, "POSITION", 0, kT::PixelFormat::RGB32_FLOAT, 0, kT::InputElementClass::PerVertex, 0 },
		{ 0, "TEXCOORD", 0, kT::PixelFormat::RG32_FLOAT, sizeof(kT::Vector3f32), kT::InputElementClass::PerVertex, 0 }
	};

	kT::InputLayoutDesc dlayoutDesc = {
		2,
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

void D3DTerrainApp::DeleteShaders()
{
}

void D3DTerrainApp::Run()
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

void D3DTerrainApp::Update(float dt)
{
}

void D3DTerrainApp::Draw(float dt)
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
		myD3DVertexBuffer
	};

	myD3DContext->IASetVertexBuffers(0, 1, dbuffers, myStrides, myOffsets);

	myD3DContext->VSSetConstantBuffers(0, 1, &myD3DConstantBuffer);

	kT::D3D11ShaderResource* srvs[] = {
		myNormalMap
	};

	myD3DContext->PSSetShaderResources(0, 1, srvs);

	myD3DContext->DrawIndexed(numTriangles * 3, 0, 0);
	myD3DSwapChain->Present();

	kT::D3D11HardwareBuffer* n = 0;
	myD3DContext->VSSetConstantBuffers(0, 1, &n);
}