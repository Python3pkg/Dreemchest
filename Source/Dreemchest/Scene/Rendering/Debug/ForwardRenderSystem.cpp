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

#include "ForwardRenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** ForwardRenderSystem::ForwardRenderSystem
ForwardRenderSystem::ForwardRenderSystem( RenderingContext& context, RenderScene& renderScene )
    : RenderSystem( context, renderScene )
    , m_debugCascadedShadows( context, renderScene )
    , m_debugRenderTarget( context, renderScene )
{
    m_phongShader       = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Phong.shader" );
    m_ambientShader     = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Ambient.shader" );
    m_shadowShader      = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Shadow.shader" );
    m_shadowCBuffer     = m_context.requestConstantBuffer( NULL, sizeof RenderScene::CBuffer::Shadow, RenderScene::CBuffer::Shadow::Layout );
    m_clipPlanesCBuffer = m_context.requestConstantBuffer( NULL, sizeof RenderScene::CBuffer::ClipPlanes, RenderScene::CBuffer::ClipPlanes::Layout );
}

// ** ForwardRenderSystem::emitRenderOperations
void ForwardRenderSystem::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const ForwardRenderer& forwardRenderer )
{
    // Ambient pass
    {
        StateScope pass = stateStack.newScope();
        pass->bindProgram( m_context.internShader( m_ambientShader ) );
        pass->enableFeatures( ShaderEmissionColor | ShaderAmbientColor );

        RenderPassBase::emitStaticMeshes( m_renderScene.staticMeshes(), frame, commands, stateStack );
        RenderPassBase::emitPointClouds( m_renderScene.pointClouds(), frame, commands, stateStack );
    }

    // Get the shadowmapping settings
    s32 shadowSize   = forwardRenderer.shadowSize();
    s32 cascadeCount = forwardRenderer.shadowCascadeCount();
    f32 lambda       = forwardRenderer.shadowCascadeLambda();

    // Get all light sources
    const RenderScene::Lights& lights = m_renderScene.lights();

    // Render a scene for each light in scene
    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        // Get a light by index
        const RenderScene::LightNode& light = lights[i];

        if( light.light->type() == LightType::Directional ) {
            ShadowParameters parameters; parameters.invSize = 1.0f / shadowSize;
            CascadedShadowMaps csm( transform.matrix(), *light.matrix, shadowSize );
            csm.calculate( camera.fov(), camera.near(), camera.far(), entity.get<Viewport>()->aspect(), lambda, cascadeCount );

            for( s32 j = 0; j < cascadeCount; j++ ) {
                const CascadedShadowMaps::Cascade& cascade = csm.cascadeAt( j );

                parameters.transform = cascade.transform;
                u8 shadows = renderShadows( frame, commands, stateStack, light, shadowSize, parameters );

                StateScope clip = stateStack.newScope();
                m_clipPlanesParameters.equation[0] = Plane::calculate( -transform.axisZ(), transform.worldSpacePosition() - transform.axisZ() * cascade.near );
                m_clipPlanesParameters.equation[1] = Plane::calculate(  transform.axisZ(), transform.worldSpacePosition() - transform.axisZ() * cascade.far );
                clip->bindConstantBuffer( m_clipPlanesCBuffer, RenderState::ClippingPlanes );
                commands.uploadConstantBuffer( m_clipPlanesCBuffer, frame.internBuffer( &m_clipPlanesParameters, sizeof m_clipPlanesParameters ), sizeof m_clipPlanesParameters );

                renderLight( frame, commands, stateStack, light, shadows );

                if( forwardRenderer.isDebugCascadeShadows() ) {
                    m_debugRenderTarget.render( frame, commands, stateStack, *entity.get<Viewport>(), shadows, Renderer::RenderTarget::Depth, 128, j * 130, 0 );
                }

                commands.releaseRenderTarget( shadows );
            }

            if( forwardRenderer.isDebugCascadeShadows() ) {
                m_debugCascadedShadows.render( frame, commands, stateStack, csm );
            }
            continue;
        }

        // A shadow map render target handle
        u8 shadows = 0;

        if( light.light->castsShadows() ) {
            ShadowParameters parameters = spotLightShadows( light, shadowSize );
            shadows = renderShadows( frame, commands, stateStack, light, shadowSize, parameters );
        }

        // Render a light pass
        renderLight( frame, commands, stateStack, light, shadows );

        // Release an intermediate shadow render target
        if( shadows ) {
            commands.releaseRenderTarget( shadows );
        }
    }
}

// ** ForwardRenderSystem::renderShadows
u8 ForwardRenderSystem::renderShadows( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, s32 dimensions, const ShadowParameters& parameters )
{
    DC_BREAK_IF( !light.light->castsShadows(), "a light instance does not cast shadows" );

    // Acquire a shadow render target
    u8 renderTarget = commands.acquireRenderTarget( dimensions, dimensions, Renderer::PixelD24X8 );

    // Render scene from a light's point of view
    RenderCommandBuffer& cmd = commands.renderToTarget( renderTarget );
    cmd.clear( Rgba( 1.0f, 1.0f, 1.0f, 1.0f ), ~0 );

    // Update a shadow constant buffer
    cmd.uploadConstantBuffer( m_shadowCBuffer, frame.internBuffer( &parameters, sizeof parameters ), sizeof parameters );

    // Push a shadow pass scope
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( m_shadowCBuffer, RenderState::ShadowConstants );
    state->bindProgram( m_context.internShader( m_shadowShader ) );
    state->setCullFace( Renderer::TriangleFaceFront );

    // Render all static meshes to a target
    RenderPassBase::emitStaticMeshes( m_renderScene.staticMeshes(), frame, cmd, stateStack );

    return renderTarget;
}

// ** ForwardRenderSystem::renderLight
void ForwardRenderSystem::renderLight( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, u8 shadows )
{
    // A light type feature bits
    Ubershader::Bitmask lightType[] = { ShaderPointLight, ShaderSpotLight, ShaderDirectionalLight };

    // Light state block
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( light.constantBuffer, RenderState::LightConstants );
    state->enableFeatures( lightType[light.light->type()] | ShaderShadowFiltering3 );
    state->bindProgram( m_context.internShader( m_phongShader ) );
    state->setBlend( Renderer::BlendOne, Renderer::BlendOne );
    state->setDepthState( Renderer::LessEqual, false );
    state->setPolygonOffset( -1, -1 );

    // Bind a rendered shadowmap
    if( shadows ) {
        state->bindRenderedTexture( shadows, RenderState::Texture1, Renderer::RenderTarget::Depth );
        state->bindConstantBuffer( m_shadowCBuffer, RenderState::ShadowConstants );
    }

    // Emit render operations
    RenderPassBase::emitStaticMeshes( m_renderScene.staticMeshes(), frame, commands, stateStack, RenderMaskPhong );
    RenderPassBase::emitPointClouds( m_renderScene.pointClouds(), frame, commands, stateStack, RenderMaskPhong );
}

// ** ForwardRenderSystem::spotLightShadows
ForwardRenderSystem::ShadowParameters ForwardRenderSystem::spotLightShadows( const RenderScene::LightNode& light, s32 dimensions ) const
{
    ShadowParameters parameters;
    parameters.transform = Matrix4::perspective( light.light->cutoff() * 2.0f, 1.0f, 0.1f, light.light->range() * 2.0f ) * light.matrix->inversed();
    parameters.invSize   = 1.0f / dimensions;
    return parameters;
}

} // namespace Scene

DC_END_DREEMCHEST