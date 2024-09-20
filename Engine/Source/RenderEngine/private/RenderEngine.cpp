#include <RenderEngine.h>
#include <RHIHelper.h>
#include <Mesh.h>

namespace GameEngine::Render
{
	RenderEngine::RenderEngine()
	{
		m_rhi = HAL::RHIHelper::CreateRHI("D3D12");
		m_rhi->Init();

		//Stele
		RenderObject::Ptr stele = std::make_shared<RenderObject>();
		m_RenderObjects.push_back(stele);
		stele->m_mesh = m_rhi->CreateSteleMesh();
		stele->m_material = m_rhi->GetMaterial(stele->m_mesh->GetName());


		m_rhi->ExecuteCommandLists();
		m_rhi->Flush();
	}

	void RenderEngine::Update()
	{

		m_rhi->Update(m_RenderObjects[0]->m_mesh, m_RenderObjects[0]->m_material);

		m_rhi->Flush();
	}

	void RenderEngine::OnResize(uint16_t width, uint16_t height)
	{
		m_swapChainWidth = width;
		m_swapChainHeight = height;
		m_rhi->OnResize();
	}
}