#pragma once

#include <flecs.h>

namespace GameEngine::Core
{
	class Camera;
	class Controller;
}

struct ControllerPtr
{
	GameEngine::Core::Controller* ptr;
};

struct JumpSpeed
{
	float value;
};

struct CameraPtr
{
	GameEngine::Core::Camera* ptr;
};

struct ShootingComponent
{
	float projectileSpeed;
	float shootCooldown;
	float lastShootTime;
};

void RegisterEcsControlSystems(flecs::world& world);