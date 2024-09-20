#include <array.h>
#include <D3D12Mesh.h>
#include <D3D12RHI.h>
#include <D3D12RHIPrivate.h>
#include <D3D12Material.h>
#include <Math/Vector.h>

namespace GameEngine
{
	namespace Render::HAL
	{
		using namespace Core;

		D3D12RHI::D3D12RHI() :
			m_d3d12Private(std::make_shared<D3D12RHIPrivate>())
		{
			m_d3d12Private->EnableDebugLayer();
		}

		void D3D12RHI::Init()
		{
			m_d3d12Private->Init();
		}

		void D3D12RHI::Update(Mesh::Ptr mesh, Material::Ptr material)
		{
			m_d3d12Private->Update(mesh, material);
		}

		
		Mesh::Ptr D3D12RHI::CreateSteleMesh()
		{
			std::vector<Vertex> vertices =
			{
				// Rectangle base
				{ Math::Vector3f(-1.0f, -2.0f, -0.5f), Math::Vector4f((float*)&DirectX::Colors::Turquoise) },
				{ Math::Vector3f(-1.0f, -2.0f, +0.5f), Math::Vector4f((float*)&DirectX::Colors::Turquoise) },
				{ Math::Vector3f(+1.0f, -2.0f, +0.5f), Math::Vector4f((float*)&DirectX::Colors::Transparent) },
				{ Math::Vector3f(+1.0f, -2.0f, -0.5f), Math::Vector4f((float*)&DirectX::Colors::Transparent) },
				{ Math::Vector3f(-1.0f, +2.0f, -0.5f), Math::Vector4f((float*)&DirectX::Colors::DarkBlue) },
				{ Math::Vector3f(-1.0f, +2.0f, +0.5f), Math::Vector4f((float*)&DirectX::Colors::DarkBlue) },
				{ Math::Vector3f(+1.0f, +2.0f, +0.5f), Math::Vector4f((float*)&DirectX::Colors::BlueViolet) },
				{ Math::Vector3f(+1.0f, +2.0f, -0.5f), Math::Vector4f((float*)&DirectX::Colors::BlueViolet) },
				// Triangle top
				{ Math::Vector3f(0.0f, +3.0f, 0.0f), Math::Vector4f((float*)&DirectX::Colors::Fuchsia) }
			};

			std::vector<uint16_t> indices =
			{
				// Rectangle base
				0, 1, 2,  0, 2, 3,  // Bottom face
				4, 7, 6,  4, 6, 5,  // Top face
				0, 4, 5,  0, 5, 1,  // Left face
				2, 6, 7,  2, 7, 3,  // Right face
				0, 3, 7,  0, 7, 4,  // Front face
				1, 5, 6,  1, 6, 2,  // Back face
				// Triangle top
				4, 8, 5,  // Front triangle
				5, 8, 6,  // Right triangle
				6, 8, 7,  // Back triangle
				7, 8, 4   // Left triangle
			};

			return m_d3d12Private->CreateMesh(vertices.data(), vertices.size(), sizeof(Vertex), indices.data(), indices.size(), sizeof(uint16_t));
		}


		Material::Ptr D3D12RHI::GetMaterial(const std::string& name)
		{
			return m_d3d12Private->GetMaterial(name);
		}

		void D3D12RHI::ExecuteCommandLists()
		{
			m_d3d12Private->ExecuteCommandLists();
		}

		void D3D12RHI::Flush()
		{
			m_d3d12Private->FlushCommandQueue();
		}

		void D3D12RHI::OnResize()
		{
			m_d3d12Private->OnResize();
		}
	}
}