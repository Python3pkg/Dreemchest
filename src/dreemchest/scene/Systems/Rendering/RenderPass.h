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

#ifndef __DC_Scene_RenderPass_H__
#define __DC_Scene_RenderPass_H__

#include "RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Base class for all render passes.
	class RenderPassBase : public Ecs::EntitySystem {
	public:

								//! Constructs RenderPassBase instance
								RenderPassBase( Ecs::Entities& entities, const String& name, const Ecs::Aspect& aspect, const Renderers& renderers )
									: EntitySystem( entities, name, aspect ), m_renderers( renderers ) {}

		//! Renders the pass.
		void					render( u32 currentTime, f32 dt, const Matrix4& viewProjection );

	protected:

		//! Processes the entity.
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity );

	protected:

		Renderers				m_renderers;	//!< Rendering HAL.
		Matrix4					m_viewProj;		//!< The view-projection matrix.
	};

	//! Generic render pass to subclass user-defined render passes from
	template<typename TComponent>
	class RenderPass : public RenderPassBase {
	public:

								//! Constructs RenderPass instance
								RenderPass( Ecs::Entities& entities, const String& name, const Renderers& renderers )
									: RenderPassBase( entities, name, Ecs::Aspect::all<TComponent, Transform>(), renderers ) {}

	private:

		//! Extracts the components from entity.
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity );

		//! This method should be overridden in subclass
		virtual void			process( u32 currentTime, f32 dt, SceneObject& sceneObject, TComponent& component, Transform& transform );
	};

	// ** RenderPass::process
	template<typename TComponent>
	void RenderPass<TComponent>::process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity )
	{
		SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
		DC_BREAK_IF( sceneObject == NULL )

		process( currentTime, dt, *sceneObject, *entity->get<TComponent>(), *entity->get<Transform>() );
	}

	// ** RenderPass::process
	template<typename TComponent>
	void RenderPass<TComponent>::process( u32 currentTime, f32 dt, SceneObject& sceneObject, TComponent& component, Transform& transform )
	{
		DC_BREAK
	}

	//! Generic render pass to draw the 2D graphics
	template<typename TComponent>
	class RenderPass2D : public RenderPass<TComponent> {
	public:

								//! Constructs RenderPass2D instance
								RenderPass2D( Ecs::Entities& entities, const String& name, const Renderers& renderers )
									: RenderPass( entities, name, renderers ) {}

	protected:

		//! Sets the view-projection matrix for 2D rendering.
		virtual bool			begin( u32 currentTime );

		//! Flushes the generated 2D mesh.
		virtual void			end( void );
	};

	// ** RenderPass2D::begin
	template<typename TComponent>
	bool RenderPass2D<TComponent>::begin( u32 currentTime )
	{
		m_renderers.m_renderer2d->begin( m_viewProj );
		return true;
	}

	// ** RenderPass2D::end
	template<typename TComponent>
	void RenderPass2D<TComponent>::end( void )
	{
		m_renderers.m_renderer2d->end();
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderPass_H__    */