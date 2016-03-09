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

#ifndef		__DC_Hal_H__
#define		__DC_Hal_H__

#include	"Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {

    // ** class RenderView
    //! RenderView class is a container for rendering surface.
    class dcInterface RenderView {
    public:
        
        virtual                     ~RenderView( void ) {}

        //! Activates the view for rendering.
        virtual bool                makeCurrent( void ) { return false; }

        //! Begins a frame rendering.
        virtual bool                beginFrame( void ) { return false; }

        //! Ends a frame rendering.
        virtual void                endFrame( void ) {}
    };

    // ** class Hal
    //! Hal class is a hardware abstraction layer around a low level graphics API.
    class dcInterface Hal : public RefCounted {
    public:

        virtual                     ~Hal( void );

        //! Clears the rendering surface.
        /*!
         \param clearColor  Value used to clear a color buffer (ClearColor should be passed, see mask parameter).
         \param depth       Value used to clear the depth buffer (ClearDepth should be passed, see mask parameter).
         \param stencil     Value used to clear the stencil buffer (ClearStencil should be passed, see mask parameter).
         \param mask        A bit mask that defines a set of buffers to be cleared.
         \return            Returns true if the device is ready to render the frame.
         */
        virtual bool        clear( const Rgba& clearColor = Rgba( 0.0f, 0.0f, 0.0f, 1.0f ), f32 depth = 1.0f, u32 stencil = 0, u32 mask = ClearAll );

        //! Present a rendered frame on a viewport.
        virtual void        present( void );

        //! Renders a list of primitives from a bound vertex buffer.
        /*!
         \param primType    Primitive type to be rendered.
         \param offset      Offset in a vertex buffer to start rendering from.
         \param count       Number of vertices to use in this draw call.
         */
        virtual void        renderPrimitives( PrimitiveType primType, u32 offset, u32 count );

        //! Renders a list of primitives from a bound vertex buffer and given index buffers.
        /*!
         \param primType    Primitive type to be rendered.
         \param indexBuffer Index buffer to use.
         \param firstIndex  First index to use in rendering.
         \param count       Number of indices to use in this draw call.
         */
        virtual void        renderIndexed( PrimitiveType primType, const IndexBufferPtr& indexBuffer, u32 firstIndex, u32 count );

        //! Creates a new 2D texture.
        /*!
         \param width   Texture width.
         \param height  Texture height.
         \param format  Texture pixel format.
         \return        Texture2D instance.
         */
        virtual Texture2DPtr	createTexture2D( u32 width, u32 height, PixelFormat format );

        //! Creates a new CUBE texture.
        /*!
         \param size    Cube texture dimensions.
         \param format  Texture pixel format.
         \return        TextureCube instance.
         */
        virtual TextureCube*    createTextureCube( u32 size, PixelFormat format );

        //! Creates a new 2D render target.
        /*!
         \param width   Render target width.
         \param height  Render target height.
         \param format  Render target pixel format.
         \return        RenderTarget instance.
         */
        virtual RenderTargetPtr   createRenderTarget( u32 width, u32 height );

        //! Creates a new shader.
        /*!
         \param vertex      Vertex shader source.
         \param fragment    Fragment shader source.
         \return            Shader instance.
         */
        virtual ShaderPtr     createShader( CString vertex, CString fragment );

        //! Creates a new vertex declaration object.
        /*!
         \param format      Vertex format (see VertexDeclaration for details).
         \param vertexSize  A single vertex size in bytes.
         \return            VertexDeclaration instance.
         */
        virtual VertexDeclarationPtr  createVertexDeclaration( CString format, u32 vertexSize = 0 );

        //! Creates a new index buffer.
        /*!
         \param count   The total amount of indices that reside inside this index buffer.
         \param GPU     Determines a location where to store the index buffer data (RAM or GPU).
         \return        IndexBuffer instance.
         */
        virtual IndexBufferPtr    createIndexBuffer( u32 count, bool GPU = true );

        //! Creates a new vertex buffer.
        /*!
         \param declaration Vertex declaration that defines a vertex layout.
         \param count       The total amount of vertices that reside inside this vertex buffer.
         \param GPU         Determines a location where to store the vertex buffer data (RAM or GPU).
         \return            VertexBuffer instance.
         */
        virtual VertexBufferPtr   createVertexBuffer( const VertexDeclarationPtr& declaration, u32 count, bool GPU = true );

        //! Binds a shader.
        virtual void    setShader( const ShaderPtr& shader );

        //! Binds a render target.
        virtual void    setRenderTarget( const RenderTargetPtr& renderTarget );

        //! Binds a texture to a specified texture sampler.
        virtual void    setTexture( u32 sampler, Texture *texture );

		//! Sets the polygon rendering mode.
		virtual void	setPolygonMode( PolygonMode mode );

        //! Sets a texture sampler state.
        /*!
         \param sampler Sampler index.
         \param wrap    Texture wrap.
         \param filter  Texture filtration.
         */
        virtual void    setSamplerState( u32 sampler, TextureWrap wrap, TextureFilter filter );

        //! Binds a vertex buffer.
        /*!
         \param vertexBuffer        Vertex buffer to be bound.
         \param vertexDeclaration   Override the vertex declaration.
         */
        virtual void    setVertexBuffer( const VertexBufferPtr& vertexBuffer, const VertexDeclarationWPtr& vertexDeclaration = VertexDeclarationWPtr() );

        //! Sets a rendering viewport.
        /*!
         \param x       Left side of a viewport.
         \param y       Bottom side of a viewport.
         \param width   Viewport width.
         \param height  Viewport height.
         */
        virtual void    setViewport( u32 x, u32 y, u32 width, u32 height );

		//! Sets a rendering viewport.
		virtual void	setViewport( const Rect& rect );

        //! Sets a rendering color mask.
        /*!
         \param value Mask value (see ColorMask enum for details).
         */
        virtual void    setColorMask( u32 value );

        //! Sets a blend factor.
        /*!
         \param source      Source pixel blend factor.
         \param destination Destination pixel blend factor.
         */
        virtual void    setBlendFactors( BlendFactor source, BlendFactor destination );

        //! Sets an alpha test.
        /*!
         \param compare Alpha test compare function.
         \param value Alpha reference value.
         */
        virtual void    setAlphaTest( Compare compare, f32 value = 1.0f );

        //! Sets face culling.
        virtual void    setCulling( TriangleFace value );

        //! Sets a scissor test.
        /*!
         \param enabled Flag indicating tha scissor test is enabled or not.
         \param x       Left side of a scissor rectangle.
         \param y       Bottom side of a scissor rectangle.
         \param width   Scissor rectangle width.
         \param height  Scissor rectangle height.
         */
        virtual void    setScissorTest( bool enabled, u32 x = 0, u32 y = 0, u32 width = 0, u32 height = 0 );

        //! Sets a depth test.
        /*!
         \param mask    Specifies whether the depth buffer is enabled for writing. If flag is false, depth buffer writing is disabled.
         \param compare Specifies the depth comparison function. The initial value of func is Less.
         */
        virtual void    setDepthTest( bool mask, Compare compare );

        //! Sets a stencil action.
        /*!
         \param face        Triangle face used. Pass TriangleFaceNone to disable stencil.
         \param compare     Specifies the test function.
         \param pass        Specifies the stencil action when both the stencil test and the depth test pass.
         \param depthFail   Specifies the stencil action when the stencil test passes.
         \param stencilFail Specifies the action to take when the stencil test fails.
         */
        virtual void    setStencilAction( TriangleFace face, Compare compare, StencilAction pass = StencilKeep, StencilAction depthFail = StencilKeep, StencilAction stencilFail = StencilKeep );

        //! Sets a reference value for stencil testing.
        /*!
         \param value   Specifies the reference value for the stencil test.
         \param mask    Specifies a mask that is ANDed with both the reference value and the stored stencil value when the test is done.
         */
        virtual void    setStencilValue( u32 value, u32 mask = ~0 );

        // ?? Thread-safe operations
		virtual void				createTexture2D( u32 width, u32 height, PixelFormat format, Texture2DPtr *texture );
        virtual void*               lockTexture( Texture2D* texture, u32& size );
        virtual void                unlockTexture( Texture2D* texture );

        // ??
        virtual void                setBlendState( BlendState* state );
        virtual void                setDepthStencilState( DepthStencilState* state );
        virtual void                setRasterizerState( RasterizerState* state );

        // ?? Fixed pipeline
        virtual void                setTransform( Transform transform, const float* matrix );
        virtual void                setColorModulation( f32 r, f32 g, f32 b, f32 a );
        virtual void                setFog( FogMode mode, f32 density = 1.0f, const Rgba& color = Rgba( 0.0f, 0.0f, 0.0f, 1.0f ), f32 linearStart = 0.0f, f32 linearEnd = 1.0f );

        //! Creates a new HAL instance.
        /*!
         \param renderer Rendering API to be used.
         \param view Rendering viewport to be used.
         */
        static Hal*                 create( RenderingHal renderer, RenderView* view = NULL );

        //! Creates and initializes a new OpenGL view.
        /*!
         \param window Platform-specific window handle (like HWND or NSWindow).
         \return OpenGLView instance.
         */
        static RenderView*          createOpenGLView( void* window, PixelFormat depthStencil = PixelD24S8 );

	protected:

									//! Constructs a new Hal instance.
                                    /*!
                                     \param view Viewport container.
                                     */
                                    Hal( RenderView* view );

    protected:

        //! Rendering viewport.
        RenderView*                 m_view;

    #ifdef DC_THREADS_ENABLED
        //! Current render thread ID.
        u32                         m_renderThread;
    #endif

        //! Current rasterizer state.
        //RasterizerState				m_rasterState2D;

        //! Current blend state.
        //BlendState					m_blendState2D;

        //! Current vertex declaration.
        VertexDeclarationWPtr		m_vertexDeclaration;
    };

    // ** class RenderResource
    //! RenderResource is a base class for all render resources.
    class dcInterface RenderResource : public RefCounted {
    public:

        virtual                     ~RenderResource( void ) {}
    };

    //! Texture is a base class for all hardware textures.
    class Texture : public RenderResource {
    public:

		//! Available texture types.
		enum Type {
			  TextureType2D				//!< 2D texture.
			, TextureTypeCube			//!< Cube texture.
		};

                                    //! Constructs a new instance of Texture with a given pixel format.
                                    Texture( PixelFormat format, Type type );

		//! Returns texture type.
		Type						type( void ) const;

        //! Returns a texture pixel format.
        PixelFormat                 pixelFormat( void ) const;

        //! Returns an amount of bytes per block for compressed textures.
        u32                         bytesPerBlock( void ) const;

        //! Returns an amount of bytes per pixel for non compressed textures.
		u32                         bytesPerPixel( void ) const;

        //! Returns a total amount of bytes for a mip level with a given dimensions.
		u32                         bytesPerMip( u32 width, u32 height ) const;

        //! Returns true if this texture is compressed.
        bool                        isCompressed( void ) const;

        //! Returns true if this texture is now locked for read/write access.
        bool                        isLocked( void ) const;

        //! Returns a pointer to locked texture data.
        void*                       locked( void ) const;

    protected:

        Type						m_type;			//!< Texture type.
        PixelFormat                 m_pixelFormat;	//!< Texture pixel format.
        void*                       m_locked;		//!< Pointer to a locked texture data.
        u32                         m_lockedLevel;	//!< Index of a locked texture mip level.
    };

    //! Texture2D class represents a 2D texture.
    class Texture2D : public Texture {
    public:

                                    //! Constructs a new Texture2D instance.
                                    /*!
                                     \param width Texture width.
                                     \param height Texture height.
                                     \param format Texture pixel format.
                                     */
                                    Texture2D( u32 width, u32 height, PixelFormat format );

        //! Returns a texture width.
        u32                         width( void ) const;

        //! Returns a texture height.
        u32                         height( void ) const;

        //! Uploads a texture data to a given mip level.
        virtual void                setData( u32 level, const void *data );

        //! Locks a given mip level.
        /*!
         \param level   Mip level to lock.
         \param size    The returned size of a locked surface.
         \return        Pointer to a locked texture data.
         */
        virtual void*               lock( u32 level, u32& size );

        //! Unlocks texture.
        virtual void                unlock( void );

    protected:

        u32                         m_width;	//!< Texture width.
        u32                         m_height;	//!< Texture height.
    };

    //! A TextureCube class.
    class TextureCube : public Texture {
    public:

                                    //! Constructs a new TextureCube instance.
                                    /*!
                                     \param size Cube texture dimensions.
                                     \param format Texture pixel format.
                                     */
                                    TextureCube( u32 size, PixelFormat format );

        //! Returns a dimensions of a cube texture.
        u32                         size( void ) const;

        //! Locks a given cube texture face.
        virtual void*               lock( u32 face );

        //! Unlocks a given cube texture face.
        virtual void                unlock( u32 face );

    private:

        u32                         m_size;	//!< Cube texture dimensions.
    };

    //! A render target.
    class RenderTarget : public RenderResource {
    public:

                                    //! Constructs a new RenderTarget instance.
                                    RenderTarget( u32 width, u32 height );

		//! Setups the color renderbuffer.
		virtual bool				setColor( PixelFormat format, u32 index = 0 );

		//! Setups the depth renderbuffer.
		virtual bool				setDepth( PixelFormat format );

		//! Returns the color attachment by index.
		Texture2DPtr				color( u32 index = 0 ) const;

		//! Returns render target width.
		u32							width( void ) const;

		//! Returns render target height.
		u32							height( void ) const;

	protected:

		Array<Texture2DPtr>			m_color;	//!< Color attachments.
		u32							m_width;	//!< Render target width.
		u32							m_height;	//!< Render target height.
    };

    // ** class VertexDeclaration
    //! VertexDeclaration class instance is used to define a vertex layout.
    //! The input format for vertex layout is formed from blocks [T][S]
    //! separated by [:]. Where T is attribute type and S is an attribute size.
    //! For example:
    //!     P2:S        - xy  position + point size
    //!     P3:T0:T1:C  - xyz position + uv 0 + uv 1 + RGBA color.
    class VertexDeclaration : public RenderResource {
    friend class Hal;
    public:

        // ** enum VertexAttribute
        //! Supported vertex attributes.
        enum VertexAttribute {
            VertexPosition,         //!< Vertex position.
            VertexNormal,           //!< Vertex normal.
            VertexColor,            //!< Vertex color.
            VertexPointSize,        //!< Point sprite size.
            VertexTex0,             //!< Texture 0 UV.
            VertexTex1,             //!< Texture 1 UV.
            VertexTex2,             //!< Texture 2 UV.
            VertexTex3,             //!< Texture 3 UV.
            VertexTex4,             //!< Texture 4 UV.

            VertexAttributesTotal,  //!< Total amount of supported vertex attributes.
        };

        // ** struct VertexElement
        //! Vertex element.
        struct VertexElement {
            s32     m_count;    //!< Attribute size.
            s32     m_offset;   //!< Attribute offset.

                    //! Constructs a new VertexElement instance.
                    VertexElement( void ) : m_count( -1 ), m_offset( -1 ) {}

            //! Returns true if this vertex element is used.
            operator bool() const { return m_count > 0 && m_offset >= 0; }
        };

    public:

        //! Returns a total vertex size in bytes.
        u32                     vertexSize( void ) const;

        //! Returns the vertex position attribute.
        const VertexElement&    position( void ) const;

        //! Returns the vertex color attribute.
        const VertexElement&    color( void ) const;

        //! Returns the vertex normal attribute.
        const VertexElement&    normal( void ) const;

        //! Returns the vertex UV attribute for a given sampler index.
        const VertexElement&    uv( u32 sampler ) const;

        //! Returns the point size attribute.
        const VertexElement&    pointSize( void ) const;

    private:

        //! Parses a vertex format from string.
        bool                    parse( CString format );

    protected:

        //! Vertex size in bytes.
        u32                     m_vertexSize;

        //! Array of vertex attributes.
        VertexElement           m_vertexElements[VertexAttributesTotal];
    };

	//! Vertex buffer class.
    class dcInterface VertexBuffer : public RenderResource {
    public:

                                    //! Constructs a new VertexBuffer instance.
                                    /*!
                                     \param vertexDeclaration Vertex buffer layout.
                                     \param count Total number of vertices that are reside inside this buffer.
                                     \param gpu Specifies whether this buffer resides in GPU memory or not.
                                     */
                                    VertexBuffer( const VertexDeclarationPtr& vertexDeclaration, u32 count, bool gpu );
		virtual						~VertexBuffer( void );

        //! Returns a vertex declaration of this vertex buffer.
        const VertexDeclarationPtr&	vertexDeclaration( void ) const;

        //! Returns a pointer to buffer vertex data.
        const void*                 pointer( void ) const;

        //! Returns a total buffer size.
        u32                         size( void ) const;

        //! Returns true if this buffer reside in GPU memory.
		bool						isGpu( void ) const;

        //! Locks a vertex buffer.
        virtual void*               lock( void );

        //! Unlocks a vertex buffer.
        virtual void                unlock( void );

		//! Lockes a vertex buffer and performs the type cast.
		template<typename TVertex>
		TVertex*					lock( void );

    protected:

        //! Vertex declaration.
        const VertexDeclarationPtr	m_vertexDeclaration;

        //! Vertex buffer size.
        u32                         m_size;

        //! Vertex buffer data.
		void*						m_data;

        //! Is it a GPU-side vertex buffer?
		bool						m_isGpu;
    };

	// ** VertexBuffer::lock
	template<typename TVertex>
	TVertex* VertexBuffer::lock( void )
	{
		return reinterpret_cast<TVertex*>( lock() );
	}

    // ** class IndexBuffer
    class dcInterface IndexBuffer : public RenderResource {
    public:

                                    //! Constructs a new IndexBuffer instance.
                                    /*!
                                     \param count Index buffer size (amount of indices stored).
                                     \param gpu Specifies whether this buffer resides in GPU memory or not.
                                     */
                                    IndexBuffer( u32 count, bool gpu );
		virtual						~IndexBuffer( void );

        //! Returns an index buffer size.
        u32                         size( void ) const;

        //! Returns a pointer to index buffer.
        const u16*                  pointer( void ) const;

        //! Returns true if this buffer reside in GPU memory.
		bool						isGpu( void ) const;

        //! Locks an index buffer.
        virtual u16*                lock( void );

        //! Unlocks an index buffer.
        virtual void                unlock( void );

    protected:

        //! Index buffer size.
        u32                         m_size;

        //! Index buffer data.
		u16*						m_data;

        //! Is it a GPU-side index buffer?
		bool						m_isGpu;
    };

    // ** class Shader
    class dcInterface Shader : public RenderResource {
    public:

        //! Searches for a shader uniform location.
        /*!
         \param name Uniform name.
         \return Uniform location index.
         */
        virtual u32                 findUniformLocation( CString name );
        
		//! Sets a 4x4 matrix value to a uniform location.
        virtual void                setMatrix( u32 location, const Matrix4& value );

        //! Sets an integer value to a uniform location.
        virtual void                setInt( u32 location, u32 value );

        //! Sets a float value to a uniform location.
        virtual void                setFloat( u32 location, f32 value );

        //! Sets a Vec2 value to a uniform location.
        virtual void                setVec2( u32 location, const Vec2& value );

		//! Sets a Vec3 value to a uniform location.
        virtual void                setVec3( u32 location, const Vec3& value );

		//! Sets a Vec4 value to a uniform location.
        virtual void                setVec4( u32 location, const Vec4& value );
    };

} // namespace Renderer

DC_END_DREEMCHEST

#endif		/*	!__DC_Hal_H__	*/