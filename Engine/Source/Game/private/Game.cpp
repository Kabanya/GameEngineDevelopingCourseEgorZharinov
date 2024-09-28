#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace GameEngine
{
    Game::Game(
        std::function<bool()> PlatformLoopFunc
    ) :
        PlatformLoop(PlatformLoopFunc)
    {
        Core::g_MainCamera = new Core::Camera();
        Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
        Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

        m_renderThread = std::make_unique<Render::RenderThread>();

        // How many objects do we want to create
        for (int i = 0; i < 1; ++i)
        {
            m_Objects.push_back(new GameObject());
            Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
            m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
        }

        // Load key mappings from config file
        LoadKeyMappings();
    }

    void Game::Run()
    {
        assert(PlatformLoop != nullptr);

        m_GameTimer.Reset();

        bool quit = false;
        while (!quit)
        {
            m_GameTimer.Tick();
            float dt = m_GameTimer.GetDeltaTime();

            Update(dt);

            quit = !PlatformLoop();

            m_renderThread->OnEndFrame();
        }
    }

    void Game::Update(float dt)
    {
        for (int i = 0; i < m_Objects.size(); ++i)
        {
            Math::Vector3f pos = m_Objects[i]->GetPosition();

            if (i == 0)
            {
                if (GetKeyState(m_KeyMapping.FlyLeft) & 0x8000) pos.x -= speedObj * dt;
                if (GetKeyState(m_KeyMapping.FlyRight) & 0x8000) pos.x += speedObj * dt;
                if (GetKeyState(m_KeyMapping.FlyForward) & 0x8000) pos.z += speedObj * dt;
                if (GetKeyState(m_KeyMapping.FlyBack) & 0x8000) pos.z -= speedObj * dt;
                if (GetKeyState(m_KeyMapping.FlyUp) & 0x8000) pos.y += speedObj * dt;
                if (GetKeyState(m_KeyMapping.FlyDown) & 0x8000) pos.y -= speedObj * dt;
            }
            m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
        }
    }

    void Game::LoadKeyMappings()
    {
        std::filesystem::path configPath = GetConfigFilePath();
        std::cout << "Config file path: " << configPath << std::endl;

        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            std::cerr << "Unable to open config file: " << configPath << std::endl;
            return;
        }

        std::string line;
        while (std::getline(configFile, line))
        {
            std::istringstream iss(line);
            std::string key, value;

            if (std::getline(iss, key, '=') && std::getline(iss, value))
            {
                if (key == "FlyLeft") m_KeyMapping.FlyLeft = StringToVirtualKey(value);
                else if (key == "FlyRight") m_KeyMapping.FlyRight = StringToVirtualKey(value);
                else if (key == "FlyForward") m_KeyMapping.FlyForward = StringToVirtualKey(value);
                else if (key == "FlyBack") m_KeyMapping.FlyBack = StringToVirtualKey(value);
                else if (key == "FlyUp") m_KeyMapping.FlyUp = StringToVirtualKey(value);
                else if (key == "FlyDown") m_KeyMapping.FlyDown = StringToVirtualKey(value);
            }
        }
    }

    int Game::StringToVirtualKey(const std::string& keyName)
    {
        if (keyName == "space") return VK_SPACE;
        if (keyName == "control") return VK_CONTROL;
        return toupper(keyName[0]); 
    }

    std::filesystem::path Game::GetConfigFilePath() const
    {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::filesystem::path execPath(path);

        std::filesystem::path projectRoot = execPath.parent_path().parent_path().parent_path().parent_path();

        std::filesystem::path configPath = projectRoot / "Assets" / "controlKeyboard.txt";

        std::cout << "Config file path: " << configPath << std::endl;

        return configPath;
    }
}