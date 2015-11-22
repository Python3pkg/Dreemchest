//
//  Filename:   Modifiers.h
//	Created:	28:05:2011   18:32

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "Modifiers.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

// ---------------------------------------- Modifier ---------------------------------------- //

// ** Modifier::Modifier
Modifier::Modifier( void )
{
	m_duration  = 1.0f;
	m_isEnabled = false;
}

// ** Modifier::create
Modifier* Modifier::create( ModifierType type )
{
	switch( type ) {
	case ModifierType::Friction:	 return DC_NEW Friction;
	case ModifierType::Force:		 return DC_NEW Force;
	case ModifierType::ForceField:	 return DC_NEW ForceField;
	}

	DC_BREAK;

	return NULL;
}

// ** Modifier::duration
float Modifier::duration( void ) const
{
	return m_duration;
}

// ** Modifier::setDuration
void Modifier::setDuration( float value )
{
	m_duration = value;
}

// ** Modifier::isEnabled
bool Modifier::isEnabled( void ) const
{
	return m_isEnabled;
}

// ** Modifier::setEnabled
void Modifier::setEnabled( bool value )
{
	m_isEnabled = value;
}

// --------------------------------------- ForceField --------------------------------------- //

// ** ForceField::ForceField
ForceField::ForceField( void )
{
	registerParameter( "PositionX", &m_x, sParameterInfo::Lifetime );
	registerParameter( "PositionY", &m_y, sParameterInfo::Lifetime );
//	registerParameter( "PositionZ", &m_z, sParameterInfo::Lifetime );
	registerParameter( "Force", &m_force, sParameterInfo::Lifetime );
	registerParameter( "Radius", &m_radius, sParameterInfo::Lifetime );

    m_isQuadratic = true;
}

// ** ForceField::isQuadratic
bool ForceField::isQuadratic( void ) const
{
    return m_isQuadratic;
}

// ** ForceField::setQuadratic
void ForceField::setQuadratic( bool value )
{
    m_isQuadratic = value;
}

// ** ForceField::apply
void ForceField::apply( float dt, float scalar, Particles *particles )
{
	DC_NOT_IMPLEMENTED
/*
	// ** Sample parameters
	float x = SampleParameter( &m_x, 0.0f );
	float y = SampleParameter( &m_y, 0.0f );
	float r = SampleParameter( &m_radius, 0.0f );
	float f = SampleParameter( &m_force, 0.0f );

    vec2  position = vec2( x, y );
    float radius   = r * r;
    float force    = f * dt;
    
    for( int i = 0, n = bundle->GetTotalChunks(); i < n; i++ ) {
        sParticle *particles = bundle->GetParticles( i );
        int       count      = bundle->GetTotalParticles( i );

        for( int j = 0; j < count; j++ ) {
            sParticle&  particle  = particles[j];
            vec2        direction = particle.m_position - position;
            float       distance  = direction.Normalize();

            if( distance > r ) {
                continue;
            }

            float scalar = 1.0f - distance / radius;
            if( m_isQuadratic ) {
                scalar *= scalar;
            }
            
            particle.m_force.velocity += direction * scalar * force;
        }
    }
*/
}

// --------------------------------------- Friction --------------------------------------- //

// ** Friction::Friction
Friction::Friction( void )
{
    registerParameter( "Value", &m_value, sParameterInfo::Lifetime );
}

// ** Friction::apply
void Friction::apply( float dt, float scalar, Particles *particles )
{
	DC_NOT_IMPLEMENTED
/*
	float value = SampleParameter( &m_value, 0.0f );
    float ratio = 1.0f - std::min( 1.0f, value * dt );
    
    for( int i = 0, n = bundle->GetTotalChunks(); i < n; i++ ) {
        sParticle* particles = bundle->GetParticles( i );
        int        count     = bundle->GetTotalParticles( i );

        for( int j = 0; j < count; j++ ) {
            sParticle& particle  = particles[j];
            
            particle.m_linear.velocity *= ratio;
            particle.m_force.velocity  *= ratio;
        }
    }
*/
}

// --------------------------------------- Force --------------------------------------- //

// ** Force::Force
Force::Force( void )
{
    registerParameter( "ForceX", &m_x, sParameterInfo::Lifetime );
	registerParameter( "ForceY", &m_y, sParameterInfo::Lifetime );
//	registerParameter( "ForceZ", &m_z, sParameterInfo::Lifetime );
}

// ** Force::apply
void Force::apply( float dt, float scalar, Particles *particles )
{
	DC_NOT_IMPLEMENTED
/*
	float x = SampleParameter( &m_x, 0.0f );
	float y = SampleParameter( &m_y, 0.0f );

    cVector2 force( x * dt, y * dt );

    for( int i = 0, n = bundle->GetTotalChunks(); i < n; i++ ) {
        sParticle *particles = bundle->GetParticles( i );
        int       count      = bundle->GetTotalParticles( i );

        for( int j = 0; j < count; j++ ) {
            sParticle& particle  = particles[j];
            particle.m_force.velocity += force;
        }
    }
*/
}

} // namespace particles

} // namespace dreemchest