#pragma once
#include "Mango/Core/System.h"
#include "Mango/Scene/Scene.h"

#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"

namespace mango
{
    namespace
    {
        // Layer that objects can be in, determines which other objects it can collide with
        // Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
        // layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
        // but only if you do collision testing).
        namespace Layers
        {
            static constexpr JPH::ObjectLayer NON_MOVING = 0;
            static constexpr JPH::ObjectLayer MOVING     = 1;
            static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
        };

        /// Class that determines if two object layers can collide
        class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
        {
        public:
            virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
            {
                MG_PROFILE_ZONE_SCOPED;

                switch (inObject1)
                {
                case Layers::NON_MOVING:
                    return inObject2 == Layers::MOVING; // Non moving only collides with moving
                case Layers::MOVING:
                    return true; // Moving collides with everything
                default:
                    MG_CORE_ASSERT(false);
                    return false;
                }
            }
        };

        // Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
        // a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
        // You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
        // many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
        // your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
        namespace BroadPhaseLayers
        {
            static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
            static constexpr JPH::BroadPhaseLayer MOVING    (1);
            static constexpr uint32_t             NUM_LAYERS(2);
        };

        // BroadPhaseLayerInterface implementation
        // This defines a mapping between object and broadphase layers.
        class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
        {
        public:
            BPLayerInterfaceImpl()
            {
                // Create a mapping table from object to broad phase layer
                m_objectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
                m_objectToBroadPhase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
            }

            virtual uint32_t GetNumBroadPhaseLayers() const override
            {
                return BroadPhaseLayers::NUM_LAYERS;
            }

            virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
            {
                JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
                return m_objectToBroadPhase[inLayer];
            }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
            virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
            {
                MG_PROFILE_ZONE_SCOPED;

                switch ((JPH::BroadPhaseLayer::Type)inLayer)
                {
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
                default: JPH_ASSERT(false); return "INVALID";
                }
            }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

        private:
            JPH::BroadPhaseLayer m_objectToBroadPhase[Layers::NUM_LAYERS];
        };

        /// Class that determines if an object layer can collide with a broadphase layer
        class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
        {
        public:
            virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
            {
                MG_PROFILE_ZONE_SCOPED;

                switch (inLayer1)
                {
                    case Layers::NON_MOVING:
                        return inLayer2 == BroadPhaseLayers::MOVING;
                    case Layers::MOVING:
                        return true;
                    default:
                        JPH_ASSERT(false);
                        return false;
                }
            }
        };

        // An example contact listener
        class MyContactListener : public JPH::ContactListener
        {
        public:
            // See: ContactListener
            virtual JPH::ValidateResult	OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override
            {
                MG_PROFILE_ZONE_SCOPED;
                // TODO: impl

                // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
                return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
            }

            virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
            {
                MG_PROFILE_ZONE_SCOPED;
                // TODO: impl
            }

            virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
            {
                MG_PROFILE_ZONE_SCOPED;
                // TODO: impl
            }

            virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
            {
                MG_PROFILE_ZONE_SCOPED;
                // TODO: impl
            }
        };

        // An example activation listener
        class MyBodyActivationListener : public JPH::BodyActivationListener
        {
        public:
            virtual void OnBodyActivated(const JPH::BodyID& inBodyID, uint64_t inBodyUserData) override
            {
                MG_PROFILE_ZONE_SCOPED;
                // TODO: impl
            }

            virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64_t inBodyUserData) override
            {
                MG_PROFILE_ZONE_SCOPED;
                // TODO: impl
            }
        };
    }

    class PhysicsSystem : public System
    {
    public:
        PhysicsSystem();
        virtual ~PhysicsSystem() = default;

        void onInit()           override;
        void onDestroy()        override;
        void onUpdate(float dt) override;

        void start();
        void stop();

    private:
        void onInitBodies();
        void onDestroyBodies();

    private:
        // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
        // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
        const uint32_t c_maxBodies = 1024;

        // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
        const uint32_t c_numBodyMutexes = 0;

        // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
        // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
        // too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
        // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
        const uint32_t c_maxBodyPairs = 1024;

        // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
        // number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
        // Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
        const uint32_t c_maxContactConstraints = 1024;

        // Create mapping table from object layer to broadphase layer
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        BPLayerInterfaceImpl m_broadPhaseLayerInterface;

        // Create class that filters object vs broadphase layers
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadphaseLayerFilter;

        // Create class that filters object vs object layers
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        ObjectLayerPairFilterImpl m_objectVsObjectLayerFilter;

        MyBodyActivationListener m_bodyActivationListener;
        MyContactListener        m_contactListener;

        JPH::PhysicsSystem*       m_physicsSystem;
        JPH::TempAllocatorImpl*   m_tempAllocator;
        JPH::JobSystemThreadPool* m_jobSystem;

        ref<Scene> m_scene;

        enum class PhysicsSystemState
        {
            Running = 0, Stopped = 1
        };

        PhysicsSystemState m_physicsSystemState = PhysicsSystemState::Stopped;
    };
}
