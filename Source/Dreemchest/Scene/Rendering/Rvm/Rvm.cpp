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

#include "Rvm.h"
#include "Commands.h"
#include "RenderFrame.h"
#include "../RenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Rvm::Rvm
Rvm::Rvm( RenderingContextWPtr context )
    : m_context( context )
    , m_hal( context->hal() )
{
    // Reset all state switchers
    memset( m_stateSwitches, 0, sizeof m_stateSwitches );

    // Setup render state switchers
    m_stateSwitches[RenderState::AlphaTest]         = &Rvm::switchAlphaTest;
    m_stateSwitches[RenderState::DepthState]        = &Rvm::switchDepthState;
    m_stateSwitches[RenderState::Blending]          = &Rvm::switchBlending;
    m_stateSwitches[RenderState::RenderTarget]      = &Rvm::switchRenderTarget;
    m_stateSwitches[RenderState::Shader]            = &Rvm::switchShader;
    m_stateSwitches[RenderState::ConstantBuffer]    = &Rvm::switchConstantBuffer;
    m_stateSwitches[RenderState::VertexBuffer]      = &Rvm::switchVertexBuffer;
}

// ** Rvm::create
RvmPtr Rvm::create( RenderingContextWPtr context )
{
    return DC_NEW Rvm( context );
}

// ** Rvm::display
void Rvm::display( const RenderFrameUPtr& frame )
{
    for( s32 i = 0, n = frame->commandBufferCount(); i < n; i++ ) {
        execute( *frame.get(), frame->commandBufferAt( i ) );
    }
}

// ** Rvm::execute
void Rvm::execute( const RenderFrame& frame, const RenderCommandBuffer& commands )
{
    for( s32 i = 0, n = commands.size(); i < n; i++ ) {
        // Get a render operation at specified index
        const RenderCommandBuffer::OpCode& opCode = commands.opCodeAt( i );

        // Apply rendering states from a stack
        applyStates( frame, opCode.states, MaxStateStackDepth );

        // Perform a draw call
        switch( opCode.type ) {
        case RenderCommandBuffer::OpCode::DrawIndexed:      m_hal->renderIndexed( opCode.primitives, Renderer::IndexBufferPtr(), opCode.first, opCode.count );
                                                            break;
        case RenderCommandBuffer::OpCode::DrawPrimitives:   m_hal->renderPrimitives( opCode.primitives, opCode.first, opCode.count );
                                                            break;
        default:                                            DC_NOT_IMPLEMENTED;
        }
    }
}

// ** Rvm::applyStates
void Rvm::applyStates( const RenderFrame& frame, const RenderStateBlock* const * states, s32 count )
{
    // A bitmask of states that were already set
    u32 activeStateMask = 0;

    for( s32 i = 0; i < count; i++ ) {
        // Get a state block at specified index
        const RenderStateBlock* block = states[i];

        // No more state blocks in a stack - break
        if( block == NULL ) {
            break;
        }

        // Skip redundant state blocks by testing a block bitmask against an active state mask
        if( (activeStateMask ^ block->mask()) == 0 ) {
            continue;
        }

        // Apply all states in a block
        for( s32 j = 0, n = block->stateCount(); j < n; j++ ) {
            // Get a render state bit
            u32 stateBit = block->stateBit( j );

            // Skip redundate state blocks by testing a state bitmask agains an active state mask
            if( activeStateMask & stateBit ) {
                continue;
            }

            // Get a render state at specified index
            const RenderState& state = block->state( j );

            // Update an active state mask
            activeStateMask = activeStateMask | stateBit;

            // Finally apply a state
            DC_ABORT_IF( m_stateSwitches[state.type] == NULL, "unhandled render state type" );
            (this->*m_stateSwitches[state.type])( frame, state );
        }
    }
}

// ** Rvm::switchAlphaTest
void Rvm::switchAlphaTest( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setAlphaTest( state.alpha.function, state.alpha.reference );
}

// ** Rvm::switchDepthState
void Rvm::switchDepthState( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setDepthTest( state.depth.write, state.depth.function );
}

// ** Rvm::switchBlending
void Rvm::switchBlending( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setBlendFactors( state.blend.src, state.blend.dst );
}

// ** Rvm::switchRenderTarget
void Rvm::switchRenderTarget( const RenderFrame& frame, const RenderState& state )
{
    const RenderTargetPtr& rt = frame.renderTarget( state.renderTarget.id );
    m_hal->setViewport( state.renderTarget.viewport[0], state.renderTarget.viewport[1], state.renderTarget.viewport[2], state.renderTarget.viewport[3] );
    rt->begin( m_hal );
}

// ** Rvm::switchShader
void Rvm::switchShader( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::ShaderPtr& shader = frame.shader( state.id );
    m_hal->setShader( shader );
}

// ** Rvm::switchConstantBuffer
void Rvm::switchConstantBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::ConstantBufferPtr& constantBuffer = frame.constantBuffer( state.constantBuffer.id );
    m_hal->setConstantBuffer( constantBuffer, state.constantBuffer.type );
}

// ** Rvm::switchVertexBuffer
void Rvm::switchVertexBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::VertexBufferPtr& vertexBuffer = frame.vertexBuffer( state.id );
    m_hal->setVertexBuffer( vertexBuffer );
}

} // namespace Scene

DC_END_DREEMCHEST