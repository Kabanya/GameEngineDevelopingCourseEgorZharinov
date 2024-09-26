#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>

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
		for (int i = 0; i < 3; ++i)
		{
			m_Objects.push_back(new GameObject());
			Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject); /*create objects in a separate thread
																											the best way to make based event mes. into render thread*/ 
		}
	}

	void Game::Run() // in main()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset(); // starting point

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick(); // call tick
			float dt = m_GameTimer.GetDeltaTime();

			Update(dt);
			
			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop(); 

			m_renderThread->OnEndFrame();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			Math::Vector3f pos = m_Objects[i]->GetPosition(); //update position 

			// Showcase
			if (i == 0)
			{
				pos.x -= 1.5f * dt;
			}
			else if (i == 1)
			{
				pos.y -= 1.5f * dt;
				pos.z += 1.0f * dt;
			}
			else if (i == 2)
			{
				pos.x += 3.5f * dt;
				pos.y -= 2.5f * dt;
			}
			m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame()); // new position
		}
	}
}