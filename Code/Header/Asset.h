#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <ResourceUploadBatch.h>
#include <BufferHelpers.h>
#include <vector>
#include <cstdint>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
};

struct Asset
{
	std::vector<Vertex>		vertices;
	std::vector<uint32_t>	indices;

	Microsoft::WRL::ComPtr<ID3D12Resource>	vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>	indexBuffer;

	D3D12_VERTEX_BUFFER_VIEW	vertexBufferView	= {};
	D3D12_INDEX_BUFFER_VIEW		indexBufferView		= {};

	void CreateBuffers(ID3D12Device* device, DirectX::ResourceUploadBatch& resourceUpload)
	{
		ThrowIfFailed
		(
			DirectX::CreateStaticBuffer
			(
				device,
				resourceUpload,
				vertices,
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				vertexBuffer.ReleaseAndGetAddressOf()
			)
		);

		ThrowIfFailed
		(
			DirectX::CreateStaticBuffer
			(
				device,
				resourceUpload,
				indices,
				D3D12_RESOURCE_STATE_INDEX_BUFFER,
				indexBuffer.ReleaseAndGetAddressOf()
			)
		);
	}


	void UpdateBufferViews()
	{
		vertexBufferView = {};
		indexBufferView = {};

		if (vertexBuffer.Get() != nullptr)
		{
			vertexBufferView =
			{
				vertexBuffer->GetGPUVirtualAddress(),
				static_cast<UINT>(vertices.size() * sizeof(Vertex)),
				sizeof(Vertex)
			};
		}

		if (indexBuffer.Get() != nullptr)
		{
			indexBufferView =
			{
				indexBuffer->GetGPUVirtualAddress(),
				static_cast<UINT>(indices.size() * sizeof(uint32_t)),
				DXGI_FORMAT_R32_UINT
			};
		}
	}
};