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

#ifndef __DC_Scene_Systems_Transofrm_H__
#define __DC_Scene_Systems_Transofrm_H__

#include "../Scene.h"
#include "../Components/Transform.h"
#include "../Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Affine transform system calculates the transformation matricies for all transform components.
    class AffineTransformSystem : public Ecs::GenericEntitySystem<AffineTransformSystem, Transform> {
    protected:

        //! Calculates the affine transform matrix for each transform component.
        virtual void        update( u32 currentTime, f32 dt ) NIMBLE_OVERRIDE;

    private:

        //! Called when entity was added.
        virtual void        entityAdded( const Ecs::Entity& entity ) NIMBLE_OVERRIDE;

        //! Called when entity was removed.
        virtual void        entityRemoved( const Ecs::Entity& entity ) NIMBLE_OVERRIDE;

    private:

        Array<Transform*>    m_transforms;    //!< Active scene transform components.
    };

    //! World space bounding box system calculates bounding volumes for static meshes in scene.
    class WorldSpaceBoundingBoxSystem : public Ecs::GenericEntitySystem<WorldSpaceBoundingBoxSystem, StaticMesh, Transform> {
    protected:

        //! Calculates the world space bounds for static mesh.
        virtual void        process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, StaticMesh& staticMesh, Transform& transform ) NIMBLE_OVERRIDE;
    };

    //! Moves scene object transform along coordinate axes.
    class MoveAlongAxesSystem : public Ecs::GenericEntitySystem<MoveAlongAxesSystem, MoveAlongAxes, Transform> {
    protected:

        //! Moves the scene object
        virtual void        process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, MoveAlongAxes& moveAlongAxes, Transform& transform ) NIMBLE_OVERRIDE;
    };

    //! Rotates scene object transform around axes.
    class RotateAroundAxesSystem : public Ecs::GenericEntitySystem<RotateAroundAxesSystem, RotateAroundAxes, Transform> {
    protected:

        //! Moves the scene object
        virtual void        process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, RotateAroundAxes& rotateAroundAxes, Transform& transform ) NIMBLE_OVERRIDE;
    };

    //! The moving to target system
    class MoveToSystem : public Ecs::GenericEntitySystem<MoveToSystem, MoveTo, Transform> {
    protected:

        //! Follows the target transform
        virtual void        process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, MoveTo& follow, Transform& transform ) NIMBLE_OVERRIDE;

        //! Attaches the internal following data to an added scene object.
        virtual void        entityAdded( const Ecs::Entity& entity ) NIMBLE_OVERRIDE;

    private:

        //! Holds the accumulated acceleration.
        struct Internal : Ecs::MixIn<Internal> {
            Vec3            m_force;        //!< The accumulated force.
        };
    };

    //! Particles update system
    class ParticlesSystem : public Ecs::GenericEntitySystem<ParticlesSystem, Particles, Transform> {
    protected:

        //! Updates the particle system.
        virtual void        process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, Particles& particles, Transform& transform ) NIMBLE_OVERRIDE;
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_Transofrm_H__    */
