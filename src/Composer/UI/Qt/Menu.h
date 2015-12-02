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

#ifndef __DC_Composer_Menu_H__
#define __DC_Composer_Menu_H__

#include "../IMenu.h"

namespace Ui {

	//! Delegate object to bind Qt signals to a callback functions.
	class SignalDelegate : public QObject {

		Q_OBJECT

	public:

		//! Callback type used by this signal delegate.
		typedef std::function<void()>	Callback;

										//! Constructs the SignalDelegate instance.
										SignalDelegate( Callback callback, QObject* sender, CString signal );

	private slots:

		//! Processes the emitted signal.
		virtual void					emitted( void );

	private:

		Callback						m_callback;	//!< Callback function.
	};

	//! Generic class to declare Qt interface implementations.
	template<typename TBase, typename TPrivate>
	class Interface : public TBase {
	public:

									//! Constructs the Interface instance.
									Interface( TPrivate* instance )
										: m_private( instance ) {}

		//! Returns the private interface.
		virtual void*				ptr( void ) const { return m_private.get(); }

	protected:

		AutoPtr<TPrivate>			m_private;	//!< Actual implementation instance.
	};

	//! Menu action Qt implementation
	class Action : public Interface<IAction, QAction> {
	public:

										//! Constructs Action instance.
										Action( QWidget* parent, const String& text, ActionCallback callback );
								
		//! Returns the checked state of a menu action.
		virtual bool					isChecked( void ) const;
	
		//! Sets the checked state of a menu action.
		virtual void					setChecked( bool value );

		//! Sets the checkable state of a menu action.
		virtual void					setCheckable( bool value );

		//! Sets the disabled state of a menu action.
		virtual void					setDisabled( bool value );
	
		//! Returns the visibility state of a menu action.
		virtual bool					isVisible( void ) const;
	
		//! Sets the visibility state of a menu action.
		virtual void					setVisible( bool value );
	
		//! Returns the menu action text.
		virtual String					text( void ) const;
	
		//! Sets the menu action text.
		virtual void					setText( const String& value );
	
		//! Sets the menu action shortcut.
		virtual void					setShortcut( const String& value );
	
		//! Sets the menu action icon.
		virtual void					setIcon( const String& value );
	
	private:

		//! Triggered callback.
		void							triggered( void );
	
	private:

		AutoPtr<SignalDelegate>			m_signal;	//!< Signal delegate.
		ActionCallback					m_callback;	//!< Menu action callback.
	};

	//! Tool bar Qt implementation.
	class ToolBar : public Interface<IToolBar, QToolBar> {
	public:

									//! Constructs ToolBar instance.
									ToolBar( QWidget* parent );

		//! Adds an action to a toolbar.
		virtual void				addAction( IActionWPtr action );
	
		//! Removes an action from a toolbar.
		virtual void				removeAction( IActionWPtr action );
	
		//! Adds a separator to a toolbar.
		virtual void				addSeparator( void );
	};

	//! Menu Qt implementation.
	class Menu : public Interface<IMenu, QMenu> {
	public:

									//! Constructs Menu instance.
									Menu( QWidget* parent );

		//! Adds new action to a menu.
		virtual IActionWPtr			addAction( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 );
	
		//! Removes action from a menu.
		virtual void				removeAction( IActionWPtr action );

		//! Adds a separator item to a menu.
		virtual void				addSeparator( void );

		//! Sets the menu title.
		virtual void				setTitle( const String& value );
	
	private:

		QVector<IActionPtr>			m_actions;	//!< All added actions reside here.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Menu_H__	*/