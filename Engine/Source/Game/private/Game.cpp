#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>
#include <cmath>

namespace GameEngine
{
    Game::Game(std::function<bool()> PlatformLoopFunc)
        : PlatformLoop(PlatformLoopFunc)
    {
        Core::g_MainCamera = new Core::Camera();
        Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
        Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

        m_renderThread = std::make_unique<Render::RenderThread>();

        // Create cube objects
        for (int i = 0; i < 12; ++i)
        {
            m_Objects.push_back(new GameObject());
            Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
            m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);

            // Initialize cube positions and velocities
            m_Objects[i]->SetPosition(Math::Vector3f(i * 2.0f - 5.0f, 20.0f, 0.0f), m_renderThread->GetMainFrame());
            m_CubeVelocities.push_back(Math::Vector3f(0.0f, 0.0f, 0.0f));
        }

        // Create plane object
        m_Objects.push_back(new GameObject());
        Render::RenderObject** planeRenderObject = m_Objects.back()->GetRenderObjectRef();
        m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Plane(), planeRenderObject);
        m_Objects.back()->SetPosition(Math::Vector3f(0.0f, -4.0f, 0.0f), m_renderThread->GetMainFrame());

        Core::g_InputHandler->RegisterCallback("GoForward", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
        Core::g_InputHandler->RegisterCallback("GoBack", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
        Core::g_InputHandler->RegisterCallback("GoRight", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
        Core::g_InputHandler->RegisterCallback("GoLeft", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });
    }

    void Game::Run()
    {
        m_GameTimer.Reset();
        bool quit = false;
        while (!quit)
        {
            m_GameTimer.Tick();
            float dt = m_GameTimer.GetDeltaTime();
            Core::g_MainWindowsApplication->Update();
            Core::g_InputHandler->Update();
            Core::g_MainCamera->Update(dt);
            Update(dt);
            m_renderThread->OnEndFrame();
            quit = !PlatformLoop();
        }
    }

    void Game::Update(float dt)
    {
        const float gravity = 9.8f;
        const float bounceFactor = 0.6f;
        const float planeY = 0.0f;
        const float planeHalfWidth = 5.0f;
        const float cubeHalfSize = 0.5f;

        for (int i = 0; i < m_Objects.size() - 1; ++i)
        {
            Math::Vector3f pos = m_Objects[i]->GetPosition();
            Math::Vector3f& velocity = m_CubeVelocities[i];

            velocity.y -= gravity * dt;

            pos = pos + velocity * dt;

            // Check collision
            if (pos.y - cubeHalfSize <= planeY &&
                std::abs(pos.x) <= planeHalfWidth &&
                std::abs(pos.z) <= planeHalfWidth)
            {
                pos.y = planeY + cubeHalfSize;
                velocity.y = -velocity.y * bounceFactor;
            }

            m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
        }
    }
}