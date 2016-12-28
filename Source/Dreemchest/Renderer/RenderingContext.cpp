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

#include "RenderingContext.h"
#include "RenderingContextHal.h"
#include "CommandBuffer.h"
#include "VertexFormat.h"
#include "VertexBufferLayout.h"
#include "RenderFrame.h"

#include "../Io/DiskFileSystem.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ------------------------------------------------------------------ RenderingContext::TransientResourceStack ----------------------------------------------------------------- //

/*!
 Transient target stack is used to convert local indices that are
 stored in commands to a global index of an transient resources.
 */
class RenderingContext::TransientResourceStack
{
public:
    
    //! A maximum number of intermediate render targets that can be hold by a single stack frame.
    enum { StackFrameSize = 8 };
    
    //! A maximum number of stack frames that can be pushed during rendering.
    enum { MaxStackFrames = 8 };
    
    //! A total size of an intermediate stack size.
    enum { MaxStackSize = MaxStackFrames * StackFrameSize };
    
                                //! Constructs an TransientResourceStack instance.
                                TransientResourceStack();
    
    //! Pushes a new stack frame.
    void                        pushFrame();
    
    //! Pops an active stack frame.
    void                        popFrame();
    
    //! Returns a persistent resource by a transient one.
    ResourceId                  get(TransientResourceId index) const;
    
    //! Loads an acquired transient resource to a specified local slot.
    void                        load(TransientResourceId index, ResourceId id);
    
    //! Unloads a transient resource from a specified local slot.
    void                        unload(TransientResourceId index);
    
private:
    
    ResourceId*                 m_stackFrame;                   //!< An active render target stack frame.
    ResourceId                  m_identifiers[MaxStackSize];    //!< An array of intermediate render target handles.
};

// ** RenderingContext::TransientResourceStack::TransientResourceStack
RenderingContext::TransientResourceStack::TransientResourceStack( void )
    : m_stackFrame( m_identifiers )
{
    memset( m_identifiers, 0, sizeof m_identifiers );
}

// ** RenderingContext::TransientResourceStack::pushFrame
void RenderingContext::TransientResourceStack::pushFrame( void )
{
    NIMBLE_ABORT_IF( (m_stackFrame + StackFrameSize) > (m_identifiers + MaxStackSize), "frame stack overflow" );
    m_stackFrame += StackFrameSize;
}

// ** RenderingContext::TransientResourceStack::popFrame
void RenderingContext::TransientResourceStack::popFrame( void )
{
    NIMBLE_ABORT_IF( m_stackFrame == m_identifiers, "stack underflow" );
    
    // Ensure that all render targets were released
    for( s32 i = 0; i < StackFrameSize; i++ )
    {
        if( m_stackFrame[i] )
        {
            LogWarning( "rvm", "%s", "an intermediate render target was not released before popping a stack frame\n" );
        }
    }
    
    // Pop a stack frame
    m_stackFrame -= StackFrameSize;
}

// ** RenderingContext::TransientResourceStack::get
ResourceId RenderingContext::TransientResourceStack::get(TransientResourceId index) const
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    return m_stackFrame[index - 1];
}

// ** RenderingContext::TransientResourceStack::load
void RenderingContext::TransientResourceStack::load(TransientResourceId index, ResourceId id)
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = id;
}

// ** RenderingContext::TransientResourceStack::unload
void RenderingContext::TransientResourceStack::unload(TransientResourceId index)
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = 0;
}

// --------------------------------------------------------- RenderingContext::ConstructionCommandBuffer --------------------------------------------------------- //

//! An internal comamnd buffer type that is used for resource construction.
class RenderingContext::ConstructionCommandBuffer : public CommandBuffer
{
public:
    
    //! Emits an input layout creation command.
    InputLayout                 createInputLayout(InputLayout id, const VertexFormat& vertexFormat);
    
    //! Emits a vertex buffer creation command.
    VertexBuffer_               createVertexBuffer(VertexBuffer_ id, const void* data, s32 size);
    
    //! Emits an index buffer creation command.
    IndexBuffer_                createIndexBuffer(IndexBuffer_ id, const void* data, s32 size);
    
    //! Emits a constant buffer creation command.
    ConstantBuffer_             createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, UniformLayout layout);
    
    //! Emits a texture creation command.
    Texture_                    createTexture2D(Texture_ id, const void* data, u16 width, u16 height, PixelFormat format, TextureFilter filter);
    
    //! Emits a cube texture creation command.
    Texture_                    createTextureCube(Texture_ id, const void* data, u16 size, u16 mipLevels, PixelFormat format, TextureFilter filter);
};

// ** RenderingContext::ConstructionCommandBuffer::createVertexBuffer
InputLayout RenderingContext::ConstructionCommandBuffer::createInputLayout(InputLayout id, const VertexFormat& vertexFormat)
{
    OpCode opCode;
    opCode.type = OpCode::CreateInputLayout;
    opCode.createInputLayout.id = id;
    opCode.createInputLayout.format = vertexFormat;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createVertexBuffer
VertexBuffer_ RenderingContext::ConstructionCommandBuffer::createVertexBuffer(VertexBuffer_ id, const void* data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateVertexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.buffer = adoptDataBuffer(data, size);
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createIndexBuffer
IndexBuffer_ RenderingContext::ConstructionCommandBuffer::createIndexBuffer(IndexBuffer_ id, const void* data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateIndexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.buffer = adoptDataBuffer(data, size);
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createConstantBuffer
ConstantBuffer_ RenderingContext::ConstructionCommandBuffer::createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, UniformLayout layout)
{
    OpCode opCode;
    opCode.type = OpCode::CreateConstantBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.buffer = adoptDataBuffer(data, size);
    opCode.createBuffer.layout = layout;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createTexture2D
Texture_ RenderingContext::ConstructionCommandBuffer::createTexture2D(Texture_ id, const void* data, u16 width, u16 height, PixelFormat format, TextureFilter filter)
{
    OpCode opCode;
    opCode.type = OpCode::CreateTexture;
    opCode.createTexture.id = id;
    opCode.createTexture.buffer = adoptDataBuffer(data, bytesPerMipChain(format, width, height, 1));
    opCode.createTexture.width = width;
    opCode.createTexture.height = height;
    opCode.createTexture.mipLevels = 1;
    opCode.createTexture.type = TextureType2D;
    opCode.createTexture.format = format;
    opCode.createTexture.filter = filter;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createTextureCube
Texture_ RenderingContext::ConstructionCommandBuffer::createTextureCube(Texture_ id, const void* data, u16 size, u16 mipLevels, PixelFormat format, TextureFilter filter)
{
    OpCode opCode;
    opCode.type = OpCode::CreateTexture;
    opCode.createTexture.id = id;
    opCode.createTexture.buffer = adoptDataBuffer(data, bytesPerMipChain(format, size, size, mipLevels) * 6);
    opCode.createTexture.width = size;
    opCode.createTexture.height = size;
    opCode.createTexture.format = format;
    opCode.createTexture.type = TextureTypeCube;
    opCode.createTexture.mipLevels = mipLevels;
    opCode.createTexture.filter = filter;
    push( opCode );
    return id;
}
    
// -------------------------------------------------------------------------------- RenderingContext -------------------------------------------------------------------------------- //
    
// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext(RenderViewPtr view)
    : m_view(view)
    , m_shaderLibrary(*this)
{
    LogDebug("renderingContext", "rendering context size is %d bytes\n", sizeof(RenderingContext));
    LogDebug("renderingContext", "rendering state size is %d bytes\n", sizeof(State));
    LogDebug("renderingContext", "rendering state block size is %d bytes\n", sizeof(StateBlock));
    LogDebug("renderingContext", "opcode size is %d bytes\n", sizeof(CommandBuffer::OpCode));

    // Create a construction command buffer instance
    m_constructionCommandBuffer = DC_NEW ConstructionCommandBuffer;
    
    // Create an intermediate target stack
    m_transientResources = DC_NEW TransientResourceStack;
    
    // Reset input layout cache
    memset( m_inputLayoutCache, 0, sizeof( m_inputLayoutCache ) );
    
    // Resize all resource index managers
    for (s32 i = 0; i < RenderResourceType::TotalTypes; i++)
    {
        m_identifiers[i].resize(32);
        m_identifiers[i].acquire();
    }
    
    // Initialize a default state block
    m_defaultStateBlock.setCullFace(TriangleFaceBack);
    m_defaultStateBlock.setDepthState(LessEqual, true);
    m_defaultStateBlock.disableAlphaTest();
    m_defaultStateBlock.disableBlending();
    m_defaultStateBlock.setColorMask(ColorMaskAll);
    //m_defaultStateBlock.setPolygonMode(PolygonFill);
    //m_defaultStateBlock.bindProgram(0);
    //m_defaultStateBlock.bindVertexBuffer(0);
    //for (s32 i = 0; i < State::MaxTextureSamplers; i++)
    //{
    //    m_defaultStateBlock.bindTexture(0, static_cast<State::TextureSampler>(i));
    //}
    
    // Create a default program
    Program defaultProgram = requestProgram(ShaderProgramDescriptor());
    setDefaultProgram(defaultProgram);
    
    // Add shared functions
    m_shaderLibrary.addSharedFunction("f_schlick", NIMBLE_STRINGIFY(float f_schlick(float dp, float f0, float pow)
                                                                    {
                                                                        float facing = (1.0 - dp);
                                                                        return max(f0 + (1.0 - f0) * pow(facing, pow), 0.0);
                                                                    }
                                                                    ));
    
    m_shaderLibrary.addSharedFunction("f_cookTorrance", NIMBLE_STRINGIFY(float f_cookTorrance(float dp, float f0)
                                                                         {
                                                                             float n = (1.0 + sqrt(f0)) / (1.0 - sqrt(f0));
                                                                             float g = sqrt(n * n + dp * dp - 1.0);

                                                                             float part1 = (g - dp)/(g + dp);
                                                                             float part2 = ((g + dp) * dp - 1.0)/((g - dp) * dp + 1.0);

                                                                             return 0.5 * part1 * part1 * (1.0 + part2 * part2);
                                                                         }
                                                                         ));
    
    m_shaderLibrary.addSharedFunction("d_blinn", NIMBLE_STRINGIFY(vec2 d_blinn(vec4 products)
                                                                  {
                                                                      float diff = max(0.0, products.x);
                                                                      float spec = step(0.0, products.x) * max(0.0, products.z);
                                                                      return vec2(diff, spec);
                                                                  }
                                                                  ));
    
    m_shaderLibrary.addSharedFunction("pbrBlinnPhong", NIMBLE_STRINGIFY(vec2 d_blinnPhong(vec4 products, float roughness)
                                                                        {
                                                                            float a = max(0.001, roughness * roughness);
                                                                            return vec2((1.0 / ((3.1415926535897932384626433832795) * a * a)) * pow(products.z, 2.0 / (a * a) - 2.0), 0.0);
                                                                        }
                                                                        ));

    m_shaderLibrary.addSharedFunction("l_products", NIMBLE_STRINGIFY(vec4 l_products(vec3 n, vec3 v, vec3 l, vec3 r)
                                                                     {
                                                                         float ndotl = dot(n, l);
                                                                         float ndotv = dot(n, v);
                                                                         float ndotr = dot(n, r);
                                                                         return vec4(ndotl, ndotv, ndotr, 1.0);
                                                                     }
                                                                     ));
    
    m_shaderLibrary.addSharedFunction("f_reflectance", NIMBLE_STRINGIFY(float f_reflectance(float eta)
                                                                        {
                                                                            float one_minus_eta = 1.0 - eta;
                                                                            float one_plus_eta  = 1.0 + eta;
                                                                            return (one_minus_eta * one_minus_eta) / (one_plus_eta * one_plus_eta);
                                                                        }
                                                                        ));
}
    
// ** RenderingContext::~RenderingContext
RenderingContext::~RenderingContext( void )
{
    delete m_constructionCommandBuffer;
}
    
// ** RenderingContext::setDefaultStateBlock
void RenderingContext::setDefaultStateBlock(const StateBlock& value)
{
    m_defaultStateBlock = value;
}
    
// ** RenderingContext::setDefaultProgram
void RenderingContext::setDefaultProgram(Program value)
{
    m_defaultProgram = value;
}
    
// ** RenderingContext::display
void RenderingContext::display(RenderFrame& frame, bool wait)
{
    // Begin frame
    if (m_view.valid())
    {
        m_view->beginFrame();
    }
    
    // First execute a construction command buffer
    construct(frame);
    
    // Execute an entry point command buffer
    execute( frame, frame.entryPoint() );
    
    // Reset rendering states
    applyStateBlock(frame, m_defaultStateBlock);
    
    // End frame
    if (m_view.valid())
    {
        m_view->endFrame(wait);
    }
}
    
// ** RenderingContext::construct
void RenderingContext::construct(RenderFrame& frame)
{
    execute(frame, *m_constructionCommandBuffer);
    m_constructionCommandBuffer->reset();
}

// ** RenderingContext::execute
void RenderingContext::execute( const RenderFrame& frame, const CommandBuffer& commands )
{
    // Push a new frame to an intermediate target stack
    m_transientResources->pushFrame();
    
    // Execute a command buffer
    executeCommandBuffer(frame, commands);
    
    // Pop a stack frame
    m_transientResources->popFrame();
}
    
// ** RenderingContext::loadTransientResource
void RenderingContext::loadTransientResource(TransientResourceId transient, ResourceId id)
{
    m_transientResources->load(transient, id);
}

// ** RenderingContext::unloadTransientResource
void RenderingContext::unloadTransientResource(TransientResourceId transient)
{
    m_transientResources->unload(transient);
}
    
// ** RenderingContext::transientTarget
ResourceId RenderingContext::transientResource(TransientResourceId transient)
{
    return m_transientResources->get(transient);
}

// ** RenderingContext::allocateIdentifier
ResourceId RenderingContext::allocateIdentifier(RenderResourceType::Enum type)
{
    NIMBLE_ABORT_IF(!m_identifiers[type].hasFreeIndices(), "too much persistent identifiers allocated");
    return m_identifiers[type].acquire();
}

// ** RenderingContext::releaseIdentifier
void RenderingContext::releaseIdentifier(RenderResourceType::Enum type, ResourceId id)
{
    m_identifiers[type].release(id);
}

// ** RenderingContext::requestInputLayout
InputLayout RenderingContext::requestInputLayout(const VertexFormat& format)
{
    // First lookup a previously constucted input layout
    InputLayout id = m_inputLayoutCache[format];
    
    if( id )
    {
        return id;
    }
    
    // Allocate an input layout identifier
    id = allocateIdentifier(RenderResourceType::InputLayout);
    
    // Nothing found - construct a new one
    id = m_constructionCommandBuffer->createInputLayout(id, format);
    
    // Now put a new input layout to cache
    m_inputLayoutCache[format] = id;
    
    return id;
}

// ** RenderingContext::requestUniformLayout
UniformLayout RenderingContext::requestUniformLayout(const String& name, const UniformElement* elements)
{
    // Allocate next uniform layout id.
    UniformLayout id = allocateIdentifier(RenderResourceType::UniformLayout);

    // Construct a uniform instance
    m_uniformLayouts.emplace(id, UniformBufferLayout());

    for (const UniformElement* element = elements; element->name; element++)
    {
        m_uniformLayouts[id].push_back(*element);
    }

    // Now sort elements by an offset
    struct OrderBy
    {
        static bool offset(const UniformElement& first, const UniformElement& second)
        {
            return first.offset < second.offset;
        }
    };

    std::sort(m_uniformLayouts[id].begin(), m_uniformLayouts[id].end(), OrderBy::offset);
    
    // Finally add the sentinel
    UniformElement sentinel = { NULL };
    m_uniformLayouts[id].push_back(sentinel);

    // Save this layout to a name mapping
    m_uniformLayoutByName[name] = id;

    return id;
}

// ** RenderingContext::requestPipelineFeatureLayout
FeatureLayout RenderingContext::requestPipelineFeatureLayout(const PipelineFeature* features)
{
    // Create a pipeline feature layout
    PipelineFeatureLayout* layout = DC_NEW PipelineFeatureLayout;
    
    // Populate it with feature mappings
    for (; features->name; features++)
    {
        layout->addFeature(features->name.value(), features->bits);
    }
    
    // Allocate an input layout identifier
    FeatureLayout id = allocateIdentifier(RenderResourceType::FeatureLayout);
    
    // Put this layout instance to a pool
    m_pipelineFeatureLayouts.emplace(id, layout);

    return id;
}

// ** RenderingContext::requestVertexBuffer
VertexBuffer_ RenderingContext::requestVertexBuffer( const void* data, s32 size )
{
    VertexBuffer_ id = allocateIdentifier(RenderResourceType::VertexBuffer);
    return m_constructionCommandBuffer->createVertexBuffer(id, data, size);
}

// ** RenderingContext::requestIndexBuffer
IndexBuffer_ RenderingContext::requestIndexBuffer( const void* data, s32 size )
{
    IndexBuffer_ id = allocateIdentifier(RenderResourceType::IndexBuffer);
    return m_constructionCommandBuffer->createIndexBuffer(id, data, size);
}

// ** RenderingContext::requestConstantBuffer
ConstantBuffer_ RenderingContext::requestConstantBuffer(const void* data, s32 size, UniformLayout layout)
{
    ConstantBuffer_ id = allocateIdentifier(RenderResourceType::ConstantBuffer);
    return m_constructionCommandBuffer->createConstantBuffer(id, data, size, layout);
}

// ** RenderingContext::requestTexture2D
Texture_ RenderingContext::requestTexture2D(const void* data, u16 width, u16 height, PixelFormat format, TextureFilter filter)
{
    Texture_ id = allocateIdentifier(RenderResourceType::Texture);
    return m_constructionCommandBuffer->createTexture2D(id, data, width, height, format, filter);
}
    
// ** RenderingContext::requestTextureCube
Texture_ RenderingContext::requestTextureCube(const void* data, u16 size, u16 mipLevels, PixelFormat format, TextureFilter filter)
{
    Texture_ id = allocateIdentifier(RenderResourceType::Texture);
    return m_constructionCommandBuffer->createTextureCube(id, data, size, mipLevels, format, filter);
}
    
// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(const ShaderProgramDescriptor& descriptor)
{
    // Allocate a program identifier
    Program id = allocateIdentifier(RenderResourceType::Program);
    
    // Put this program to a pool
    m_programs.emplace(id, descriptor);
    
    return id;
}

// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(VertexShader vertexShader, FragmentShader fragmentShader)
{
    // Create a shader program descriptor
    ShaderProgramDescriptor descriptor;
    descriptor.vertexShader = vertexShader;
    descriptor.fragmentShader = fragmentShader;
    
    // Now request a program
    Program id = requestProgram(descriptor);
    
    return id;
}

// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(const String& vertex, const String& fragment)
{
    // Create vertex and fragment shaders
    VertexShader   vertexShader   = m_shaderLibrary.addVertexShader(vertex);
    FragmentShader fragmentShader = m_shaderLibrary.addFragmentShader(fragment);
    
    // Now request a program.
    Program id = requestProgram(vertexShader, fragmentShader);
    
    return id;
}

// ** RenderingContext::findUniformLayout
const UniformElement* RenderingContext::findUniformLayout(const String& name) const
{
    Map<String, UniformLayout>::const_iterator i = m_uniformLayoutByName.find(name);

    if (i != m_uniformLayoutByName.end())
    {
        return &m_uniformLayouts[i->second][0];
    }

    return NULL;
}

// ** RenderingContext::mergeStateBlocks
s32 RenderingContext::mergeStateBlocks(const StateBlock* const * stateBlocks, s32 blockCount, State* states, s32 maxStates, PipelineFeatures& userDefined) const
{
    PipelineFeatures userFeatures = 0;
    PipelineFeatures userFeaturesMask = ~0;
    
    // A bitmask of states that were already set
    u32 activeStateMask = 0;
    
    // A total number of states written to an output array
    s32 statesWritten = 0;
    
    for( s32 i = 0; i < blockCount; i++ )
    {
        // Get a state block at specified index
        const StateBlock* block = stateBlocks[i];
        
        // No more state blocks in a stack - break
        if( block == NULL )
        {
            break;
        }
        
        // Update feature set
        userFeatures      = userFeatures     | block->features();
        userFeaturesMask  = userFeaturesMask & block->featureMask();
        
        // Skip redundant state blocks by testing a block bitmask against an active state mask
        if( (activeStateMask ^ block->mask()) == 0 )
        {
            continue;
        }
        
        // Apply all states in a block
        for( s32 j = 0, n = block->stateCount(); j < n; j++ )
        {
            // Get a render state bit
            u32 stateBit = block->stateBit( j );
            
            // Skip redundate state blocks by testing a state bitmask agains an active state mask
            if( activeStateMask & stateBit )
            {
                continue;
            }
            
            NIMBLE_ABORT_IF(statesWritten >= maxStates, "to much render states");
            
            // Write a render state at specified index to an output array
            states[statesWritten++] = block->state(j);
            
            // Update an active state mask
            activeStateMask = activeStateMask | stateBit;
        }
    }
    
    // Compose a user defined feature mask
    userDefined = userFeatures & userFeaturesMask;
    
    return statesWritten;
}

// ** RenderingContext::startPipelineConfiguration
s32 RenderingContext::startPipelineConfiguration(const StateBlock* const * stateBlocks, s32 blockCount, State* states, s32 maxStates, PipelineFeatures& userDefined)
{
    // Merge all state blocks to a single array of rendering states
    s32 stateCount = mergeStateBlocks(stateBlocks, blockCount, states, maxStates, userDefined);
    
    // This will start recording pipeline changes
    m_pipeline.beginStateBlock();
    
    return stateCount;
}

// ** RenderingContext::finishPipelineConfiguration
PipelineFeatures RenderingContext::finishPipelineConfiguration(PipelineFeatures userDefined)
{
    // Apply a user defined feature mask
    m_pipeline.activateUserFeatures(userDefined);
    
    // Finish applying state blocks
    m_pipeline.endStateBlock();
    
    //return features;
    return m_pipeline.features();
}

// ** RenderingContext::createVertexBufferLayout
VertexBufferLayoutUPtr RenderingContext::createVertexBufferLayout(VertexFormat vertexFormat) const
{
    // Create an input layout
    VertexBufferLayoutUPtr inputLayout = DC_NEW VertexBufferLayout(vertexFormat.vertexSize());
    
    // Add vertex attributes to an input layout
    if(vertexFormat & VertexFormat::Position)
    {
        inputLayout->attributeLocation(VertexPosition, 3, vertexFormat.attributeOffset(VertexFormat::Position));
    }
    if(vertexFormat & VertexFormat::Color)
    {
        inputLayout->attributeLocation(VertexColor, 4, vertexFormat.attributeOffset(VertexFormat::Color));
    }
    if(vertexFormat & VertexFormat::Normal)
    {
        inputLayout->attributeLocation(VertexNormal, 3, vertexFormat.attributeOffset(VertexFormat::Normal));
    }
    if(vertexFormat & VertexFormat::TexCoord0)
    {
        inputLayout->attributeLocation(VertexTexCoord0, 2, vertexFormat.attributeOffset(VertexFormat::TexCoord0));
    }
    if(vertexFormat & VertexFormat::TexCoord1)
    {
        inputLayout->attributeLocation(VertexTexCoord1, 2, vertexFormat.attributeOffset(VertexFormat::TexCoord1));
    }
    
    return inputLayout;
}

// ** RenderingContext::deprecatedRequestShader
Program RenderingContext::deprecatedRequestShader(const String& fileName)
{
    static CString vertexShaderMarker   = "[VertexShader]";
    static CString fragmentShaderMarker = "[FragmentShader]";
    
    // Read the code from an input stream
    String code = Io::DiskFileSystem::readTextFile( fileName );
    NIMBLE_ABORT_IF(code.empty(), "a shader source is empty or file not found");
    
    // Extract vertex/fragment shader code blocks
    size_t vertexBegin   = code.find( vertexShaderMarker );
    size_t fragmentBegin = code.find( fragmentShaderMarker );
    
    if( vertexBegin == String::npos && fragmentBegin == String::npos )
    {
        return 0;
    }
    
    String vertexShader, fragmentShader;
    
    if( vertexBegin != String::npos )
    {
        u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
        vertexShader = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
    }
    
    if( fragmentBegin != String::npos )
    {
        u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
        fragmentShader = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
    }
    
    return requestProgram(vertexShader, fragmentShader);
}
    
} // namespace Renderer

DC_END_DREEMCHEST
