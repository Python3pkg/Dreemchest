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

#include "Physics.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------- Shape2D ------------------------------------------- //

// ** Shape2D::clear
void Shape2D::clear( void )
{
	m_parts.clear();
}

// ** Shape2D::partCount
u32 Shape2D::partCount( void ) const
{
	return ( u32 )m_parts.size();
}

// ** Shape2D::part
const Shape2D::Part& Shape2D::part( u32 index ) const
{
	DC_ABORT_IF( index >= partCount(), "index is out of range" );
	return m_parts[index];
}

// ** Shape2D::addPart
void Shape2D::addPart( const Part& part )
{
    m_parts.push_back( part );
}

// ** Shape2D::addCircle
void Shape2D::addCircle( f32 radius, f32 x, f32 y, const Material& material )
{
	Part part;
	part.type = Circle;
	part.material = material;
	part.circle.radius = radius;
	part.circle.x = x;
	part.circle.y = y;
    addPart( part );
}

// ** Shape2D::addRect
void Shape2D::addRect( f32 width, f32 height, f32 x, f32 y, const Material& material )
{
	Part part;
	part.type = Rect;
	part.material = material;
	part.rect.width = width;
	part.rect.height = height;
	part.rect.x = x;
	part.rect.y = y;
	addPart( part );
}

// ** Shape2D::addPolygon
void Shape2D::addPolygon( const Vec2* vertices, u32 count, const Material& material )
{
	DC_BREAK_IF( count > Part::MaxVertices, "too much vertices" );

	Part part;
	part.type = Polygon;
	part.material = material;
	part.polygon.count = count;

	for( u32 i = 0; i < count; i++ ) {
		part.polygon.vertices[i * 2 + 0] = vertices[i].x;
		part.polygon.vertices[i * 2 + 1] = vertices[i].y;
	}

	addPart( part );
}

// ** Shape2D::serialize
void Shape2D::serialize( Ecs::SerializationContext& ctx, Archive& ar ) const
{
    KeyValue material = KvBuilder()
							<< "density" << m_parts[0].material.density
							<< "friction" << m_parts[0].material.friction
							<< "restitution" << m_parts[0].material.restitution;

    switch( m_parts[0].type ) {
    case Polygon:   {
                        VariantArray vertices;

                        for( u32 i = 0; i < m_parts[0].polygon.count; i++ ) {
                            vertices << m_parts[0].polygon.vertices[i * 2 + 0] << m_parts[0].polygon.vertices[i * 2 + 1];
                        }

                        ar = KvBuilder() << "type" << "polygon" << "vertices" << vertices << "material" << material;
                    }
                    break;
    case Circle:    {
                        ar = KvBuilder() << "type" << "circle" << "radius" << m_parts[0].circle.radius << "material" << material;
                    }
                    break;
    default:        DC_BREAK;
    }
}

// ** Shape2D::deserialize
void Shape2D::deserialize( Ecs::SerializationContext& ctx, const Archive& ar )
{
	// Get the object from archive
	KeyValue object = ar.as<KeyValue>();

    // Get shape type
    const String& type = object.get<String>( "type", "" );

    // Get material
    KeyValue material = object.get<KeyValue>( "material" );

    // Parse material
    Material shapeMaterial;
    shapeMaterial.density       = material.get( "density", 1.0f );
    shapeMaterial.friction      = material.get( "friction", 0.2f );
    shapeMaterial.restitution   = material.get( "restitution", 0.0f );

    if( type == "polygon" ) {
        VariantArray::Container vertices = object.get<VariantArray>( "vertices" );
        Array<Vec2>     points;

        for( s32 i = 0, n = vertices.size() / 2; i < n; i++ ) {
            points.push_back( Vec2( vertices[i * 2 + 0].as<f32>(), vertices[i * 2 + 1].as<f32>() ) );
        }

        addPolygon( &points[0], points.size(), shapeMaterial );
    }
    else if( type == "circle" ) {
        addCircle( object.get( "radius", 0.0f ), 0.0f, 0.0f, shapeMaterial );
    }
    else {
        DC_NOT_IMPLEMENTED;
    }
}

// ----------------------------------------- RigidBody2D ----------------------------------------- //

// ** RigidBody2D::RigidBody2D
RigidBody2D::RigidBody2D( f32 mass, Type type, u16 category, u16 collisionMask, bool isBullet, bool isSensor )
	: m_mass( mass ), m_type( type ), m_linearDamping( 0.0f ), m_angularDamping( 0.0f ), m_torque( 0.0f ), m_category( category ), m_collisionMask( collisionMask ), m_gravityScale( 1.0f ), m_rotatedTo( 0.0f )
{
    m_flags.set( IsBullet, isBullet );
    m_flags.set( IsSensor, isSensor );
}

// ** RigidBody2D::mass
f32 RigidBody2D::mass( void ) const
{
	return m_mass;
}

// ** RigidBody2D::type
RigidBody2D::Type RigidBody2D::type( void ) const
{
	return m_type;
}

// ** RigidBody2D::linearDamping
f32 RigidBody2D::linearDamping( void ) const
{
	return m_linearDamping;
}

// ** RigidBody2D::setLinearDamping
void RigidBody2D::setLinearDamping( f32 value )
{
	m_linearDamping = value;
}

// ** RigidBody2D::angularDamping
f32 RigidBody2D::angularDamping( void ) const
{
	return m_angularDamping;
}

// ** RigidBody2D::setAngularDamping
void RigidBody2D::setAngularDamping( f32 value )
{
	m_angularDamping = value;
}

// ** RigidBody2D::gravityScale
f32 RigidBody2D::gravityScale( void ) const
{
    return m_gravityScale;
}

// ** RigidBody2D::setGravityScale
void RigidBody2D::setGravityScale( f32 value )
{
    m_gravityScale = value;
}

// ** RigidBody2D::moveTo
void RigidBody2D::moveTo( const Vec2& position )
{
    m_movedTo  = position;
    m_flags.on( WasMoved );
}

// ** RigidBody2D::movedTo
const Vec2& RigidBody2D::movedTo( void ) const
{
    return m_movedTo;
}

// ** RigidBody2D::wasMoved
bool RigidBody2D::wasMoved( void ) const
{
    return m_flags.is( WasMoved );
}

// ** RigidBody2D::rotateTo
void RigidBody2D::rotateTo( f32 angle )
{
    m_rotatedTo = angle;
    m_flags.on( WasRotated );
}

// ** RigidBody2D::rotatedTo
f32 RigidBody2D::rotatedTo( void ) const
{
    return m_rotatedTo;
}

// ** RigidBody2D::wasRotated
bool RigidBody2D::wasRotated( void ) const
{
    return m_flags.is( WasRotated );
}

// ** RigidBody2D::putToRest
void RigidBody2D::putToRest( void )
{
    m_flags.on( WasPutToRest );
}

// ** RigidBody2D::wasPutToRest
bool RigidBody2D::wasPutToRest( void ) const
{
    return m_flags.is( WasPutToRest );
}

// ** RigidBody2D::isBullet
bool RigidBody2D::isBullet( void ) const
{
    return m_flags.is( IsBullet );
}

// ** RigidBody2D::isSensor
bool RigidBody2D::isSensor( void ) const
{
    return m_flags.is( IsSensor );
}

// ** RigidBody2D::torque
f32 RigidBody2D::torque( void ) const
{
	return m_torque;
}

// ** RigidBody2D::applyTorque
void RigidBody2D::applyTorque( f32 value )
{
	m_torque += value;
}

// ** RigidBody2D::force
const Vec2& RigidBody2D::force( void ) const
{
	return m_force;
}

// ** RigidBody2D::applyForc
void RigidBody2D::applyForce( const Vec2& value )
{
	m_force += value;
}

// ** RigidBody2D::category
u16 RigidBody2D::category( void ) const
{
    return m_category;
}

// ** RigidBody2D::collisionMask
u16 RigidBody2D::collisionMask( void ) const
{
    return m_collisionMask;
}

// ** RigidBody2D::applyForceToPoint
void RigidBody2D::applyForceToPoint( const Vec2& value, const Vec2& point )
{
	m_forces.push_back( AppliedForce( value, point ) );
}

// ** RigidBody2D::applyImpulse
void RigidBody2D::applyImpulse( const Vec2& value )
{
    m_impulses.push_back( AppliedForce( value, Vec2( 0.0f, 0.0f ) ) );
}

// ** RigidBody2D::applyImpulseToPoint
void RigidBody2D::applyImpulseToPoint( const Vec2& value, const Vec2& point )
{
    m_impulses.push_back( AppliedForce( value, point ) );
}

// ** RigidBody2D::appliedForceCount
u32 RigidBody2D::appliedForceCount( void ) const
{
	return ( u32 )m_forces.size();
}

// ** RigidBody2D::appliedImpulse
const RigidBody2D::AppliedForce& RigidBody2D::appliedImpulse( u32 index ) const
{
	DC_ABORT_IF( index >= appliedImpulseCount(), "index is out of range" );
	return m_impulses[index];
}

// ** RigidBody2D::appliedImpulseCount
u32 RigidBody2D::appliedImpulseCount( void ) const
{
	return ( u32 )m_impulses.size();
}

// ** RigidBody2D::appliedForce
const RigidBody2D::AppliedForce& RigidBody2D::appliedForce( u32 index ) const
{
	DC_ABORT_IF( index >= appliedForceCount(), "index is out of range" );
	return m_forces[index];
}

// ** RigidBody2D::collisionEventCount
u32 RigidBody2D::collisionEventCount( void ) const
{
    return ( u32 )m_collisionEvents.size();
}

// ** RigidBody2D::collisionEvent
const RigidBody2D::CollisionEvent& RigidBody2D::collisionEvent( u32 index ) const
{
    DC_ABORT_IF( index < 0 || index >= collisionEventCount(), "index is out of range" );
    return m_collisionEvents[index];
}

// ** RigidBody2D::queueCollisionEvent
void RigidBody2D::queueCollisionEvent( const CollisionEvent& e )
{
    m_collisionEvents.push_back( e );
}

// ** RigidBody2D::clear
void RigidBody2D::clear( void )
{
	m_torque = 0.0f;
	m_force  = Vec2( 0.0f, 0.0f );
    m_flags.off( WasPutToRest | WasMoved | WasRotated );
	m_forces.clear();
    m_impulses.clear();
}

// ** RigidBody2D::clearEvents
void RigidBody2D::clearEvents( void )
{
    m_collisionEvents.clear();
}

// ** RigidBody2D::linearVelocity
const Vec2& RigidBody2D::linearVelocity( void ) const
{
    return m_linearVelocity;
}

// ** RigidBody2D::setLinearVelocity
void RigidBody2D::setLinearVelocity( const Vec2& value )
{
    m_linearVelocity = value;
}

// ** RigidBody2D::updateMass
void RigidBody2D::updateMass( f32 value )
{
    m_mass = value;
}

} // namespace Scene

DC_END_DREEMCHEST