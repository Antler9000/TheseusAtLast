#pragma once
#include "Asset.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

struct Object
{
	const Asset* asset = nullptr;

	DirectX::XMFLOAT4X4 worldMatrix =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;

	D3D12_GPU_DESCRIPTOR_HANDLE constantBufferViewGPUHandle = {};

	void XM_CALLCONV SetWorldSpace(DirectX::FXMVECTOR position, DirectX::FXMVECTOR direction, DirectX::FXMVECTOR up)
	{
		DirectX::XMVECTOR zAxis	= DirectX::XMVector3Normalize(direction);
		DirectX::XMVECTOR xAxis	= DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, zAxis));
		DirectX::XMVECTOR yAxis	= DirectX::XMVector3Cross(zAxis, xAxis);

		DirectX::XMMATRIX worldMatrixXMMatrix
		(
			DirectX::XMVectorSetW(xAxis, 0.0f),
			DirectX::XMVectorSetW(yAxis, 0.0f),
			DirectX::XMVectorSetW(zAxis, 0.0f),
			DirectX::XMVectorSetW(position, 1.0f)
		);

		DirectX::XMStoreFloat4x4(&worldMatrix, worldMatrixXMMatrix);
	}

	void CreateConstantBuffer(ID3D12Device* device)
	{
		constexpr UINT constantBufferSize = (sizeof(DirectX::XMFLOAT4X4) + 255) & ~255;

		D3D12_HEAP_PROPERTIES heapProperties	= CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc		= CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

		ThrowIfFailed
		(
			device->CreateCommittedResource
			(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(constantBuffer.ReleaseAndGetAddressOf())
			)
		);
	}

	void XM_CALLCONV UpdateWorldViewProjection(DirectX::FXMMATRIX viewProjectionMatrix)
	{
		DirectX::XMMATRIX worldMatrixXM = DirectX::XMLoadFloat4x4(&worldMatrix);
		DirectX::XMMATRIX transposedWorldViewProjectionMatrix = DirectX::XMMatrixTranspose(worldMatrixXM * viewProjectionMatrix);

		DirectX::XMFLOAT4X4 constantData;
		DirectX::XMStoreFloat4x4(&constantData, transposedWorldViewProjectionMatrix);

		void* mappedData = nullptr;
		D3D12_RANGE readRange = { 0, 0 };

		ThrowIfFailed(constantBuffer->Map(0, &readRange, &mappedData));
		std::memcpy(mappedData, &constantData, sizeof(constantData));
		constantBuffer->Unmap(0, nullptr);
	}
};