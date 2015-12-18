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

#ifndef __DC_Composer_Qt_SceneModelPrivate_H__
#define __DC_Composer_Qt_SceneModelPrivate_H__

#include "AbstractTreeModel.h"
#include "../SceneModel.h"

DC_BEGIN_COMPOSER

	class SceneModelPrivate;

	//! Scene model Qt implementation.
	class QSceneModel : public QGenericTreeModel<Scene::SceneObjectWPtr> {
	public:

								//! Constructs QSceneModel instance.
								QSceneModel( SceneModelPrivate* parentSceneModel, Scene::SceneWPtr scene, QObject* parent = NULL );

		//! Removes the scene object at specified index.
		void					remove( const QModelIndex& index );

	private:

		//! Returns the item flags.
		virtual Qt::ItemFlags	flags( const QModelIndex& index ) const Q_DECL_OVERRIDE;

		//! Returns the model data by index.
		virtual QVariant		data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;

		//! Writes new data to model at specified index.
		virtual bool			setData( const QModelIndex& index, const QVariant& value, int role ) Q_DECL_OVERRIDE;

		//! Changes the scene object parent transform.
		virtual bool			moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const DC_DECL_OVERRIDE;

		//! Handles scene object addition.
		void					handleSceneObjectAdded( const Scene::Scene::SceneObjectAdded& e );

		//! Handles scene object removal.
		void					handleSceneObjectRemoved( const Scene::Scene::SceneObjectRemoved& e );

	private:

		SceneModelPrivate*		m_parent;	//!< Parent scene model.
		Scene::SceneWPtr		m_scene;	//!< Actual scene.
	};

	//! Scene model interface.
	class SceneModelPrivate : public PrivateInterface<SceneModel, QSceneModel> {
	public:

								//! Constructs SceneModelPrivate instance.
								SceneModelPrivate( Scene::SceneWPtr scene );
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Qt_SceneModelPrivate_H__	*/