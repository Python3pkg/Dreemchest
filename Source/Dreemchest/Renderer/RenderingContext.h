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

#ifndef __DC_Renderer_RenderingContext_H__
#define __DC_Renderer_RenderingContext_H__

#include "RenderState.h"
#include "PipelineFeatureLayout.h"
#include "ShaderLibrary.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Rendering context.
    class RenderingContext : public RefCounted
    {
    public:
        
        //! Cleans all allocated resources.
        virtual                                 ~RenderingContext( void );
        
        //! Sets a default state block.
        void                                    setDefaultStateBlock(const StateBlock& value);
        
        //! Returns a default state block.
        const StateBlock&                       defaultStateBlock() const;
        StateBlock&                             defaultStateBlock();
        
        //! Sets a default program.
        void                                    setDefaultProgram(Program value);
        
        //! Displays a frame captured by a render scene.
        void                                    display(RenderFrame& frame, bool wait = false);
        
        //! Forces a rendering context to construct all queued resources.
        void                                    construct(RenderFrame& frame);
        
        //! Queues an input layout instance for creation and returns it's index.
        InputLayout                             requestInputLayout( const VertexFormat& vertexFormat );

        //! Queues a uniform layut instance for creation and returns it's index.
        UniformLayout                           requestUniformLayout(const String& name, const UniformElement* elements);
        
        //! Queues a vertex buffer instance for creation and returns it's index.
        VertexBuffer_                           requestVertexBuffer( const void* data, s32 size );
        
        //! Queues an index buffer instance for creation and returns it's index.
        IndexBuffer_                            requestIndexBuffer( const void* data, s32 size );
        
        //! Queues a constant buffer instance for creation and returns it's index.
        ConstantBuffer_                         requestConstantBuffer(const void* data, s32 size, UniformLayout layout);

        //! Queues a constant buffer instance for creation and returns it's index.
        ConstantBuffer_                         deprecatedRequestConstantBuffer(const void* data, s32 size, const UniformElement* elements);
        
        //! Queues a 2D texture instance for creation and returns it's index.
        Texture_                                requestTexture2D(const void* data, u16 width, u16 height, PixelFormat format, TextureFilter filter = FilterMipLinear);
        
        //! Queues a cube texture instance for creation and returns it's index.
        Texture_                                requestTextureCube(const void* data, u16 size, u16 mipLevels, PixelFormat format, TextureFilter filter = FilterMipLinear);
        
        //! Queues a pipeline feature layout instance for creation and returns it's index.
        FeatureLayout                           requestPipelineFeatureLayout(const PipelineFeature* features);
        
        //! Queues a program instance creation and returns it's index.
        Program                                 requestProgram(const ShaderProgramDescriptor& descriptor);
        
        //! Queues a program instance creation and returns it's index.
        Program                                 requestProgram(VertexShader vertexShader, FragmentShader fragmentShader = FragmentShader());
        
        //! Queues a program instance creation and returns it's index.
        Program                                 requestProgram(const String& vertex, const String& fragment);
        
        //! Queues a constant buffer destruction.
        void                                    deleteConstantBuffer(ConstantBuffer_ id);
        
        //! Queues a uniform layout destruction.
        void                                    deleteUniformLayout(UniformLayout id);
        
        //! Queues a program destruction.
        void                                    deleteProgram(Program id);

        //! Returns a uniform buffer layout by a name.
        const UniformElement*                   findUniformLayout(const String& name) const;
        
        //! Queues a shader instance creation and returns it's index.
        Program                                 deprecatedRequestShader(const String& fileName);

    protected:
        
                                                //! Constructs a RenderingContext instance.
                                                RenderingContext(RenderViewPtr view);
        
        //! Executes a specified command buffer.
        virtual void                            executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands) NIMBLE_ABSTRACT;
        
        //! Executes a specified command buffer inside an intermediate render stack frame.
        void                                    execute( const RenderFrame& frame, const CommandBuffer& commands );
        
        //! Loads an transient resource to a specified slot.
        void                                    loadTransientResource(TransientResourceId index, ResourceId id);
        
        //! Unloads an transient resource from a specified slot.
        void                                    unloadTransientResource(TransientResourceId index);
        
        //! Returns an transient resource at specified slot.
        ResourceId                              transientResource(TransientResourceId index) const;
        
        //! Creates a vertex buffer layout instance from a flexible vertex format.
        VertexBufferLayoutUPtr                  createVertexBufferLayout(VertexFormat vertexFormat) const;
        
        //! Configures a pipeline state according to a state stack.
        void                                    applyStates(PipelineState& pipeline, const State* states, s32 count) const;
        
        //! Allocates a new persistent identifier of specified type.
        ResourceId                              allocateIdentifier(RenderResourceType::Enum type);
        
        //! Allocates a persistent identifier of specified type.
        template<typename TResourceIdentifier>
        TResourceIdentifier                     allocateIdentifier();
        
        //! Releases an allocated persistent identifier of specified type.
        void                                    releaseIdentifier(RenderResourceType::Enum type, ResourceId id);
        
    protected:

        //! A forward declaration of a stack type to store intermediate render targets.
        class TransientResourceStack;
        
        //! A maximum number of input layout types
        enum { MaxInputLayouts = 255 };
        
        RenderViewPtr                           m_view;                                                 //!< A rendering viewport.
        ResourceIdentifiers                     m_identifiers[RenderResourceType::TotalTypes];          //!< An array of persistent identifier managers.
        FixedArray<PipelineFeatureLayoutUPtr>   m_pipelineFeatureLayouts;                               //!< An array of constructed pipeline feature layouts.
        FixedArray<VertexBufferLayoutUPtr>      m_inputLayouts;                                         //!< Allocated input layouts.
        FixedArray<ShaderProgramDescriptor>     m_programs;                                             //!< Allocated shader programs.
        FixedArray<UniformBufferLayout>         m_uniformLayouts;                                       //!< Allocated uniform layouts.
        BidMap<String, UniformLayout>           m_uniformLayoutByName;                                  //!< Maps from a name to a uniform layout id.
        InputLayout                             m_inputLayoutCache[MaxInputLayouts];                    //!< A lookup table for input layout types.
        ResourceCommandBuffer*                  m_resourceCommandBuffer;                                //!< A command buffer that is used for resource construction commands.
        TransientResourceStack*                 m_transientResources;                                   //!< A transient resource stack.
        StateBlock                              m_defaultStateBlock;                                    //!< A default state block is applied after all commands were executed.
        Program                                 m_defaultProgram;                                       //!< A default program to be used.
        PipelineState                           m_pipeline;                                             //!< An active pipeline state.
        ShaderLibrary                           m_shaderLibrary;                                        //!< A shader library.
    };
    
    // ** RenderingContext::allocateIdentifier
    template<typename TResourceIdentifier>
    TResourceIdentifier RenderingContext::allocateIdentifier()
    {
        return allocateIdentifier(static_cast<RenderResourceType::Enum>(TResourceIdentifier::ResourceType));
    }
    
    //! Creates a rendering context that uses a deprecated rendering HAL interface.
    RenderingContextPtr createDeprecatedRenderingContext(RenderViewPtr view, HalPtr hal);
    
#ifdef DC_OPENGL_ENABLED
    //! Platform-specific OpenGL view constructor.
    extern RenderViewPtr createOpenGLView(void* window, PixelFormat depthStencil);
    
    //! Creates a rendering context that uses an OpenGL 2 rendering API.
    RenderingContextPtr createOpenGL2RenderingContext(RenderViewPtr view);
#endif    /*    DC_OPENGL_ENABLED    */
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_RenderingContext_H__  */
