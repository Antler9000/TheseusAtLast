#include "Podo.h"
#include "Option.h"
#include "Alloc.h"
#include "Debug.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12sdklayers.h>
#include <d3dx12_root_signature.h>
#include <d3dx12_core.h>
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxgi1_6.h>
#include <dxgi1_5.h>
#include <dxgi1_4.h>
#include <dxgi1_3.h>
#include <dxgi1_2.h>
#include <dxgi.h>
#include <dxgicommon.h>
#include <wrl/client.h>
#include <windows.h>
#include <string>
#include <stdexcept>

using Microsoft::WRL::ComPtr;
using std::wstring;

void Podo::InitFactory()
{
	UINT factoryFlags = 0;

#ifdef _DEBUG
	factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

	//NOTE : 쿼리 출력 매개변수는 BOOL 타입이므로, bool 타입인 m_optionTearing.featureSupported를 인자로 사용하면 안 됨
	BOOL tearingQuery = FALSE;
	m_dxgiFactory->CheckFeatureSupport(
		DXGI_FEATURE_PRESENT_ALLOW_TEARING, 
		&tearingQuery,
		sizeof(tearingQuery)
	);
	m_optionTearing.featureSupported = tearingQuery;
}

void Podo::InitAdapterAndOutput()
{
	m_dxgiAdapter.Reset();

	ComPtr<IDXGIAdapter3> tempAdapter = nullptr;

	HRESULT result = S_OK;
	for (int i = 0; result != DXGI_ERROR_NOT_FOUND; i++)
	{
		result = m_dxgiFactory->EnumAdapterByGpuPreference(
			i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(tempAdapter.ReleaseAndGetAddressOf())
		);

		if (SUCCEEDED(result) == true)
		{
			if (InitOutput(tempAdapter.Get()) == true)
			{
				m_dxgiAdapter = tempAdapter;

				return;
			}
		}
	}

	throw std::runtime_error("can't find pAdapter that connected with most intersecting output");
}

bool Podo::InitOutput(IDXGIAdapter3* pAdapter)
{
	m_dxgiOutput.Reset();
	m_dxgiOutput6.Reset();

	ComPtr<IDXGIOutput> tempOutput = nullptr;
	m_optionHDR.outputSupported = false;

	HMONITOR targetMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);

	HRESULT enumResult = S_OK;
	for (int i = 0; enumResult != DXGI_ERROR_NOT_FOUND; i++)
	{
		enumResult = pAdapter->EnumOutputs(i, tempOutput.ReleaseAndGetAddressOf());

		if (SUCCEEDED(enumResult) == true)
		{
			DXGI_OUTPUT_DESC tempOutputDesc;
			tempOutput->GetDesc(&tempOutputDesc);

			if (tempOutputDesc.Monitor == targetMonitor)
			{
				m_dxgiOutput = tempOutput;

				HRESULT asResult = m_dxgiOutput.As(&m_dxgiOutput6);
				if (SUCCEEDED(asResult) == true)
				{
					m_dxgiOutput6->GetDesc1(&m_dxgiOutputDesc);
					if (m_dxgiOutputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709)
					{
						m_optionHDR.outputSupported = false;
					}
					else
					{
						m_optionHDR.outputSupported = true;
					}
				}

				return true;
			}
		}
	}

	return false;
}

void Podo::InitDevice()
{
	m_device5.Reset();
	m_device2.Reset();
	m_device.Reset();

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debug;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())));
	debug->EnableDebugLayer();
	debug->SetEnableGPUBasedValidation(true);
#endif

	ThrowIfFailed(
		D3D12CreateDevice(m_dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()))
	);

	m_optionMeshShader.deviceSupported = SUCCEEDED(m_device.As(&m_device2));
	m_optionRayTracing.deviceSupported = SUCCEEDED(m_device.As(&m_device5));

	m_optionMeshShader.featureSupported = false;
	if (m_optionMeshShader.deviceSupported == true)
	{	
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 meshShaderFeatureQuery = {};
		m_device2->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &meshShaderFeatureQuery, sizeof(meshShaderFeatureQuery));
		m_optionMeshShader.featureSupported = (meshShaderFeatureQuery.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED);
	}

	m_optionRayTracing.featureSupported = false;
	if (m_optionRayTracing.deviceSupported == true)
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 rayTracingFeatureQuery = {};
		m_device5->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &rayTracingFeatureQuery, sizeof(rayTracingFeatureQuery));
		m_optionRayTracing.featureSupported = (rayTracingFeatureQuery.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED);
	}
}

void Podo::InitFence()
{
	ThrowIfFailed(m_device->CreateFence(m_fenceCurrent, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
}

void Podo::InitFenceEvent()
{
	CloseFenceEvent();

	m_fenceEvent = CreateEventExW(
		nullptr,
		nullptr,
		0,
		EVENT_MODIFY_STATE | SYNCHRONIZE
	);

	ThrowIfNull(m_fenceEvent);
}

void Podo::InitCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask	= 0;
	ThrowIfFailed(
		m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf()))
	);
}

void Podo::InitCommandAllocator()
{
	ThrowIfFailed(
		m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			IID_PPV_ARGS(m_commandAllocator.ReleaseAndGetAddressOf())
		)
	);
}

void Podo::InitCommandList()
{
	m_commandList6.Reset();
	m_commandList4.Reset();
	m_commandList.Reset();

	ThrowIfFailed(
		m_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())
		)
	);

	m_optionRayTracing.commandListSupported = SUCCEEDED(m_commandList.As(&m_commandList4));
	m_optionMeshShader.commandListSupported = SUCCEEDED(m_commandList.As(&m_commandList6));
 
	m_commandList->Close();
}

void Podo::InitFormatSupport()
{
	D3D12_FEATURE_DATA_FORMAT_SUPPORT backBufferFormatSDRQuery = {
		m_screenBackBufferFormatSDR,
		D3D12_FORMAT_SUPPORT1_NONE,
		D3D12_FORMAT_SUPPORT2_NONE
	};
	D3D12_FEATURE_DATA_FORMAT_SUPPORT backBufferFormatHDRQuery = {
		m_screenBackBufferFormatHDR,
		D3D12_FORMAT_SUPPORT1_NONE,
		D3D12_FORMAT_SUPPORT2_NONE
	};
	D3D12_FEATURE_DATA_FORMAT_SUPPORT depthStencilFormatQuery = {
		m_screenDepthStencilBufferFormat,
		D3D12_FORMAT_SUPPORT1_NONE,
		D3D12_FORMAT_SUPPORT2_NONE
	};

	ThrowIfFailed(
		m_device->CheckFeatureSupport(
			D3D12_FEATURE_FORMAT_SUPPORT, &depthStencilFormatQuery, sizeof(depthStencilFormatQuery)
		)
	);
	ThrowIfFailed(
		m_device->CheckFeatureSupport(
			D3D12_FEATURE_FORMAT_SUPPORT, &backBufferFormatSDRQuery, sizeof(backBufferFormatSDRQuery)
		)
	);
	HRESULT hdrQueryResult = m_device->CheckFeatureSupport(
		D3D12_FEATURE_FORMAT_SUPPORT, &backBufferFormatHDRQuery, sizeof(backBufferFormatHDRQuery)
	);

	ThrowIfFalse(depthStencilFormatQuery.Support1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
	ThrowIfFalse(backBufferFormatSDRQuery.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
	if (SUCCEEDED(hdrQueryResult) == true)
	{
		m_optionHDR.formatSupported = ((backBufferFormatHDRQuery.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) != 0);
	}
	else
	{
		m_optionHDR.formatSupported = false;
	}
}

void Podo::InitHDRSwapChainSupport()
{
	m_screenSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc	= {};
	swapChainDesc.Width					= 0;
	swapChainDesc.Height				= 0;
	swapChainDesc.Format				= m_screenBackBufferFormatHDR;
	swapChainDesc.Stereo				= false;
	swapChainDesc.SampleDesc.Count		= 1;
	swapChainDesc.SampleDesc.Quality	= 0;
	swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount			= m_screenBackBufferCount;
	swapChainDesc.Scaling				= DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode				= DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags					= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT
										| (m_optionTearing.IsActive() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

	ComPtr<IDXGISwapChain1> tempSwapChain = nullptr;

	ThrowIfFailed(
		m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			m_hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			tempSwapChain.ReleaseAndGetAddressOf()
		)
	);

	ComPtr<IDXGISwapChain3> tempSwapChain3 = nullptr;

	ThrowIfFailed((tempSwapChain.As(&tempSwapChain3)));

	m_optionHDR.colorSpaceSupported = false;

	UINT colorSpaceHDRQuery = 0;
	HRESULT queryResult = tempSwapChain3->CheckColorSpaceSupport(m_screenBackBufferColorSpaceHDR, &colorSpaceHDRQuery);
	if (SUCCEEDED(queryResult) == true)
	{
		m_optionHDR.colorSpaceSupported = ((colorSpaceHDRQuery & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) != 0);
	}
}

void Podo::InitSavedOptions()
{
	OptionRestore();

	if (m_optionFullScreen.IsSupported() == false)
	{
		m_optionFullScreen.userEnabled = false;
	}

	if (m_optionHDR.IsSupported() == false)
	{
		m_optionHDR.userEnabled = false;
	}

	if (m_optionRayTracing.IsSupported() == false)
	{
		m_optionRayTracing.userEnabled = false;
	}

	if (m_optionMeshShader.IsSupported() == false)
	{
		m_optionMeshShader.userEnabled = false;
	}
}

void Podo::InitScreenMode()
{
	HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &monitorInfo);

	LONG monitorBaseX = monitorInfo.rcMonitor.left;
	LONG monitorBaseY = monitorInfo.rcMonitor.top;
	LONG monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	LONG monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	m_previousWindowWidth = monitorWidth * 2 / 3;
	m_previousWindowHeight = monitorHeight * 2 / 3;
	m_previousWindowPosX = monitorBaseX + (monitorWidth / 2) - (m_previousWindowWidth / 2);
	m_previousWindowPosY = monitorBaseY + (monitorHeight / 2) - (m_previousWindowHeight / 2);

	if (m_optionFullScreen.IsActive() == true)
	{
		ResetToFullScreenMode();
	}
	else
	{
		ResetToWindowMode();
	}
}

void Podo::InitSwapChain()
{
	m_screenSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width					= 0;
	swapChainDesc.Height				= 0;
	swapChainDesc.Format				= m_optionHDR.IsActive() ? m_screenBackBufferFormatHDR : m_screenBackBufferFormatSDR;
	swapChainDesc.Stereo				= false;
	swapChainDesc.SampleDesc.Count		= 1;
	swapChainDesc.SampleDesc.Quality	= 0;
	swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount			= m_screenBackBufferCount;
	swapChainDesc.Scaling				= DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode				= DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags					= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT
										| (m_optionTearing.IsActive() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

	ComPtr<IDXGISwapChain1> tempSwapChain = nullptr;

	ThrowIfFailed(
		m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			m_hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			tempSwapChain.ReleaseAndGetAddressOf()
		)
	);

	ThrowIfFailed(
		m_dxgiFactory->MakeWindowAssociation(
			m_hWnd,
			DXGI_MWA_NO_ALT_ENTER
		)
	);

	ThrowIfFailed(tempSwapChain.As(&m_screenSwapChain));

	if (m_optionHDR.IsActive() == true)
	{
		m_screenSwapChain->SetColorSpace1(m_screenBackBufferColorSpaceHDR);
	}
	else
	{
		m_screenSwapChain->SetColorSpace1(m_screenBackBufferColorSpaceSDR);
	}
}

void Podo::InitBackBufferInfo()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	m_screenSwapChain->GetDesc1(&swapChainDesc);
	m_screenBackBufferWidth = swapChainDesc.Width;
	m_screenBackBufferHeight = swapChainDesc.Height;
	m_screenBackBufferAspectRatio = (m_screenBackBufferHeight ? (static_cast<float>(m_screenBackBufferWidth) / m_screenBackBufferHeight) : 0);

	m_screenBackBufferIndex = m_screenSwapChain->GetCurrentBackBufferIndex();

	for (UINT i = 0; i < m_screenBackBufferCount; i++)
	{
		ThrowIfFailed(m_screenSwapChain->GetBuffer(i, IID_PPV_ARGS(m_screenBackBuffers[i].ReleaseAndGetAddressOf())));
	}
}

void Podo::InitViewPort()
{
	m_screenViewPort.TopLeftX	= 0.0f;
	m_screenViewPort.TopLeftY	= 0.0f;
	m_screenViewPort.Width		= FLOAT(m_screenBackBufferWidth);
	m_screenViewPort.Height		= FLOAT(m_screenBackBufferHeight);
	m_screenViewPort.MinDepth	= 0.0f;
	m_screenViewPort.MaxDepth	= 1.0f;
}

void Podo::InitScissorRectangle()
{
	m_screenScissorRectangle.left	= 0;
	m_screenScissorRectangle.top	= 0;
	m_screenScissorRectangle.right	= m_screenBackBufferWidth;
	m_screenScissorRectangle.bottom	= m_screenBackBufferHeight;
}

void Podo::InitDepthStencilBuffer()
{
	D3D12_RESOURCE_DESC depthStencilBufferDesc = {};
	depthStencilBufferDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilBufferDesc.Alignment			= 0;
	depthStencilBufferDesc.Width				= m_screenBackBufferWidth;
	depthStencilBufferDesc.Height				= m_screenBackBufferHeight;
	depthStencilBufferDesc.DepthOrArraySize		= 1;
	depthStencilBufferDesc.MipLevels			= 1;
	depthStencilBufferDesc.Format				= m_screenDepthStencilBufferFormat;
	depthStencilBufferDesc.SampleDesc.Count		= 1;
	depthStencilBufferDesc.SampleDesc.Quality	= 0;
	depthStencilBufferDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilBufferDesc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = m_screenDepthStencilBufferFormat;
	clearValue.DepthStencil.Depth	= 1.0f;
	clearValue.DepthStencil.Stencil	= 0;

	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(
		m_device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilBufferDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(m_screenDepthStencilBuffer.ReleaseAndGetAddressOf())
		)
	);
}

void Podo::InitDescriptorHeapRTV()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.NumDescriptors = m_screenBackBufferCount;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	ThrowIfFailed(
		m_device->CreateDescriptorHeap(
			&descriptorHeapDesc,
			IID_PPV_ARGS(m_descriptorHeapRTV.ReleaseAndGetAddressOf())
		)
	);

	m_descriptorHeapRTVIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_descriptorHeapRTVCpuStartHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeapRTV->GetCPUDescriptorHandleForHeapStart());
}

void Podo::InitDescriptorHeapDSV()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	ThrowIfFailed(
		m_device->CreateDescriptorHeap(
			&descriptorHeapDesc,
			IID_PPV_ARGS(m_descriptorHeapDSV.ReleaseAndGetAddressOf())
		)
	);

	m_descriptorHeapDSVIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_descriptorHeapDSVCpuStartHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeapDSV->GetCPUDescriptorHandleForHeapStart());
}

void Podo::InitDescriptorHeapCBVSRVUAV()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	UINT totalCount = m_descriptorHeapCBVCount + m_descriptorHeapSRVCount + m_descriptorHeapUAVCount;
	descriptorHeapDesc.NumDescriptors = (totalCount ? totalCount : 128);
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;

	ThrowIfFailed(
		m_device->CreateDescriptorHeap(
			&descriptorHeapDesc,
			IID_PPV_ARGS(m_descriptorHeapCBVSRVUAV.ReleaseAndGetAddressOf())
		)
	);

	//NOTE : ImGui가 SRV를 둘 곳을 고정적으로 남겨두고, 그 뒷부분부터 사용하기로 함
	m_descriptorHeapCBVSRVUAVIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_descriptorHeapCBVSRVUAVSCpuStartHandleForImGui = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeapCBVSRVUAV->GetCPUDescriptorHandleForHeapStart());
	m_descriptorHeapCBVSRVUAVSGpuStartHandleForImGui = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeapCBVSRVUAV->GetGPUDescriptorHandleForHeapStart());

	m_descriptorHeapCBVSRVUAVSCpuStartHandleForRender = m_descriptorHeapCBVSRVUAVSCpuStartHandleForImGui;
	m_descriptorHeapCBVSRVUAVSCpuStartHandleForRender.Offset(m_imGuiDescriptorHeapCapacity, m_descriptorHeapCBVSRVUAVIncrementSize);
	m_descriptorHeapCBVSRVUAVSGpuStartHandleForRender = m_descriptorHeapCBVSRVUAVSGpuStartHandleForImGui;
	m_descriptorHeapCBVSRVUAVSGpuStartHandleForRender.Offset(m_imGuiDescriptorHeapCapacity, m_descriptorHeapCBVSRVUAVIncrementSize);
}


void Podo::InitRTV()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandleRTV = m_descriptorHeapRTVCpuStartHandle;
	for (UINT i = 0; i < m_screenBackBufferCount; i++)
	{
		m_device->CreateRenderTargetView(m_screenBackBuffers[i].Get(), nullptr, cpuHandleRTV.Offset(i, m_descriptorHeapRTVIncrementSize));
	}
}

void Podo::InitDSV()
{
	m_device->CreateDepthStencilView(m_screenDepthStencilBuffer.Get(), nullptr, m_descriptorHeapDSVCpuStartHandle);
}

void Podo::InitCBVSRVUAV()
{

}

void Podo::InitImGui()
{
	CloseImGui();

	m_imGuiDescriptorHeapAllocator.Create(m_device.Get(), m_descriptorHeapCBVSRVUAV.Get(), m_imGuiDescriptorHeapCapacity);

	ImGui_ImplDX12_InitInfo initInfo = {};
	initInfo.Device				= m_device.Get();
	initInfo.CommandQueue		= m_commandQueue.Get();
	initInfo.NumFramesInFlight	= 1;
	initInfo.RTVFormat			= m_optionHDR.IsActive() ? m_screenBackBufferFormatHDR : m_screenBackBufferFormatSDR;

	initInfo.SrvDescriptorHeap = m_descriptorHeapCBVSRVUAV.Get();
	initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* pOutCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle)
		{ 
			return m_imGuiDescriptorHeapAllocator.Alloc(pOutCpuHandle, outGpuHandle);
		};
	initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
		{
		return m_imGuiDescriptorHeapAllocator.Free(cpuHandle, gpuHandle);
		};
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX12_Init(&initInfo);

	ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();
	float imGuiScale = 2.0f * m_optionGUI.GetMasterScale();
	style.ScaleAllSizes(imGuiScale);
	style.FontScaleDpi = imGuiScale;

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text]				= ImVec4(0.78f, 0.74f, 0.63f, 1.0f);
	colors[ImGuiCol_TextDisabled]		= ImVec4(0.38f, 0.38f, 0.34f, 1.0f);
	colors[ImGuiCol_FrameBg]			= ImVec4(0.105f, 0.110f, 0.100f, 1.0f);
	colors[ImGuiCol_FrameBgHovered]		= ImVec4(0.185f, 0.175f, 0.130f, 1.0f);
	colors[ImGuiCol_FrameBgActive]		= ImVec4(0.260f, 0.230f, 0.140f, 1.0f);
	colors[ImGuiCol_CheckMark]			= ImVec4(0.82f, 0.68f, 0.32f, 1.0f);
	colors[ImGuiCol_SliderGrab]			= ImVec4(0.58f, 0.50f, 0.30f, 1.0f);
	colors[ImGuiCol_SliderGrabActive]	= ImVec4(0.82f, 0.66f, 0.28f, 1.0f);
	colors[ImGuiCol_WindowBg]			= ImVec4(0.075f, 0.078f, 0.075f, 1.0f);
	colors[ImGuiCol_TitleBg]			= ImVec4(0.115f, 0.115f, 0.105f, 1.0f);
	colors[ImGuiCol_TitleBgActive]		= ImVec4(0.180f, 0.170f, 0.145f, 1.0f);
	colors[ImGuiCol_TitleBgCollapsed]	= ImVec4(0.060f, 0.062f, 0.060f, 1.0f);
	colors[ImGuiCol_Button]				= ImVec4(0.210f, 0.215f, 0.200f, 1.0f);
	colors[ImGuiCol_ButtonHovered]		= ImVec4(0.340f, 0.320f, 0.250f, 1.0f);
	colors[ImGuiCol_ButtonActive]		= ImVec4(0.470f, 0.380f, 0.180f, 1.0f);
	colors[ImGuiCol_Border]				= ImVec4(0.30f, 0.28f, 0.23f, 0.55f);
	colors[ImGuiCol_Separator]			= ImVec4(0.34f, 0.31f, 0.25f, 0.65f);
	colors[ImGuiCol_SeparatorHovered]	= ImVec4(0.45f, 0.38f, 0.34f, 0.78f);
	colors[ImGuiCol_SeparatorActive]	= ImVec4(0.55f, 0.45f, 0.43f, 0.90f);

	m_imGuiInitialized = true;
}

void Podo::InitTimers()
{
	WorldTimersReset();
	WorldTimersStop();
}