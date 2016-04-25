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

#include "RenderCache.h"
#include "Rvm/RenderingContext.h"
#include "../Assets/Mesh.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderCache::RenderCache
RenderCache::RenderCache( Assets::AssetsWPtr assets, RenderingContextWPtr context )
    : m_assets( assets )
    , m_context( context )
{
}

// ** RenderCache::create
RenderCachePtr RenderCache::create( Assets::AssetsWPtr assets, RenderingContextWPtr context )
{
    return DC_NEW RenderCache( assets, context );
}

// ** RenderCache::findInputLayout
RenderResource RenderCache::findInputLayout( const VertexFormat& format )
{
    InputLayouts::iterator i = m_inputLayouts.find( format );

    if( i != m_inputLayouts.end() ) {
        return i->second;
    }

    RenderResource id = m_context->requestInputLayout( format );
    m_inputLayouts[format] = id;
    return id;
}

// ** RenderCache::findVertexBuffer
RenderResource RenderCache::findVertexBuffer( const MeshHandle& mesh )
{
    RenderResources::iterator i = m_vertexBuffers.find( mesh.asset().uniqueId() );

    if( i != m_vertexBuffers.end() ) {
        return i->second;
    }

    DC_BREAK_IF( mesh->chunkCount() == 0, "could not cache an empty mesh" );

    const Mesh::VertexBuffer& vertices = mesh->vertexBuffer( 0 );
    VertexFormat vertexFormat( VertexFormat::Normal | VertexFormat::Uv0 | VertexFormat::Uv1 );

    RenderResource id = m_context->requestVertexBuffer( &vertices[0], vertices.size() * vertexFormat.vertexSize() );
    m_vertexBuffers[mesh.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "vertex buffer with %d vertices created\n", vertices.size() );

    return id;
}

// ** RenderCache::findIndexBuffer
RenderResource RenderCache::findIndexBuffer( const MeshHandle& mesh )
{
    RenderResources::iterator i = m_indexBuffers.find( mesh.asset().uniqueId() );

    if( i != m_indexBuffers.end() ) {
        return i->second;
    }

    DC_BREAK_IF( mesh->chunkCount() == 0, "could not cache an empty mesh" );

    const Mesh::IndexBuffer& indices = mesh->indexBuffer( 0 );

    RenderResource id = m_context->requestIndexBuffer( &indices[0], indices.size() * sizeof( u16 ) );
    m_indexBuffers[mesh.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "index buffer with %d indices created\n", indices.size() );

    return id;
}

// ** RenderCache::findConstantBuffer
RenderResource RenderCache::findConstantBuffer( const MaterialHandle& material )
{
    RenderResources::iterator i = m_materialConstantBuffers.find( material.asset().uniqueId() );

    if( i != m_materialConstantBuffers.end() ) {
        return i->second;
    }


    RenderResource id = m_context->requestConstantBuffer( NULL, sizeof RenderScene::CBuffer::Material, RenderScene::CBuffer::Material::Layout );
    m_materialConstantBuffers[material.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "material constant buffer created for '%s'\n", material.asset().name().c_str() );

    return id;
}

} // namespace Scene

DC_END_DREEMCHEST