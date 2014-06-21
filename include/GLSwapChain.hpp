#ifndef __KTGLSWAPCHAIN_HPP__
#define __KTGLSWAPCHAIN_HPP__

#include <kT/Graphics/PixelFormat.hpp>
#include <kT/Math/Size.hpp>

#include <GL/glew.h>

namespace kT
{
	class OGL3Device;
	class OGL3Texture;

	class GLSwapChain
	{
	public:

		GLSwapChain(
				OGL3Device* dev,
				PixelFormat::Format format,
				const Sizeui32& size
			);

		~GLSwapChain();

		OGL3Texture* GetBackTexture() { return myBackTexture; }

		void Present();

	protected:

		OGL3Texture* myBackTexture;

		GLuint myPBO;
	};
}

#endif // __KTGLSWAPCHAIN_HPP__