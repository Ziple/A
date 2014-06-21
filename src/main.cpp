#include <GameApplication.hpp>
#include <D3DGameApplication.hpp>
#include <D3DProceduralApp.hpp>
#include <D3DTerrainApp.hpp>
#include <D3DParticleApp.hpp>
#include <D3Duv.hpp>

#include <kT/Core/Thread.hpp>

void* runD3DParticles(void* app)
{
	D3DParticleApp* d3dapp = new D3DParticleApp();
	d3dapp->Run();
	delete d3dapp;
	return 0;
}

void* runD3DUV(void* app)
{
	D3DUVApp* d3dapp = new D3DUVApp();
	d3dapp->Run();
	delete d3dapp;
	return 0;
}

void* runD3DProc(void* app)
{
	D3DProceduralApp* d3dapp = new D3DProceduralApp();
	d3dapp->Run();
	delete d3dapp;
	return 0;
}

void* runD3DTerrain(void* app)
{
	D3DTerrainApp* d3dapp = new D3DTerrainApp();
	d3dapp->Run();
	delete d3dapp;
	return 0;
}

void* runD3D( void* app )
{
	D3DGameApplication* d3dapp = new D3DGameApplication();
	d3dapp->Run();
	delete d3dapp;
	return 0;
}

void* runOGL( void* app )
{
	GameApplication* oglapp = new GameApplication();
	oglapp->Run();
	delete oglapp;
	return 0;
}

int main(int argc, char **argv)
{
	bool runBasicSamples = false;

	if (runBasicSamples)
	{
		kT::Thread* d3dRunner = new kT::Thread(runD3D, 0);
		kT::Thread* oglRunner = new kT::Thread(runOGL, 0);

		d3dRunner->Start();
		oglRunner->Start();

		d3dRunner->Wait();
		oglRunner->Wait();

		delete d3dRunner;
		delete oglRunner;
	}

	bool runTerrainSamples = false;

	kT::Thread* d3dTerrainRunner;
	if (runTerrainSamples)
	{
		d3dTerrainRunner = new kT::Thread(runD3DTerrain, 0);
		d3dTerrainRunner->Start();
	}

	bool runProceduralSamples = false;

	kT::Thread* d3dProcRunner;
	if (runProceduralSamples)
	{
		d3dProcRunner = new kT::Thread(runD3DProc, 0);
		d3dProcRunner->Start();
	}

	bool runUVSamples = false;

	kT::Thread* d3dUVRunner;
	if (runUVSamples)
	{
		d3dUVRunner = new kT::Thread(runD3DUV, 0);
		d3dUVRunner->Start();
	}

	bool runParticlesSample = true;

	kT::Thread* d3dParticlesRunner;
	if (runParticlesSample)
	{
		d3dParticlesRunner = new kT::Thread(runD3DParticles, 0);
		d3dParticlesRunner->Start();
	}

	if (runParticlesSample)
	{
		d3dParticlesRunner->Wait();
		delete d3dParticlesRunner;
	}

	if (runUVSamples)
	{
		d3dUVRunner->Wait();
		delete d3dUVRunner;
	}

	if (runTerrainSamples)
	{
		d3dTerrainRunner->Wait();
		delete d3dTerrainRunner;
	}

	if (runProceduralSamples)
	{
		d3dProcRunner->Wait();
		delete d3dProcRunner;
	}

    return 0;
}