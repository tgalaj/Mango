#include "mgpch.h"
#include "PhysicsSystem.h"
#include "Mango/Scene/Components.h"
#include "Mango/Scene/Entity.h"
#include "Mango/Scene/SceneManager.h"
#include "Mango/Core/Services.h"

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

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

namespace
{
    static void joltTraceCallback(const char* inFMT, ...)
    {
        MG_PROFILE_ZONE_SCOPED;

        // Format the message
        va_list list;
        va_start(list, inFMT);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), inFMT, list);
        va_end(list);

        MG_CORE_TRACE("Physics System: ", buffer);
    }

#ifdef JPH_ENABLE_ASSERTS
    static bool joltAssertFailedCallback(const char* inExpression, const char* inMessage, const char* inFile, uint32_t inLine)
    {
        MG_PROFILE_ZONE_SCOPED;

        MG_CORE_ASSERT_MSG("({}) {}", inExpression, (inMessage != nullptr ? inMessage : ""));
        return true;
    };
#endif // JPH_ENABLE_ASSERTS

    static inline JPH::Vec3 glmVec3ToJoltVec3(const glm::vec3& v)
    {
        MG_PROFILE_ZONE_SCOPED;
        return JPH::Vec3(v.x, v.y, v.z);
    }

    static inline JPH::ObjectLayer getLayer(JPH::EMotionType motionType)
    {
        MG_PROFILE_ZONE_SCOPED;

        switch (motionType)
        {
        case JPH::EMotionType::Static:
            return mango::Layers::NON_MOVING;
        case JPH::EMotionType::Kinematic:
            return mango::Layers::MOVING;
        case JPH::EMotionType::Dynamic:
            return mango::Layers::MOVING;
        default:
            MG_CORE_ASSERT_FAIL("Unknon motionType.");
            return mango::Layers::NON_MOVING;
        }
    }
}

namespace mango
{
    PhysicsSystem::PhysicsSystem()
        : System("PhysicsSystem")
    {
        MG_PROFILE_ZONE_SCOPED;
    }

    void PhysicsSystem::onInit()
    {
        MG_PROFILE_ZONE_SCOPED;

        // Register allocation hook
        JPH::RegisterDefaultAllocator();

        // Install callbacks
        JPH::Trace = joltTraceCallback;
        JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = joltAssertFailedCallback;)

        // Create a factory
        JPH::Factory::sInstance = new JPH::Factory();

        // Register all Jolt physics types
        JPH::RegisterTypes();

        // We need a temp allocator for temporary allocations during the physics update. We're
        // pre-allocating 10 MB to avoid having to do allocations during the physics update.
        // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
        // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
        // malloc / free.
        m_tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

        // We need a job system that will execute physics jobs on multiple threads. Typically
        // you would implement the JobSystem interface yourself and let Jolt Physics run on top
        // of your own job scheduler. JobSystemThreadPool is an example implementation.
        m_jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

        // Now we can create the actual physics system.
        m_physicsSystem = new JPH::PhysicsSystem();
        m_physicsSystem->Init(c_maxBodies, c_numBodyMutexes, c_maxBodyPairs, c_maxContactConstraints, m_broadPhaseLayerInterface, m_objectVsBroadphaseLayerFilter, m_objectVsObjectLayerFilter);

        // A body activation listener gets notified when bodies activate and go to sleep
        // Note that this is called from a job so whatever you do here needs to be thread safe.
        // Registering one is entirely optional.
        m_physicsSystem->SetBodyActivationListener(&m_bodyActivationListener);

        // A contact listener gets notified when bodies (are about to) collide, and when they separate again.
        // Note that this is called from a job so whatever you do here needs to be thread safe.
        // Registering one is entirely optional.
        m_physicsSystem->SetContactListener(&m_contactListener);
    }

    void PhysicsSystem::onInitBodies()
    {
        MG_PROFILE_ZONE_SCOPED;

        // The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
        // variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
        JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

        m_scene = Services::sceneManager()->getActiveScene();

        auto view = m_scene->getEntitiesWithComponent<RigidBody3DComponent>();
        for (auto e : view)
        {
                  Entity      entity         = { e, m_scene.get() };
                  auto        scale          = entity.scale();
            const JPH::Shape* collisionShape = nullptr;
            if (entity.hasComponent<BoxCollider3DComponent>())
            {
                auto& bc3d           = entity.getComponent<BoxCollider3DComponent>();
                      collisionShape = new JPH::RotatedTranslatedShape({ bc3d.offset.x, bc3d.offset.y, bc3d.offset.z }, 
                                                                       JPH::Quat::sIdentity(), 
                                                                       new JPH::BoxShape(glmVec3ToJoltVec3(bc3d.halfExtent * scale)));
            }

            if (entity.hasComponent<SphereColliderComponent>())
            {
                auto& sc3d           = entity.getComponent<SphereColliderComponent>();
                collisionShape       = new JPH::RotatedTranslatedShape({ sc3d.offset.x, sc3d.offset.y, sc3d.offset.z },
                                                                       JPH::Quat::sIdentity(), 
                                                                       new JPH::SphereShape(sc3d.radius * scale.x));
            }

            if (!collisionShape)
            {
                MG_CORE_WARN("Physics System: entity named [{}] does not have a collision shape assigned."
                             "Did you forget to add a collider component?", 
                             entity.getComponent<TagComponent>().tag);
                continue;
            }

            auto& rb3d = entity.getComponent<RigidBody3DComponent>();

            auto position    = entity.position();
            auto orientation = entity.orientation();

            JPH::BodyCreationSettings bodySettings(collisionShape,
                                                   JPH::RVec3(position.x, position.y, position.z),
                                                   JPH::Quat(orientation.x, orientation.y, orientation.z, orientation.w),
                                                   (JPH::EMotionType)rb3d.motionType,
                                                   getLayer((JPH::EMotionType)rb3d.motionType));

            bodySettings.mFriction       = rb3d.friction;
            bodySettings.mRestitution    = rb3d.restitution;
            bodySettings.mLinearDamping  = rb3d.linearDamping;
            bodySettings.mAngularDamping = rb3d.angularDamping;

            // Create the rigid body
            rb3d.runtimeBody = bodyInterface.CreateBody(bodySettings);
            bodyInterface.AddBody(static_cast<JPH::Body*>(rb3d.runtimeBody)->GetID(),
                                   rb3d.isInitiallyActivated ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
        }

        // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
        // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
        // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
        m_physicsSystem->OptimizeBroadPhase();
    }

    void PhysicsSystem::onDestroy()
    {
        MG_PROFILE_ZONE_SCOPED;

        // Remove the bodies from the physics system. 
        // Note that the body itself keeps all of its state and can be re-added at any time.
        JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

        auto view = m_scene->getEntitiesWithComponent<RigidBody3DComponent>();
        for (auto e : view)
        {
            Entity entity = { e, m_scene.get() };
            auto& rb3d = entity.getComponent<RigidBody3DComponent>();

            bodyInterface.RemoveBody(static_cast<JPH::Body*>(rb3d.runtimeBody)->GetID());
            bodyInterface.DestroyBody(static_cast<JPH::Body*>(rb3d.runtimeBody)->GetID());
        }

        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;

        delete m_jobSystem;
        delete m_tempAllocator;
        delete m_physicsSystem;
    }

    void PhysicsSystem::onUpdate(float dt)
    {
        MG_PROFILE_ZONE_SCOPED;
        
        // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
        // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
        const float c_deltaTime      = 1.0f / 60.0f;
        const int   c_collisionSteps = 1;

        // Step the physics world
        m_physicsSystem->Update(c_deltaTime, c_collisionSteps, m_tempAllocator, m_jobSystem);

        JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

        auto view = m_scene->getEntitiesWithComponent<RigidBody3DComponent>();
        for (auto e : view)
        {
            Entity entity = { e, m_scene.get() };

            auto& rb3d = entity.getComponent<RigidBody3DComponent>();

            if (rb3d.runtimeBody)
            {
                auto bodyID = static_cast<JPH::Body*>(rb3d.runtimeBody)->GetID();
                if (bodyInterface.IsActive(bodyID))
                {
                    JPH::RVec3 p = bodyInterface.GetCenterOfMassPosition(bodyID);
                    JPH::Quat  r = bodyInterface.GetRotation(bodyID);

                    entity.setPosition(p.GetX(), p.GetY(), p.GetZ());
                    entity.setOrientation(glm::quat(r.GetW(), r.GetX(), r.GetY(), r.GetZ()));
                }
            }
        }
    }
}