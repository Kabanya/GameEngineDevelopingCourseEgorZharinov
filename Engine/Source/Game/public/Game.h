#pragma once

#include <RenderEngine.h>
#include <RenderThread.h>
#include <Timer.h>
#include <Window/IWindow.h>
#include <unordered_map>
#include <string>
#include <filesystem>

namespace GameEngine
{
    class GameObject;

    class Game final
    {
    public:
        Game() = delete;
        Game(
            std::function<bool()> PlatformLoopFunc
        );

    public:
        void Run();
        void Update(float dt);

    private:

        std::function<bool()> PlatformLoop = nullptr;
        float speedObj = 2.5f;

        struct KeyMapping {
            int FlyLeft;
            int FlyRight;
            int FlyForward;
            int FlyBack;
            int FlyUp;
            int FlyDown;
        };

        KeyMapping m_KeyMapping;

        void LoadKeyMappings();

        int StringToVirtualKey(const std::string& keyName);

        std::filesystem::path GetConfigFilePath() const;

    private:
        Core::Timer m_GameTimer;
        std::unique_ptr<Render::RenderThread> m_renderThread;
        std::vector<GameObject*> m_Objects;
    };
}