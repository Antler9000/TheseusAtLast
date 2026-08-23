#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>
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
};