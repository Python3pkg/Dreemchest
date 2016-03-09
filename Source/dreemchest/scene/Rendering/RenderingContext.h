/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#ifndef __DC_Scene_RenderingContext_H__
#define __DC_Scene_RenderingContext_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Rendering context.
	class RenderingContext : public RefCounted {
	public:

		//! Renderable item.
		struct Renderable {
										//! Constructs Renderable instance
										Renderable( Renderer::PrimitiveType primitiveType = Renderer::TotalPrimitiveTypes, Renderer::VertexBufferPtr vertexBuffer = Renderer::VertexBufferPtr(), Renderer::IndexBufferPtr indexBuffer = Renderer::IndexBufferPtr() )
											: primitiveType( primitiveType ), vertexBuffer( vertexBuffer ), indexBuffer( indexBuffer ) {}

			Renderer::PrimitiveType		primitiveType;	//!< Rendering primitive type.
			Renderer::VertexBufferPtr	vertexBuffer;	//!< Renderable vertex buffer.
			Renderer::IndexBufferPtr	indexBuffer;	//!< Renderable index buffer.
		};

		//! Returns the renderable by index.
		const Renderable&					renderable( u32 index ) const;

		//! Returns the texture by index.
		const Renderer::TexturePtr&			texture( u32 index ) const;

		//! Returns RVM.
		RvmPtr								rvm( void ) const;

		//! Returns shader cache.
		ShaderCachePtr						shaders( void ) const;

		//! Returns hal.
		Renderer::HalPtr					hal( void ) const;

		//! Returns renderer.
		Renderer::Renderer2DPtr				renderer( void ) const;

		//! Creates renderable asset from a mesh and uploads it to GPU.
		const RenderingAssetId&				uploadRenderable( MeshWPtr mesh, s32 chunk );

		//! Constructs texture from image asset and loads it to GPU.
		const RenderingAssetId&				uploadTexture( ImageWPtr image );

		//! Creates new rendering context.
		static RenderingContextPtr			create( const Renderer::HalPtr& hal );

	private:

											//! Constructs the RenderingContext instance.
											RenderingContext( RvmPtr rvm, ShaderCachePtr shaders, Renderer::HalPtr hal, Renderer::Renderer2DPtr renderer );

	private:

		RvmPtr								m_rvm;			//!< Rendering virtual machine.
		ShaderCachePtr						m_shaders;		//!< Shaders cache.
		Renderer::HalPtr					m_hal;			//!< Rendering HAL.
		Renderer::Renderer2DPtr				m_renderer;		//!< Rendering interface.
		Array<Renderable>					m_renderables;	//!< All renderable assets reside here.
		Array<Renderer::TexturePtr>			m_textures;		//!< All textures reside here.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */