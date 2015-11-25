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

#include "Zones.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

// ------------------------------------------------- Zone ------------------------------------------------- //

// ** Zone::create
ZonePtr Zone::create( ZoneType type )
{
    switch( type ) {
    case ZoneDisk: return ZonePtr( DC_NEW DiskZone );
    case ZoneLine: return ZonePtr( DC_NEW LineZone );
    }

	DC_BREAK
    return ZonePtr();
}

// ------------------------------------------------- DiskZone ------------------------------------------------- //

// ** DiskZone::type
ZoneType DiskZone::type( void ) const
{
	return ZoneDisk;
}

// ** DiskZone::generateRandomPoint
Vec3 DiskZone::generateRandomPoint( f32 scalar, const Vec3& center ) const
{
	f32 inner = SampleParameter( &m_innerRadius, 0.0f );
	f32 outer = SampleParameter( &m_outerRadius, 0.0f );

    Vec2  direction = Vec2::randDirection();
    f32   distance  = RANDOM_SCALAR( inner, outer );

    return center + Vec3( direction.x, direction.y, 0.0f ) * distance;
}

// ------------------------------------------------- LineZone ------------------------------------------------- //

// ** LineZone::type
ZoneType LineZone::type( void ) const
{
	return ZoneLine;
}

// ** LineZone::generateRandomPoint
Vec3 LineZone::generateRandomPoint( f32 scalar, const Vec3& center ) const
{
	float length = SampleParameter( &m_length, 0.0f );
	float angle  = SampleParameter( &m_angle, 0.0f );

    Vec2  direction = Vec2::fromAngle( angle );
    f32   distance  = RANDOM_SCALAR( -length * 0.5f, length * 0.5f );

    return center + Vec3( direction.x, direction.y, 0.0f ) * distance;
}

} // namespace Fx

DC_END_DREEMCHEST