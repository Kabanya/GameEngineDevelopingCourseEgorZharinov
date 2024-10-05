#pragma once

#include <RenderEngine.h>
#include <RenderThread.h>
#include <Timer.h>
#include <Window/IWindow.h>
#include <vector>
#include <functional>

namespace GameEngine
{
    class GameObject;

    class Game final
    {
    public:
        Game() = delete;
        Game(std::function<bool()> PlatformLoopFunc);

    public:
        void Run();
        void Update(float dt);
        void ShuffleObjects();

    private:
        std::function<bool()> PlatformLoop = nullptr;

    private:
        Core::Timer m_GameTimer;
        std::unique_ptr<Render::RenderThread> m_renderThread;
        std::vector<GameObject*> m_Objects;
        std::vector<bool> m_IsIndependentObject;
        std::vector<Math::Vector3f> m_ObjectVelocities;
        std::vector<Math::Vector3f> m_ObjectDirections;
        std::vector<float> m_ObjectRanges;
        std::vector<Math::Vector3f> m_ObjectStartPositions;
        float m_IndependentObjectSpeed;
    };
}