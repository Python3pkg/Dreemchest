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

#ifndef		__DC_Network_PosixTCPSocket_H__
#define		__DC_Network_PosixTCPSocket_H__

#include    "../TCPSocket.h"
#include	"../TCPStream.h"
#include	"PosixNetwork.h"

DC_BEGIN_DREEMCHEST

namespace net {

    //! Berkley socket implementation.
	class PosixTCPSocket : public impl::TCPSocketPrivate {
    public:

										PosixTCPSocket( TCPSocketDelegate* delegate, SocketDescriptor& socket = SocketDescriptor::Invalid, const NetworkAddress& address = NetworkAddress::Null );
        virtual							~PosixTCPSocket( void );

        // ** ITCPSocket
		virtual const NetworkAddress&	address( void ) const;
		virtual const SocketDescriptor&	descriptor( void ) const;
		virtual bool					isValid( void ) const;
        virtual bool					connectTo( const NetworkAddress& address, u16 port );
        virtual void					close( void );
        virtual void					update( void );
        virtual u32						sendTo( const void* buffer, u32 size );

    private:

		//! TCP socket event delegate.
		TCPSocketDelegatePtr		m_delegate;

		//! Socket descriptor.
        SocketDescriptor			m_socket;

		//! Socket receive buffer.
		TCPStreamPtr				m_stream;

		//! Socket address.
        NetworkAddress				m_address;
    };
    
} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_PosixTCPSocket_H__	*/