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

#include "StaticMeshEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** StaticMeshEmitter::emit
void StaticMeshEmitter::emit( const Vec3& camera, RenderingContext& ctx, Rvm& rvm, const StaticMesh& staticMesh, const Transform& transform )
{
    // Get the material
    const MaterialHandle& material = staticMesh.material( 0 );
    RenderingMode         mode     = material->renderingMode();

    // Does this material passes the filter?
    if( (BIT( mode ) & m_renderModes) == 0 ) {
        return;
    }

    // Request the renderable asset for this mesh.
    s32 renderable = ctx.requestRenderable( staticMesh.mesh() );

    // Request the technique asset for a material.
    s32 technique = ctx.requestTechnique( material );

    // Emit the rendering command
    rvm.emitDrawCall( &transform.matrix(), renderable, technique, mode, (camera - transform.position()).length() );
}

} // namespace Scene

DC_END_DREEMCHEST