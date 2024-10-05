#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>
#include <cmath>
#include <algorithm>
#include <random>

namespace GameEngine
{
    Game::Game(std::function<bool()> PlatformLoopFunc)
        : PlatformLoop(PlatformLoopFunc)
    {
        Core::g_MainCamera = new Core::Camera();
        Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
        Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

        m_renderThread = std::make_unique<Render::RenderThread>();

        m_IndependentObjectSpeed = 3.5f; 

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-10.0, 10.0);
        std::uniform_int_distribution<> type_dis(0, 1);

        // Create objects with random type
        for (int i = 0; i < 100; ++i) 
        {
            m_Objects.push_back(new GameObject());
            Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
            m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);

            bool isIndependent = type_dis(gen) == 0;  
            m_IsIndependentObject.push_back(isIndependent);

            float x = static_cast<float>(dis(gen));
            float y = isIndependent ? static_cast<float>(dis(gen)) : 30.0f; 
            float z = static_cast<float>(dis(gen));
            Math::Vector3f position(x, y, z);
            m_Objects.back()->SetPosition(position, m_renderThread->GetMainFrame());

            if (isIndependent)
            {
                m_ObjectDirections.push_back(Math::Vector3f(1.0f, 0.0f, 0.0f));
                m_ObjectRanges.push_back(10.0f);
                m_ObjectStartPositions.push_back(position);
                m_ObjectVelocities.push_back(Math::Vector3f(0.0f, 0.0f, 0.0f)); 
            }
            else
            {
                m_ObjectDirections.push_back(Math::Vector3f(0.0f, 0.0f, 0.0f));  
                m_ObjectRanges.push_back(0.0f); 
                m_ObjectStartPositions.push_back(Math::Vector3f(0.0f, 0.0f, 0.0f)); 
                m_ObjectVelocities.push_back(Math::Vector3f(0.0f, 0.0f, 0.0f));
            }
        }

        // Create plane object
        m_Objects.push_back(new GameObject());
        Render::RenderObject** planeRenderObject = m_Objects.back()->GetRenderObjectRef();
        m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Plane(), planeRenderObject);
        m_Objects.back()->SetPosition(Math::Vector3f(0.0f, -4.0f, 0.0f), m_renderThread->GetMainFrame());
        m_IsIndependentObject.push_back(false);  // The plane is not an independent object

        Core::g_InputHandler->RegisterCallback("GoForward", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
        Core::g_InputHandler->RegisterCallback("GoBack", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
        Core::g_InputHandler->RegisterCallback("GoRight", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
        Core::g_InputHandler->RegisterCallback("GoLeft", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });

        ShuffleObjects();
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

        for (size_t i = 0; i < m_Objects.size() - 1; ++i)  
        {
            Math::Vector3f pos = m_Objects[i]->GetPosition();

            if (m_IsIndependentObject[i])
            {
                Math::Vector3f dir = m_ObjectDirections[i];
                float range = m_ObjectRanges[i];
                Math::Vector3f startPos = m_ObjectStartPositions[i];

                pos = pos + dir * m_IndependentObjectSpeed * dt;

                if (std::abs((pos - startPos).GetLength()) >= range)
                {
                    dir = -dir;
                    m_ObjectDirections[i] = dir;
                }
            }
            else
            {//  physics cubes
                Math::Vector3f& velocity = m_ObjectVelocities[i];

                velocity.y -= gravity * dt;

                pos = pos + velocity * dt;

                if (pos.y - cubeHalfSize <= planeY &&
                    std::abs(pos.x) <= planeHalfWidth &&
                    std::abs(pos.z) <= planeHalfWidth)
                {
                    pos.y = planeY + cubeHalfSize;
                    velocity.y = -velocity.y * bounceFactor;
                }
            }

            m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
        }
    }

    void Game::ShuffleObjects()
    {
        std::random_device rd;
        std::mt19937 g(rd());

        for (size_t i = m_Objects.size() - 2; i > 0; --i)  
        {
            std::uniform_int_distribution<size_t> dist(0, i);
            size_t j = dist(g);

            std::swap(m_Objects[i], m_Objects[j]);

            bool tempBool = m_IsIndependentObject[i];
            m_IsIndependentObject[i] = m_IsIndependentObject[j];
            m_IsIndependentObject[j] = tempBool;

            Math::Vector3f tempVec = m_ObjectVelocities[i];
            m_ObjectVelocities[i] = m_ObjectVelocities[j];
            m_ObjectVelocities[j] = tempVec;

            tempVec = m_ObjectDirections[i];
            m_ObjectDirections[i] = m_ObjectDirections[j];
            m_ObjectDirections[j] = tempVec;

            float tempFloat = m_ObjectRanges[i];
            m_ObjectRanges[i] = m_ObjectRanges[j];
            m_ObjectRanges[j] = tempFloat;

            tempVec = m_ObjectStartPositions[i];
            m_ObjectStartPositions[i] = m_ObjectStartPositions[j];
            m_ObjectStartPositions[j] = tempVec;
        }
    }
}