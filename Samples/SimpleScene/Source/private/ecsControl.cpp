#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>

using namespace GameEngine;

void RegisterEcsControlSystems(flecs::world& world)
{
    world.system<Position, CameraPtr, const Speed, const ControllerPtr>()
        .each([&](flecs::entity e, Position& position, CameraPtr& camera, const Speed& speed, const ControllerPtr& controller)
            {
                Math::Vector3f currentMoveDir = Math::Vector3f::Zero();
                if (controller.ptr->IsPressed("GoLeft"))
                {
                    currentMoveDir = currentMoveDir - camera.ptr->GetRightDir();
                }
                if (controller.ptr->IsPressed("GoRight"))
                {
                    currentMoveDir = currentMoveDir + camera.ptr->GetRightDir();
                }
                if (controller.ptr->IsPressed("GoBack"))
                {
                    currentMoveDir = currentMoveDir - camera.ptr->GetViewDir();
                }
                if (controller.ptr->IsPressed("GoForward"))
                {
                    currentMoveDir = currentMoveDir + camera.ptr->GetViewDir();
                }
                position.value = position.value + currentMoveDir.Normalized() * speed * world.delta_time();
                camera.ptr->SetPosition(position.value);
            });

    world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
        .each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
            {
                constexpr float planeEpsilon = 0.1f;
                if (plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z < plane.value.w + planeEpsilon)
                {
                    if (controller.ptr->IsPressed("Jump"))
                    {
                        vel.value.y = jump.value;
                    }
                }
            });

    world.system<const Position, const CameraPtr, ShootingComponent, const ControllerPtr>()
        .each([&](flecs::entity e, const Position& position, const CameraPtr& camera, ShootingComponent& shooting, const ControllerPtr& controller)
            {
                if (controller.ptr->IsPressed("Shoot") && (world.delta_time() - shooting.lastShootTime > shooting.shootCooldown))
                {
                    Math::Vector3f shootDir = camera.ptr->GetViewDir();
                    Math::Vector3f spawnPos = position.value;

                    flecs::entity bullet = world.entity()
                        .set(Position{ spawnPos })
                        .set(Velocity{ shootDir * shooting.projectileSpeed })
                        .set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
                        .set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
                        .set(RenderObjectPtr{ new Render::RenderObject() })
                        .add<Bullet>();

                    shooting.lastShootTime = world.delta_time();
                }
            });
}