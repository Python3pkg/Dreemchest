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

#ifndef __DC_Scene_RenderingSystem_H__
#define __DC_Scene_RenderingSystem_H__

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

#include "RenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Base class for all rendering systems.
	class RenderingSystemBase : public RefCounted {
	public:

								//! Constructs the RenderingSystem instance.
								RenderingSystemBase( Ecs::Entities& entities, const String& name, const Ecs::Aspect& aspect );

		//! Adds a new render pass to this system.
		template<typename TPass>
		void					addPass( void );

		//! Renders all active cameras to their viewports.
		void					render( RenderingContextPtr context );

	private:

		//! Renders the scene from a camera.
		virtual void			renderFromCamera( RenderingContextPtr context, Camera& camera, Transform& transform );

	private:

		//! Container type to store nested render passes.
		typedef Array<RenderPassBasePtr> RenderPasses;

		Ecs::Entities&			m_entities;	//!< Entity system.
		String					m_name;		//!< Rendering system name.
		Ecs::FamilyPtr			m_cameras;	//!< All active cameras.
		RenderPasses			m_passes;	//!< All render passes to be performed by this rendering system.
	};

	// ** RenderingSystemBase::addPass
	template<typename TPass>
	void RenderingSystemBase::addPass( void )
	{
		m_passes.push_back( DC_NEW TPass( m_entities ) );
	}

	//! Generic class for single pass rendering systems.
	template<typename TPass>
	class SinglePassRenderingSystem : public RenderingSystemBase {
	public:
								SinglePassRenderingSystem( Ecs::Entities& entities )
									: RenderingSystemBase( entities, "SinglePassRenderingSystem", Ecs::Aspect::all<Camera, Transform>() ) { addPass<TPass>(); }
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingSystem_H__    */