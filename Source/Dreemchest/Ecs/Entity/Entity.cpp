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

#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** Entity::Entity
Entity::Entity( void ) : m_flags( 0 )
{

}

// ** Entity::setEcs
void Entity::setEcs( EcsWPtr value )
{
	m_ecs = value;
}

// ** Entity::ecs
EcsWPtr Entity::ecs( void ) const
{
	return m_ecs;
}

// ** Entity::setId
void Entity::setId( const EntityId& value )
{
	m_id = value;
}

// ** Entity::id
const EntityId& Entity::id( void ) const
{
	return m_id;
}

// ** Entity::clear
void Entity::clear( void )
{
    m_components.clear();
}

// ** Entity::isSerializable
bool Entity::isSerializable( void ) const
{
    return m_flags.is( Serializable );
}

// ** Entity::setSerializable
void Entity::setSerializable( bool value )
{
    m_flags.set( Serializable, value );
}

// ** Entity::mask
const Bitset& Entity::mask( void ) const
{
	return m_mask;
}

// ** Entity::components
const Entity::Components& Entity::components( void ) const
{
	return m_components;
}

// ** Entity::components
Entity::Components& Entity::components( void )
{
	return m_components;
}

// ** Entity::queueRemoval
void Entity::queueRemoval( void )
{
	m_ecs->removeEntity( m_id );
}

// ** Entity::markAsRemoved
void Entity::markAsRemoved( void )
{
	m_flags.on( Removed );
}

// ** Entity::flags
u8 Entity::flags( void ) const
{
	return m_flags;
}

// ** Entity::setFlags
void Entity::setFlags( u8 value )
{
	m_flags = value;
}

// ** Entity::updateComponentBit
void Entity::updateComponentBit( u32 bit, bool value )
{
	if( value ) {
		m_mask.set( bit );
	} else {
		m_mask.clear( bit );
	}
	
	if( m_ecs.valid() ) {
		m_ecs->notifyEntityChanged( m_id );
	}
}

// ** Entity::detachById
void Entity::detachById( TypeIdx id )
{
	DC_BREAK_IF( m_flags.is( Removed ), "this entity was removed" );

	Components::iterator i = m_components.find( id );
	DC_ABORT_IF( i == m_components.end(), "component does not exist" );

	updateComponentBit( i->second->typeIndex(), false );
    i->second->setParentEntity( NULL );
	m_components.erase( i );
}

#if DC_ECS_ENTITY_CLONING

// ** Entity::deepCopy
EntityPtr Entity::deepCopy( const EntityId& id ) const
{
    // Create entity instance
    Ecs*      ecs    = const_cast<Ecs*>( m_ecs.get() );
    EntityPtr entity = id.isNull() ? ecs->createEntity() : ecs->createEntity( id );

    // Now clone components
    for( Components::const_iterator i = m_components.begin(), end = m_components.end(); i != end; ++i ) {
        entity->attachComponent( i->second->deepCopy().get() );
    }

    return entity;
}

#endif  /*  DC_ECS_ENTITY_CLONING   */

#ifndef DC_ECS_NO_SERIALIZATION

// ** Entity::read
void Entity::read( const Io::Storage* storage )
{
	Archive ar;

	Variant value;
    Io::BinaryVariantStream( storage->isBinaryStorage()->stream() ).read( value );
	ar = value;

    SerializationContext ctx( ecs() );
    deserialize( ctx, ar );
}

// ** Entity::write
void Entity::write( Io::Storage* storage ) const
{
    SerializationContext ctx( ecs() );
    Archive ar;

    serialize( ctx, ar );
    Io::BinaryVariantStream( storage->isBinaryStorage()->stream() ).write( ar );
}

// ** Entity::serialize
void Entity::serialize( SerializationContext& ctx, Archive& ar ) const
{
    KeyValue object = KvBuilder() << "Type" << typeName() << "_id" << id() << "flags" << static_cast<u8>( m_flags );

	const Components& items = components();

	for( Components::const_iterator i = items.begin(), end = items.end(); i != end; ++i ) {
		CString key = i->second->typeName();
		Archive component;

        i->second->serialize( ctx, component );

		if( !component.isValid() ) {
			continue;
		}

		object.setValueAtKey( key, component );
	}

	ar = Archive::fromValue( object );
}

// ** Entity::deserialize
void Entity::deserialize( SerializationContext& ctx, const Archive& ar )
{
	Components& items = components();

    // Set flags
	KeyValue object = ar.as<KeyValue>();
    m_flags = object.get<u8>( "flags", 0 );

	// Load all attached components
	for( Components::iterator i = items.begin(), end = items.end(); i != end; ++i ) {
		CString key = i->second->typeName();

        const Archive& kv = object.valueAtKey( key );
        if( !kv.isValid() ) {
            LogDebug( "deserialize", "no data for component '%s'\n", key );
            continue;
        }

        i->second->deserialize( ctx, kv );
	}
	
	// Create components from key-value archive
	const KeyValue::Properties& kv = object.properties();

	for( KeyValue::Properties::const_iterator i = kv.begin(); i != kv.end(); ++i ) {
		if( i->first == "Type" || i->first == "_id" || i->first == "flags" ) {
			continue;
		}

		bool hasComponent = false;

		for( Components::iterator j = items.begin(); j != items.end(); ++j ) {
			if( j->second->typeName() == i->first ) {
				hasComponent = true;
				break;
			}
		}

		if( hasComponent ) {
			continue;
		}

        if( !i->second.isValid() ) {
			continue;
		}

		ComponentPtr component = ctx.createComponent( i->first );

        if( !component.valid() ) {
            continue;
        }

        component->deserialize( ctx, i->second );
		attachComponent( component.get() );
	}
}

#endif  /*  !DC_ECS_NO_SERIALIZATION    */

} // namespace Ecs

DC_END_DREEMCHEST