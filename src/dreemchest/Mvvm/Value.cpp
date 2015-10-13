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

#include "Value.h"
#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

// ----------------------------------------------------------- Value ----------------------------------------------------------- //

// ** Value::setParent
void Value::setParent( ObjectWPtr value )
{
	m_parent = value;
}

// ** Value::type
ValueTypeIdx Value::type( void ) const
{
	return 0;
}

// ** Value::is
bool Value::is( ValueTypeIdx expected ) const
{
	ValueTypeIdx actual = type();
	return actual == expected;
}

// ** Value::notifyValueChanged
void Value::notifyValueChanged( void )
{
	if( m_parent.valid() ) {
		m_parent->check();
	}

	for( Bindings::iterator i = m_bindings.begin(), end = m_bindings.end(); i != end; ++i ) {
		i->get()->handleValueChanged();
	}
}

// ** Value::addBinding
void Value::addBinding( BindingWPtr binding )
{
	m_bindings.insert( binding );
}

// ** Value::removeBinding
void Value::removeBinding( BindingWPtr binding )
{
	m_bindings.erase( binding );
}

// ----------------------------------------------------------- Object ----------------------------------------------------------- //

// ** Object::Object
Object::Object( void )
{
	m_isValid = add<Boolean>( "isValid" );
}

// ** Object::isValid
Boolean::WPtr Object::isValid( void ) const
{
	return m_isValid;
}

// ** Object::is
bool Object::is( ValueTypeIdx expected ) const
{
	ValueTypeIdx actual = valueType<Object>();

	if( expected == actual ) {
		return true;
	}

	return Value::is( expected );
}

// ** Object::type
ValueTypeIdx Object::type( void ) const
{
	return Value::valueType<Object>();
}

// ** Object::validate
bool Object::validate( void ) const
{
	return true;
}

// ** Object::check
bool Object::check( void )
{
	bool result = validate();
	m_isValid->set( result );
	return result;
}

// ** Object::has
bool Object::has( const String& name ) const
{
	return m_properties.find( name ) != m_properties.end();
}

// ** Object::get
ValueWPtr Object::get( const String& name ) const
{
	Properties::const_iterator i = m_properties.find( name );
	return i != m_properties.end() ? i->second : ValuePtr();
}

// ** Object::resolve
ValueWPtr Object::resolve( const String& uri ) const
{
	u32 idx = uri.find( '.' );

	// No objects in URI - return the property.
	if( idx == String::npos ) {
		return get( uri );
	}

	// Split the URI.
	String data = uri.substr( 0, idx );
	String key  = uri.substr( idx + 1 );

	// Find the nested property.
	ValueWPtr value = get( data );

	// Cast the property to object.
	ObjectWPtr object = castTo<Object>( value );

	if( !object.valid() ) {
		return ValueWPtr();
	}

	return object->resolve( key );
}

// ----------------------------------------------------------- Array ----------------------------------------------------------- //

//! Returns array value type.
ValueTypeIdx Array::type( void ) const
{
	return Value::valueType<Array>();
}

// ** Array::is
bool Array::is( ValueTypeIdx expected ) const
{
	ValueTypeIdx actual = valueType<Array>();

	if( expected == actual ) {
		return true;
	}

	return Value::is( expected );
}

// ** Array::size
s32 Array::size( void ) const
{
	return ( s32 )m_values.size();
}

// ** Array::push
void Array::push( const ValuePtr& value )
{
	m_values.push_back( value );
	notifyValueChanged();
}

} // namespace mvvm

DC_END_DREEMCHEST