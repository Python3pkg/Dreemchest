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

#ifndef __DC_Composer_Qt_Document_H__
#define __DC_Composer_Qt_Document_H__

#include "Widget.h"

DC_BEGIN_COMPOSER

namespace Ui {

	//! Subclass of a QDockWidget.
	class QDocumentDock : public QDockWidget {

		Q_OBJECT

	public:

											//! Constructs the QDocumentDock instance.
											QDocumentDock( Document* document, const QString& title, QWidget* parent );
	private:

		//! Handles the closed event.
		virtual void						closeEvent( QCloseEvent *e ) Q_DECL_OVERRIDE;

	private slots:

		//! Handles the visibility changed signal
		void								visibilityChanged( bool visible );

	private:

		Document*							m_document;	//!< Parent document dock.
	};

	//! Document Qt implementation.
	class Document : public PrivateInterface<IDocument, QDocumentDock> {
	friend class QDocumentDock;
	public:

											//! Constructs the Document instance.
											Document( IMainWindowWPtr mainWindow, Editors::AssetEditorPtr assetEditor, const String& title, QWidget* parent = NULL );

		//! Returns the rendering frame used for this document dock.
		virtual IRenderingFrameWPtr			renderingFrame( void );

		//! Attaches the rendering frame to this document.
		virtual IRenderingFrameWPtr			attachRenderingFrame( void );

		//! Returns an attached asset editor.
		virtual Editors::AssetEditorWPtr	assetEditor( void ) const;

	private:

		//! Sets this document as active.
		void								activate( void );

		//! Closes this document.
		bool								close( void );

	private:

		IMainWindowWPtr						m_mainWindow;		//!< Parent main window.
		IRenderingFramePtr					m_renderingFrame;	//!< The attached rendering frame.
		Editors::AssetEditorPtr				m_assetEditor;		//!< Asset editor attached to this document dock.
	};

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Qt_Document_H__	*/