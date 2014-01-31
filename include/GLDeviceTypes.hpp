#ifndef __KTGLDEVICETYPES_HPP__
#define __KTGLDEVICETYPES_HPP__

#include <kT/Core/Build.hpp>
#include <kT/Graphics/GraphicsDevice/ImplementationForwardDeclarations.hpp>

namespace kT
{
	class GLDevice;
	class GLImmediateContext;
	class GLHardwareBuffer;
	class GLTexture;
	class GLRenderTarget;
	class GLSwapChain;
	class GLDepthStencilTarget;
	class GLDeferredContext;
	class GLImmediateContext;
	class GLInputLayout;
	class GLCommandList;
    class OGL3Shader;

	struct GLDeviceTypes
	{
		typedef GLDevice Device;
		typedef GLImmediateContext ImmediateContext;
		typedef GLHardwareBuffer HardwareBuffer;
		typedef GLTexture Texture;
		typedef GLRenderTarget RenderTarget;
		typedef GLSwapChain SwapChain;
		typedef GLDepthStencilTarget DepthStencilTarget;
		typedef GLDeferredContext DeferredContext;
		typedef GLImmediateContext ImmediateContext;
		typedef GLInputLayout InputLayout;
		typedef GLCommandList CommandList;
        typedef OGL3Shader Shader;
	};
}

#define kTGLDeviceTemplateListLineTypes kTGraphicsDeviceTemplateListLineTypes(kT::GLDeviceTypes)

#endif /* __KTGLDEVICETYPES_HPP__ */