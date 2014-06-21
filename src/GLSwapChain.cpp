#include <GLSwapChain.hpp>
#include <kT/Graphics/OGL3Device/OGL3Texture.hpp>

namespace kT
{
	GLSwapChain::GLSwapChain(
		OGL3Device* dev,
		PixelFormat::Format format,
		const Sizeui32& size ):
		myBackTexture(0)
	{
		myBackTexture = new OGL3Texture(dev, kT::Texture::RenderTarget, format, size);
		
		auto pfd = PixelUtility::GetPixelFormatDescriptor(format);

		glGenBuffers(1, &myPBO);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, myPBO);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, size.Width*size.Height*pfd.GetSize(), 0, GL_STREAM_DRAW);
	}

	GLSwapChain::~GLSwapChain()
	{
		glDeleteBuffers(1, &myPBO);
		delete myBackTexture;
	}

	void GLSwapChain::Present()
	{

	}
}