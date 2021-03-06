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

// Include the engine header file.
#include <Dreemchest.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Declare the log tag in global namespace
DREEMCHEST_LOGGER_TAG( Simple )

// Application delegate is used to handle an events raised by application instance.
class ParticleSystems : public Platform::ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Platform::Application* application ) {
        // Set the default log handler.
        Logger::setStandardLogger();

        // Create a 800x600 window like we did in previous example.
        // This window will contain a rendering viewport.
        Platform::Window* window = Platform::Window::create( 800, 600 );

        // Create a rendering view.
        Renderer::RenderView* view = Renderer::Hal::createOpenGLView( window->handle() );

        // Now create the main renderer interface called HAL (hardware abstraction layer).
        m_hal = Renderer::Hal::create( Renderer::OpenGL, view );
        //m_renderingContext = Scene::RenderingContext::create( m_hal );

        // Create the particle system
        m_scene = Scene::Scene::create();

        // Create an empty asset bundle
        //m_assets = AssetBundle::create( "assets" );

        //m_assets->addMesh( "assets/tomb05_c", "tomb05_c" );

        //TerrainPtr terrain = m_assets->addTerrain( "terrain", "terrain", 128 );

        //for( u32 i = 0; i < terrain->chunkCount(); i++ ) {
        //    for( u32 j = 0; j < terrain->chunkCount(); j++ ) {
        //        SceneObjectPtr chunk = m_scene->createSceneObject();
        //        MeshPtr           mesh  = terrain->createChunkMesh( m_hal, j, i );

        //        chunk->attach<StaticMesh>( mesh );
        //        chunk->attach<::Scene::Transform>( j * Terrain::kChunkSize, -4, i * Terrain::kChunkSize, ::Scene::Transform::WPtr() );
        //    }
        //}

        //m_scene->addSystem<AssetSystem>( m_assets );

        Scene::Vec3BindingPtr wasdDirection = DC_NEW Scene::Vec3FromKeyboard( Platform::Key::A, Platform::Key::D, Platform::Key::W, Platform::Key::S );

        Scene::SceneObjectPtr camera = m_scene->createSceneObject();
        camera->attach<Scene::Camera>( Scene::Projection::Perspective, Scene::WindowTarget::create( window ), Rgb::fromHashString( "#484848" ) );
        camera->attach<Scene::RenderParticles>();
    //    camera->attach<Scene::RenderForwardLit>();
        camera->attach<Scene::RenderWireframe>();
        camera->attach<Scene::Transform>();
        camera->attach<Scene::MoveAlongAxes>( 60.0f, true, wasdDirection );
    //    camera->attach<Scene::RenderBoundingVolumes>();

        Renderer::Renderer2DPtr renderer = Renderer::Renderer2D::create( m_hal, 4096 );

        // Finally subscribe to updates events.
        window->subscribe<Platform::Window::Update>( dcThisMethod( ParticleSystems::handleUpdate ) );
    }

    // Called each frame and renders a single frame
    virtual void handleUpdate( const Platform::Window::Update& e ) {
        Rgb clearColor = Rgb::fromHashString( "#314D79" );

        // First clear a viewport with a color
        m_hal->clear( Rgba( clearColor.r, clearColor.g, clearColor.b ) );

        Threads::Thread::sleep( 30 );

        m_scene->update( 0, 0.03f );
    //    m_scene->render( m_renderingContext );
        NIMBLE_NOT_IMPLEMENTED;

        // And now just present all rendered data to the screen
        m_hal->present();
    }

    Renderer::HalPtr    m_hal;

    Scene::ScenePtr            m_scene;
//    AssetBundlePtr        m_assets;
//    AssetBundlePtr        m_meshes;

    //! Scene rendering context.
    Scene::RenderingContextPtr    m_renderingContext;
};

// Now declare an application entry point with Particles application delegate.
dcDeclareApplication( new ParticleSystems )
