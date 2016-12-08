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

#ifndef        __DC_R_OpenGLHal_H__
#define        __DC_R_OpenGLHal_H__

#include    "../Hal.h"

#ifdef DC_DEBUG
    #ifdef DC_THREADS_ENABLED
        #define CHECK_THREAD DC_ASSERT( m_renderThread == thread::Thread::currentThread(), "Accessing OpenGL from another thread" )
    #else
        #define CHECK_THREAD
    #endif

    #define        DC_CHECK_GL_CONTEXT    DC_ASSERT( glGetString( GL_EXTENSIONS ) != NULL, "OpenGL context should be initialized" )
    #define     DC_CHECK_GL         sOpenGLErrorCheck __gl_check; CHECK_THREAD
    #define     DC_CHECK_GL_ERROR   DC_EXPECT( glGetError() == GL_NO_ERROR, "Unexpected OpenGL error occured" )
#else
    #define        DC_CHECK_GL_CONTEXT
    #define     DC_CHECK_GL
    #define     DC_CHECK_GL_ERROR
#endif

// ** OpenGL headers and libraries
#if defined( DC_PLATFORM_WINDOWS )
    #include    <windows.h>

    #include    <gl/gl.h>
    #include    <gl/glu.h>
    #include    "Windows/glext.h"
    #include    "Windows/wglext.h"
#endif

#if defined( DC_PLATFORM_ANDROID )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif

#if defined( DC_PLATFORM_HTML5 )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif

#if defined( DC_PLATFORM_IOS )
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#endif

#if defined( DC_PLATFORM_MACOS )
    #include <OpenGL/gl.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/glext.h>
#endif

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

DC_BEGIN_DREEMCHEST

namespace Renderer {

    // ** struct sOpenGLErrorCheck
    struct sOpenGLErrorCheck {
        sOpenGLErrorCheck( void ) { NIMBLE_BREAK_IF( DumpErrors() ); }
        ~sOpenGLErrorCheck( void ) { NIMBLE_BREAK_IF( DumpErrors() ); }

        bool DumpErrors( void ) {
            bool hasErrors = false;

            do {
                GLenum error = glGetError();
                if( error == GL_NO_ERROR ) {
                    break;
                }

                switch( error ) {
                case GL_INVALID_ENUM:       LogError( "opengl", "%s", "GL_INVALID_ENUM\n" );      break;
                case GL_INVALID_VALUE:      LogError( "opengl", "%s", "GL_INVALID_VALUE\n" );     break;
                case GL_INVALID_OPERATION:  LogError( "opengl", "%s", "GL_INVALID_OPERATION\n" ); break;
                case GL_OUT_OF_MEMORY:      LogError( "opengl", "%s", "GL_OUT_OF_MEMORY\n" );     break;
                case GL_STACK_OVERFLOW:     LogError( "opengl", "%s", "GL_STACK_OVERFLOW\n" );    break;
                case GL_STACK_UNDERFLOW:    LogError( "opengl", "%s", "GL_STACK_UNDERFLOW\n" );   break;
                default:                    LogError( "opengl", "%s", "Unknown OpenGL error\n" );
                                            continue;
                }

                hasErrors = true;
            } while( /*true*/false );
            
            return hasErrors;
        }
    };

    // ** class OpenGLView
    class OpenGLView : public RenderView {
    public:

    };

    // ** class OpenGLHal
    class OpenGLHal : public Hal {
    public:

                                    OpenGLHal( RenderView* view );

        // ** Renderer
        virtual bool                clear( const Rgba& clearColor, f32 depth, u32 stencil, u32 mask );
        virtual void                present( void );
        virtual void                renderPrimitives( PrimitiveType primType, u32 offset, u32 count );
        virtual void                renderIndexed( PrimitiveType primType, u32 firstIndex, u32 count );
        virtual Texture2DPtr        createTexture2D( u32 width, u32 height, PixelFormat format );
        virtual TextureCube*        createTextureCube( u32 size, PixelFormat format );
        virtual RenderTargetPtr     createRenderTarget( u32 width, u32 height );
        virtual ShaderPtr            createShader( const char *vertex, const char *fragment );
        virtual IndexBufferPtr      createIndexBuffer( u32 count, bool GPU = true );
        virtual ConstantBufferPtr   createConstantBuffer( u32 size, const ConstantBufferLayout* layout = NULL );
        virtual VertexBufferPtr        createVertexBuffer( s32 size, bool GPU = true );
        virtual void                setPolygonMode( PolygonMode mode );
        virtual void                setShader( const ShaderPtr& shader );
        virtual void                setRenderTarget( const RenderTargetPtr& renderTarget );
        virtual void                setTexture( u32 sampler, Texture *texture );
        virtual void                setSamplerState( u32 sampler, TextureWrap wrap, TextureFilter filter );
        virtual void                setFog( FogMode mode, f32 density = 1.0f, const Rgba& color = Rgba( 0.0f, 0.0f, 0.0f, 1.0f ), f32 linearStart = 0.0f, f32 linearEnd = 1.0f );
        virtual void                setVertexBuffer( const VertexBufferPtr& vertexBuffer );
        virtual void                setIndexBuffer( const IndexBufferPtr& indexBuffer );
        virtual void                setInputLayout( const InputLayoutPtr& inputLayout );
        virtual void                setConstantBuffer( const ConstantBufferPtr& constantBuffer, s32 location );
        virtual void                setViewport( u32 x, u32 y, u32 width, u32 height );
        virtual void                setColorMask( u32 value );
        virtual void                setBlendFactors( BlendFactor source, BlendFactor destination );
        virtual void                setAlphaTest( Compare compare, f32 value );
        virtual void                setCulling( TriangleFace value );
        virtual void                setScissorTest( bool enabled, u32 x, u32 y, u32 width, u32 height );
        virtual void                setDepthTest( bool mask, Compare compare );
        virtual void                setStencilAction( TriangleFace face, Compare compare, StencilAction pass, StencilAction depthFail, StencilAction stencilFail );
        virtual void                setStencilValue( u32 value, u32 mask );
        virtual void                setBlendState( BlendState* state );
        virtual void                setDepthStencilState( DepthStencilState* state );
        virtual void                setRasterizerState( RasterizerState* state );
        virtual void                setTransform( Transform transform, const float* matrix );
        virtual void                setColorModulation( f32 r, f32 g, f32 b, f32 a );
        virtual void                setPolygonOffset( f32 factor, f32 units );

        // ** OpenGLRenderer
        static GLenum               blendFactor( u32 factor );
        static GLenum               compareFunc( u32 compare );
        static GLenum               triangleFace( u32 face );
        static GLenum               stencilOp( u32 action );
        static GLenum               textureWrap( u32 wrap );
        static GLenum               textureFilter( u32 filter );
        static GLenum               textureType( const Texture *texture );
        static GLuint               textureID( const Texture *texture );
        static GLenum               internalImageFormat( u32 pixelFormat );
        static GLenum               imageFormat( u32 pixelFormat );
        static GLenum               imageDataType( u32 pixelFormat );
        static u32                  imageAlign( u32 pixelFormat );

    private:

        void                        enableInputLayout( const u8 *pointer, const InputLayoutWPtr& inputLayout );
        void                        disableInputLayout( const InputLayoutWPtr& inputLayout );

    private:

        // ** struct sSampler
        struct sSampler {
            TextureWPtr m_texture;
            GLenum      m_wrap;
            GLenum      m_filter;
        };

        static const u32            MAX_SAMPLERS = 32;

        u32                         m_stencilValue, m_stencilMask;
        Compare                     m_stencilFunc;
        sSampler                    m_samplers[MAX_SAMPLERS];
        IndexBufferWPtr             m_activeIndexBuffer;
    };

    // ** class OpenGLTexture2D
    class OpenGLTexture2D : public Texture2D {
    friend class OpenGLHal;
    public:

                                    OpenGLTexture2D( u32 width, u32 height, PixelFormat format );
        virtual                     ~OpenGLTexture2D( void );

        // ** Texture2D
        virtual void                setData( u32 level, const void* data );
        virtual void*               lock( u32 level, u32& size );
        virtual void                unlock( void );

        // ** OpenGLTexture2D
        GLuint                      id( void ) const;

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLTextureCube
    class OpenGLTextureCube : public TextureCube {
    friend class OpenGLHal;
    public:

                                    OpenGLTextureCube( u32 size, PixelFormat format );

        // ** TextureCube
        virtual void*               lock( u32 face );
        virtual void                unlock( u32 face );

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLRenderTarget
    class OpenGLRenderTarget : public RenderTarget {
    friend class OpenGLHal;
    public:

                                    OpenGLRenderTarget( u32 width, u32 height );
        virtual                     ~OpenGLRenderTarget( void );

        // ** RenderTarget
        virtual bool                setAttachment( PixelFormat format, Attachment attachment );
        virtual bool                setDepth( PixelFormat format );

        // ** OpenGLRenderTarget
        bool                        check( void ) const;

    private:

        GLuint                      m_id;
        GLuint                        m_depth;
    };

    // ** class OpenGLVertexBuffer
    class OpenGLVertexBuffer : public VertexBuffer {
    friend class OpenGLHal;
    public:

                                    OpenGLVertexBuffer( s32 size );
        virtual                     ~OpenGLVertexBuffer( void );

        // ** VertexBuffer
        virtual void*               lock( void );
        virtual void                unlock( void );
        virtual void                setBufferData( const void* source, s32 offset, s32 size );

        // ** OpenGLVertexBuffer
        void                        bind( void );

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLIndexBuffer
    class OpenGLIndexBuffer : public IndexBuffer {
    friend class OpenGLHal;
    public:

                                    OpenGLIndexBuffer( u32 count );
        virtual                     ~OpenGLIndexBuffer( void );

        // ** IndexBuffer
        virtual u16*                lock( void );
        virtual void                unlock( void );

        // ** OpenGLIndexBuffer
        void                        bind( void );

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLShader
    class OpenGLShader : public Shader {
    friend class OpenGLHal;
    public:

                                    OpenGLShader( void );
        virtual                     ~OpenGLShader( void );

        // ** Shader
        virtual u32                 findUniformLocation( CString name ) const;
        virtual u32                 findUniformLocation( const FixedString& name ) const;
        virtual void                setMatrix( u32 location, const f32 value[16] );
        virtual void                setInt( u32 location, u32 value );
        virtual void                setFloat( u32 location, f32 value );
        virtual void                setVec2( u32 location, const Vec2& value );
        virtual void                setVec3( u32 location, const Vec3& value );
        virtual void                setVec4( u32 location, const Vec4& value );

        // ** OpenGLShader
        bool                        compile( GLenum shaderType, CString data, char *error, u32 errSize );
        bool                        link( char *error, u32 errSize ) const;

    private:

        typedef HashMap<String32, u32> UniformLocations;

        GLuint                      m_program;
        GLuint                      m_vertex, m_fragment;
        mutable UniformLocations    m_uniformLocations;
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif        /*    !__DC_R_OpenGLRenderer_H__    */
