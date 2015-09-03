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

#include "WireframeRenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** WireframeRenderPass::begin
bool WireframeRenderPass::begin( u32 currentTime )
{
	if( !StaticMeshRenderPass::begin( currentTime ) ) {
		return false;
	}

	// Set wireframe polygon mode
	m_renderers.m_hal->setPolygonMode( Renderer::PolygonWire );

	// Set the additive blending
	m_renderers.m_hal->setBlendFactors( Renderer::BlendOne, Renderer::BlendOne );

	return true;
}

// ** WireframeRenderPass::end
void WireframeRenderPass::end( void )
{
	// Set the depth testing function
	m_renderers.m_hal->setDepthTest( true, Renderer::LessEqual );

	// Flush emitted render operations
	StaticMeshRenderPass::end();

	// Set fill polygon mode
	m_renderers.m_hal->setPolygonMode( Renderer::PolygonFill );
}

} // namespace Scene

DC_END_DREEMCHEST