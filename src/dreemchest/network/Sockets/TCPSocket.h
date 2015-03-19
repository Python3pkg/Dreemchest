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

#ifndef		__DC_Network_TCPSocket_H__
#define		__DC_Network_TCPSocket_H__

#include	"SocketDescriptor.h"

DC_BEGIN_DREEMCHEST

namespace net {

	BeginPrivateInterface( TCPSocket )
		InterfaceMethod( const NetworkAddress&		address( void ) const )
		InterfaceMethod( const SocketDescriptor&	descriptor( void ) const )
		InterfaceMethod( bool						isValid( void ) const )
		InterfaceMethod( bool						connectTo( const NetworkAddress& address, u16 port ) )
		InterfaceMethod( void						close( void ) )
		InterfaceMethod( void						update( void ) )
		InterfaceMethod( u32						sendTo( const void* buffer, u32 size ) )
	EndPrivateInterface

    //! TCP socket class.
    class TCPSocket : public RefCounted {
    public:

									//! Constructs a TCPSocket instance.
									TCPSocket( impl::TCPSocketPrivate* impl = NULL );
		virtual						~TCPSocket( void ) {}

									//! Returns true if this socket is valid.
									operator bool() const;

		//! Returns a socket descriptor.
		const SocketDescriptor&		descriptor( void ) const;

		//! Returns true if this socket is valid.
		bool						isValid( void ) const;

		//! Closes a socket.
		void						close( void );

		//! Reads all incoming data.
        void						update( void );

		//! Returns a remote address.
		const NetworkAddress&		address( void ) const;

		//! Sends data to socket.
		/*
		\param buffer Data to be sent.
		\param size Data size to be sent.
		*/
        u32							sendTo( const void* buffer, u32 size );

		//! Connects to a TCP socket at a given remote address and port.
		static TCPSocketPtr			connectTo( const NetworkAddress& address, u16 port, TCPSocketDelegate* delegate = NULL );
        
		UsePrivateInterface( TCPSocket )
    };

	//! TCP socket event delegate.
	class TCPSocketDelegate : public RefCounted {
	public:

		virtual					~TCPSocketDelegate( void ) {}

		//! Handles a socket disconnection.
		virtual void			handleClosed( TCPSocket* sender ) {}

		//! Handles a received data.
		virtual void			handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream ) {}
	};

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_TCPSocket_H__	*/