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

#ifndef __DC_Scene_RenderState_H__
#define __DC_Scene_RenderState_H__

#include "../../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! A maximum number of state blocks that can be pushed onto a state stack.
    enum { MaxStateStackDepth = 4 };

    //! Render state defines a single state change.
    struct RenderState {
        //! Available constant buffers.
        enum ConstantBufferType {
              GlobalConstants       //!< A constant buffer that stores global scene settings (ambient color, fog type, etc.).
            , PassConstants         //!< A constant buffer that stores a pass variables (view-projection matrix, light color, etc.).
            , InstanceConstants     //!< A constant buffer that stores instance variables (model matrix, instance color, etc.).
            , MaxConstantBuffers    //!< A maximum number of supported constant buffers.
        };

        //! Available texture samplers.
        enum TextureSampler {
              Texture0
            , Texture1
            , Texture2
            , MaxTextureSamplers    //!< A maximum number of supported texture samplers.
        };

        //! Available render state types.
        enum Type {
              VertexBuffer      = 0                                     //!< Binds an input vertex buffer.
            , IndexBuffer       = VertexBuffer   + 1                    //!< Binds an input index buffer.
            , ConstantBuffer    = IndexBuffer    + 1                    //!< Binds a constant buffer.
            , Shader            = ConstantBuffer + MaxConstantBuffers   //!< Binds a program instance.
            , RenderTarget      = Shader         + 1                    //!< Binds a render target.
            , Blending          = RenderTarget   + 1                    //!< Sets a blend function
            , DepthState        = Blending       + 1                    //!< Sets a depth test function and a reference value.
            , AlphaTest         = DepthState     + 1                    //!< Sets an alpha test function and a reference value.
            , Texture           = AlphaTest      + 1                    //!< Binds a texture to a sampler #(Type.Texture + index).
            , TotalStates       = Texture        + MaxTextureSamplers   //!< A total number of available render states.
        };

                                        //! Constructs an empty RenderState instance.
                                        RenderState( void );

                                        //! Constructs a RenderState instance of specified type.
                                        RenderState( Type type, s32 id );

                                        //! Constructs a depth render state instance.
                                        RenderState( Renderer::Compare function, bool write );

                                        //! Constructs an alpha test render state instance.
                                        RenderState( Renderer::Compare function, f32 reference );

                                        //! Constructs a constant buffer binding state.
                                        RenderState( s32 id, ConstantBufferType type );

                                        //! Constructs a blend function render state.
                                        RenderState( Renderer::BlendFactor src, Renderer::BlendFactor dst );

                                        //! Constructs a blend function render state.
                                        RenderState( s32 id, TextureSampler sampler );

                                        //! Constructs a render target state.
                                        RenderState( s32 id, const Rect& viewport );

        Type                            type;           //!< Render state type.
        union {
            s32                         id;             //!< A resource identifier to be bound to a pipeline.

            struct {
                s32                     id;             //!< Buffer ID to be bound.
                ConstantBufferType      type;           //!< Constant buffer index.
            } constantBuffer;                           //!< Bind constant buffer data.

            struct {
                Renderer::BlendFactor   src;            //!< A source blend factor.
                Renderer::BlendFactor   dst;            //!< A destination blend factor.
            } blend;                                    //!< Blend mode to be set.

            struct {
                Renderer::Compare       function;       //!< A depth tesing function.
                bool                    write;          //!< Enables or disables writing to a depth buffer.
            } depth;                                    //!< A depth testing state to be set.

            struct {
                Renderer::Compare       function;       //!< An alpha test function.
                f32                     reference;      //!< An alpha test function reference value.
            } alpha;                                    //!< An alpha testing state to be set.

            struct {
                s32                     id;             //!< Texture ID to be bound.
                TextureSampler          sampler;        //!< A sampler index to bind texture to.
            } texture;                                  //!< A texture sampler data.

            struct {
                s32                     id;             //!< Render target identifier.
                f32                     viewport[4];    //!< A target viewport.
            } renderTarget;                             //!< A render target state to be set.
        };
    };

    //! Render state block agregates a set of state changes.
    class RenderStateBlock {
    public:

                                        //! Constructs a RenderStateBlock block.
                                        RenderStateBlock( void );

        //! Binds a vertex buffer to a pipeline.
        void                            bindVertexBuffer( s32 id );

        //! Binds an index buffer to a pipeline.
        void                            bindIndexBuffer( s32 id );

        //! Binds a constant buffer to a pipeline.
        void                            bindConstantBuffer( s32 id, RenderState::ConstantBufferType type );

        //! Binds a program to a pipeline.
        void                            bindProgram( s32 id );

        //! Binds a texture to a specified sampler.
        void                            bindTexture( s32 id, RenderState::TextureSampler sampler );

        //! Sets a blend function.
        void                            setBlend( Renderer::BlendFactor src, Renderer::BlendFactor dst );

        //! Sets a depth state.
        void                            setDepthState( Renderer::Compare function, bool write );

        //! Sets an alpha test function.
        void                            setAlphaTest( Renderer::Compare function, f32 reference );

        //! Sets a render target.
        void                            setRenderTarget( s32 id, const Rect& viewport );

        //! Disables an alpha testing.
        void                            disableAlphaTest( void );

        //! Disables blending
        void                            disableBlending( void );

        //! Returns a state block mask.
        u32                             mask( void ) const;

        //! Returns a total number of states inside this block.
        s32                             stateCount( void ) const;

        //! Returns a state at specified index.
        const RenderState&              state( s32 index ) const;

        //! Returns a state bit at specified index.
        u32                             stateBit( s32 index ) const;

    private:

        //! Pushes a new state to a block.
        void                            pushState( const RenderState& state, u32 stateBit );

    private:

        u32                             m_mask;         //!< A bit mask of state changes that are preset inside this state block.
        Array<u32>                      m_stateBits;    //!< An array of state bits.
        Array<RenderState>              m_states;       //!< An array of state changes.
    };

    // ** RenderStateBlock::mask
    NIMBLE_INLINE u32 RenderStateBlock::mask( void ) const
    {
        return m_mask;
    }

    // ** RenderStateBlock::stateCount
    NIMBLE_INLINE s32 RenderStateBlock::stateCount( void ) const
    {
        return static_cast<s32>( m_states.size() );
    }

    // ** RenderStateBlock::state
    NIMBLE_INLINE const RenderState& RenderStateBlock::state( s32 index ) const
    {
        DC_ABORT_IF( index < 0 || index >= stateCount(), "index is out of range" );
        return m_states[index];
    }

    // ** RenderStateBlock::stateBit
    NIMBLE_INLINE u32 RenderStateBlock::stateBit( s32 index ) const
    {
        DC_ABORT_IF( index < 0 || index >= stateCount(), "index is out of range" );
        return m_stateBits[index];
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderState_H__    */