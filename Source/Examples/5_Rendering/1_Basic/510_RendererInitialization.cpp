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

// Open a platform namespace to use shorter types.
using namespace Platform;

// Open a renderer namespace.
using namespace Renderer;

// Application delegate is used to handle an events raised by application instance.
class RendererInitialization : public ApplicationDelegate
{
    // This method will be called once an application is launched.
    virtual void handleLaunched(Application* application)
    {
        // Set the default log handler.
        Logger::setStandardLogger();

        // Create a 800x600 window like we did in previous example.
        // This window will contain a rendering viewport.
        Window* window = Window::create(800, 600);

        // Create a rendering view.
        m_view = createOpenGLView(window->handle(), TextureD24 | TextureS8);
        
        // Now create the main renderer interface called HAL (hardware abstraction layer).
    //    m_hal = Hal::create(OpenGL, view);

        // Finally subscribe to updates events.
        window->subscribe<Window::Update>(dcThisMethod(RendererInitialization::handleWindowUpdate));
    }

    // Called each frame and renders a single frame
    virtual void handleWindowUpdate(const Window::Update& e)
    {
        // First clear a viewport with a color
     //   m_hal->clear(Rgba(0.3f, 0.3f, 0.3f));

        // And now just present all rendered data to the screen
    //    m_hal->present();
    }

    RenderViewPtr m_view;
    //HalPtr m_hal;   //!< Rendering HAL.
};

// Now declare an application entry point with RendererInitialization application delegate.
dcDeclareApplication(new RendererInitialization)
