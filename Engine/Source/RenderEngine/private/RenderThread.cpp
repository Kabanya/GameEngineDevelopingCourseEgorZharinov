#include <RenderCommands.h>
#include <RenderEngine.h>
#include <RenderThread.h>

using namespace GameEngine::Render;

namespace
{
	// Function to run render thread
	static void RunThisThread(void* thisPtr) //helper function
	{
		RenderThread* const self = reinterpret_cast<RenderThread*>(thisPtr);
		self->Run();
	}
}

namespace GameEngine::Render
{
	RenderThread::RenderThread() //create the thread
	{
		m_MainThreadId = std::this_thread::get_id();

		frameMutex[m_CurMainFrame].lock(); // *can ast prof. why it mutexed

		m_Thread = std::make_unique<std::jthread>(RunThisThread, this);
		m_Thread->detach();
	}

	RenderThread::~RenderThread()
	{
		delete m_RenderEngine;
	}

	// Render Loop
	void RenderThread::Run()
	{
		m_RenderThreadId = std::this_thread::get_id(); 

		m_RenderEngine = new RenderEngine();

		while (true)
		{
			std::lock_guard<std::mutex> lock(frameMutex[m_CurrRenderFrame]);

			ProcessCommands(); //for current frame

			m_RenderEngine->Update(m_CurrRenderFrame);

			OnEndFrame();  // egalement au switchFrame
		}
	}

	bool RenderThread::IsRenderThread() const
	{
		return m_RenderThreadId == std::this_thread::get_id();
	}

	template<typename... Args>
	void RenderThread::EnqueueCommand(ERC command, Args... args) //EnqueueCommand - event based messaging!
	{
		switch (command)
		{
		case ERC::CreateRenderObject:
			m_commands[m_CurMainFrame].push_back(
				new EnqueuedRenderCommand(
					[this](RenderCore::Geometry::Ptr geometry, RenderObject** renderObject) { m_RenderEngine->CreateRenderObject(geometry, renderObject); },
					std::forward<Args>(args)...) //lamda function
			);
			break;
		default:
			assert(0);
			break;
		}

		if (IsRenderThread())
		{
			RenderCommand* renderCommand = m_commands[m_CurMainFrame].back();
			renderCommand->DoTask();
			delete renderCommand;
			m_commands[m_CurMainFrame].pop_back();
		}
	}

	void RenderThread::ProcessCommands()
	{
		for (auto& command : m_commands[m_CurrRenderFrame])
		{
			command->DoTask();
		}

		m_commands[m_CurrRenderFrame].clear();
	}

	void RenderThread::OnEndFrame()
	{
		SwitchFrame();
	}

	void RenderThread::SwitchFrame()
	{
		if (IsRenderThread())
		{
			m_CurrRenderFrame = GetNextFrame(m_CurrRenderFrame); // get number of next buffer // 117
		}
		else 
		{
			frameMutex[m_CurMainFrame].unlock();

			m_CurMainFrame = GetNextFrame(m_CurMainFrame);

			frameMutex[m_CurMainFrame].lock(); 
		}
	}

	size_t RenderThread::GetNextFrame(size_t frameNumber) const
	{
		return (frameNumber + 1) % RenderCore::g_FrameBufferCount;
	}
}