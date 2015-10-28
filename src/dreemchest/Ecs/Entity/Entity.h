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

#ifndef __DC_Ecs_Entity_H__
#define __DC_Ecs_Entity_H__

#include "../Component/Component.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! Entity handle contains an entity id & parent world.
	/*!
	Entity is just a unique ID that tags each game-object as a separate
	item. Entity should not contain any processing logic or object-specific
	game data. Entity's behaviour in a game world is defined by a set of
	data components.
	*/
	class Entity : public RefCounted {
	friend class Ecs;
	public:

		//! Returns an entity identifier.
		const EntityId&			id( void ) const;

		//! Returns a component mask.
		const Bitset&			mask( void ) const;

		//! Returns true if entity has a component of given type.
		template<typename TComponent>
		TComponent*				has( void ) const;

		//! Resturns an entity's component by type.
		template<typename TComponent>
		TComponent*				get( void ) const;

		//! Attaches the created component to an entity.
		template<typename TComponent>
		TComponent*				attachComponent( TComponent* component );

		//! Constructs a new component and attaches it to this entity.
	#ifndef DC_CPP11_DISABLED
		template<typename TComponent, typename ... Args>
		TComponent*				attach( Args ... args );
	#endif	/*	!DC_CPP11_DISABLED	*/

		//! Removes a component from this entity.
		template<typename TComponent>
		void					detach( void );

		//! Queues removal of this entity from the world and all systems.
		void					queueRemoval( void );

	private:

								//! Constructs Entity instance.
								Entity( EcsWPtr ecs, const EntityId& id );

		//! Marks this entity as queued for removal.
		void					markAsRemoved( void );

	private:

		//! Container type to store components.
		typedef Map<TypeIdx, ComponentBasePtr> Components;

		EcsWPtr					m_ecs;			//!< Parent ECS instance.
		EntityId				m_id;			//!< Entity identifier.
		Components				m_components;	//!< Attached components.
		Bitset					m_mask;			//!< Component mask.
		bool					m_isRemoved;	//!< Entity is queued for removal.
	};

	// ** Entity::has
	template<typename TComponent>
	TComponent* Entity::has( void ) const
	{
		DC_BREAK_IF( m_isRemoved );
		Components::const_iterator i = m_components.find( TypeIndex<TComponent>::idx() );
		return i == m_components.end() ? NULL : static_cast<TComponent*>( i->second.get() );
	}

	// ** Entity::get
	template<typename TComponent>
	TComponent* Entity::get( void ) const
	{
		DC_BREAK_IF( m_isRemoved );

		TypeIdx idx = TypeIndex<TComponent>::idx();
		Components::const_iterator i = m_components.find( idx );
		DC_BREAK_IF( i == m_components.end() )

        TComponent* result = castTo<TComponent>( i->second.get() );
		DC_BREAK_IF( result == NULL );

		return result;
	}

	// ** Entity::attachComponent
	template<typename TComponent>
	TComponent* Entity::attachComponent( TComponent* component )
	{
		DC_BREAK_IF( m_isRemoved );
		DC_BREAK_IF( has<TComponent>() );

		TypeIdx idx = component->typeIndex();

		m_components[idx] = component;
		m_mask.set( idx );
		m_ecs->notifyEntityChanged( m_id );
		return component;	
	}

	// ** Entity::attach
#ifndef DC_CPP11_DISABLED
	template<typename TComponent, typename ... Args>
	TComponent* Entity::attach( Args ... args )
	{
		TComponent* component = m_ecs->createComponent<TComponent>( args... );
		return attachComponent<TComponent>( component );
	}
#endif	/*	!DC_CPP11_DISABLED	*/

	// ** Entity::detach
	template<typename TComponent>
	void Entity::detach( void )
	{
		DC_BREAK_IF( m_isRemoved );

		Components::iterator i = m_components.find( TypeIndex<TComponent>::idx() );
		DC_BREAK_IF( i == m_components.end() )
		m_mask.clear( i->second->typeIndex() );
		m_ecs->notifyEntityChanged( m_id );
		m_components.erase( i );
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	! __DC_Ecs_Entity_H__	*/