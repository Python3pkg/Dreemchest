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

#include "TransformSystems.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// -------------------------------------------- AffineTransformSystem -------------------------------------------- //

// ** AffineTransformSystem::update
void AffineTransformSystem::update( u32 currentTime, f32 dt )
{
	for( u32 i = 0; i < m_transforms.size(); i++ ) {
		Transform* transform = m_transforms[i];

		Matrix4 T = Matrix4::translation( transform->position() ) * transform->rotation() * Matrix4::scale( transform->scale() );

		if( transform->parent().valid() ) {
			T = transform->parent()->matrix() * T;
		}

		transform->setMatrix( T );
	}
}

// ** AffineTransformSystem::entityAdded
void AffineTransformSystem::entityAdded( const Ecs::Entity& entity )
{
	m_transforms.push_back( entity.get<Transform>() );
}

// ** AffineTransformSystem::entityRemoved
void AffineTransformSystem::entityRemoved( const Ecs::Entity& entity )
{
	DC_BREAK
}

// -------------------------------------------- WorldSpaceBoundingBoxSystem -------------------------------------------- //

// ** WorldSpaceBoundingBoxSystem::process
void WorldSpaceBoundingBoxSystem::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, StaticMesh& staticMesh, Transform& transform )
{
	if( !staticMesh.mesh().valid() ) {
		return;
	}

	staticMesh.setWorldSpaceBounds( staticMesh.mesh()->bounds() * transform.matrix() );
}

// ------------------------------------------------------- MoveAlongAxesSystem ------------------------------------------------------- //

// ** MoveAlongAxesSystem::MoveAlongAxesSystem
MoveAlongAxesSystem::MoveAlongAxesSystem( void ) : GenericEntitySystem( "MoveAlongAxesSystem" )
{

}

// ** MoveAlongAxesSystem::process
void MoveAlongAxesSystem::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, MoveAlongAxes& moveAlongAxes, Transform& transform )
{
	// Get movement values
	Vec3 movement = moveAlongAxes.delta() * moveAlongAxes.speed() * dt;

	// Get the coordinate system flags.
	u8 cs = moveAlongAxes.coordinateSystem();

	// Construct the coordinate system axes
	Vec3 axes[TotalCoordinateSystemAxes] = {
		  cs & CSLocalX ? transform.axisX() : Vec3( 1.0f, 0.0f, 0.0f )
		, cs & CSLocalY ? transform.axisY() : Vec3( 0.0f, 1.0f, 0.0f )
		, cs & CSLocalZ ? transform.axisZ() : Vec3( 0.0f, 0.0f, 1.0f )
	};

	// Calculate new position values
	Vec3 position = transform.position() + axes[AxisX] * movement.x + axes[AxisY] * movement.y + axes[AxisZ] * movement.z;

	position.x = moveAlongAxes.rangeForAxis( AxisX ).clamp( position.x );
	position.y = moveAlongAxes.rangeForAxis( AxisY ).clamp( position.y );
	position.z = moveAlongAxes.rangeForAxis( AxisZ ).clamp( position.z );

	// Set the position
	transform.setPosition( position );
}

// ------------------------------------------------------- RotateAroundAxesSystem ------------------------------------------------------- //

// ** RotateAroundAxesSystem::RotateAroundAxesSystem
RotateAroundAxesSystem::RotateAroundAxesSystem( void ) : GenericEntitySystem( "RotateAroundAxesSystem" )
{

}

// ** RotateAroundAxesSystem::process
void RotateAroundAxesSystem::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, RotateAroundAxes& rotateAroundAxes, Transform& transform )
{
	// Get rotation values
	Vec3 rotation = rotateAroundAxes.delta() * rotateAroundAxes.speed() * dt;

	// Get the coordinate system flags.
	u8 cs = rotateAroundAxes.coordinateSystem();

	// Construct the coordinate system axes
	Vec3 axes[TotalCoordinateSystemAxes] = {
		  cs & CSLocalX ? transform.axisX() : Vec3( 1.0f, 0.0f, 0.0f )
		, cs & CSLocalY ? transform.axisY() : Vec3( 0.0f, 1.0f, 0.0f )
		, cs & CSLocalZ ? transform.axisZ() : Vec3( 0.0f, 0.0f, 1.0f )
	};

	// Rotate the transform
	for( s32 i = 0; i < TotalCoordinateSystemAxes; i++ ) {
		CoordinateSystemAxis axis = static_cast<CoordinateSystemAxis>( i );

		// Get the rotation delta
		f32 delta = rotation[i];

		// No rotation around this axis - skip
		if( fabs( delta ) <= 0.001f ) {
			continue;
		}

		// Get current rotation around an axis and check overflow
		f32 rotation = rotateAroundAxes.rotationForAxis( axis );

		if( !rotateAroundAxes.rangeForAxis( axis ).contains( rotation + delta ) ) {
			continue;
		}

		rotateAroundAxes.setRotationForAxis( axis, rotation + delta );

		// Apply rotation
		const Vec3& a = axes[axis];
		transform.rotate( delta, a.x, a.y, a.z );
	}
}

// -------------------------------------------------- FollowSystem ----------------------------------------------------- //

// ** FollowSystem::process
void FollowSystem::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, Follow& follow, Transform& transform )
{
	const TransformWPtr& target = follow.target();
	DC_BREAK_IF( target == NULL )

	switch( follow.type() ) {
	case Follow::Immediate:	transform.setPosition( target->position() );
							break;

	case Follow::Smooth:	{
								Vec3 dir = target->position() - transform.position();
								transform.setPosition( transform.position() + dir * follow.damping() * dt );
							}
							break;

	case Follow::Elastic:	{
								Internal::Ptr internal = follow.internal<Internal>();

								Vec3 direction = target->position() - transform.position();

								internal->m_force += direction * follow.springForce();
								Vec3 velocity = internal->m_force;

								transform.setPosition( transform.position() + velocity * dt );

								internal->m_force *= 1.0f - (follow.damping() * dt);
							}
							break;
	}
	
}

// ** FollowSystem::entityAdded
void FollowSystem::entityAdded( const Ecs::Entity& entity )
{
	entity.get<Follow>()->setInternal<Internal>( DC_NEW Internal );
}

// ---------------------------------------------- ParticlesSystem -------------------------------------------- //

// ** ParticlesSystem::process
void ParticlesSystem::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, Particles& particles, Transform& transform )
{
	using namespace Fx;

	// Get the particles instance
	ParticleSystemInstanceWPtr instance = particles.instance();

	// Set the transform
	instance->setPosition( transform.worldSpacePosition() );

	// Update particles
	instance->update( dt );
}

} // namespace Scene

DC_END_DREEMCHEST