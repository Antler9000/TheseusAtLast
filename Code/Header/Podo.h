#pragma once
#define NOMINMAX
#include "BaseApp.h"
#include "State.h"
#include "Option.h"
#include "Timer.h"
#include "Alloc.h"
#include "imgui.h"
#include <d3dx12_root_signature.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgi1_4.h>
#include <dxgi.h>
#include <dxgicommon.h>
#include <dxgiformat.h>
#include <wrl/client.h>
#include <windows.h>
#include <string>
#include <fstream>

class Podo : public BaseApp<Podo>
{
public:

	Podo(HINSTANCE hInstance, int nCmdShow) : BaseApp(L"Podo Nature Engine", hInstance, nCmdShow)
	{
		OptionRestore();

		WorldTimersStop();

		Reset();
	}

	~Podo()
	{
		OptionSave();

		FlushCommandQueue();
		
		Close();
	}

	int RunMessageLoop()
	{
		MSG msg = { };

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != FALSE)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else if (NeedReset() == true)
			{
				Reset();
			}
			else if (NeedResetInterfaces() == true)
			{
				ResetInterfaces();
			}
			else
			{
				if (IsUpdateStopped() == true)
				{
					Sleep(100);
				}
				else
				{
					Update();
				}
			}
		}

		return (int)msg.wParam;
	}

	//NOTE: 이 메서드는 BaseApp에 작석된 WindowProcedure 정적 메서드에서 호출함
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	void Reset();
	void ResetScreenMode();
	void ResetFullScreenMode();
	void ResetWindowMode();
	void ResetInterfaces();
	void ResetFactory();
	void ResetAdapterAndOutput();
	bool ResetOutput(IDXGIAdapter3* pAdapter);
	void ResetDevice();
	void ResetFence();
	void ResetFenceEvent();
	void ResetCommandQueue();
	void ResetCommandAllocator();
	void ResetCommandList();
	void ResetFormatSupport();
	void ResetHDRSwapChainSupport();
	void ResetSwapChain();
	void ResetBackBufferInfo();
	void ResetViewPort();
	void ResetScissorRectangle();
	void ResetDepthStencilBuffer();
	void ResetDescriptorHeapRTV();
	void ResetDescriptorHeapDSV();
	void ResetDescriptorHeapCBVSRVUAV();
	void ResetRTV();
	void ResetDSV();
	void ResetCBVSRVUAV();
	void ResetImGui();

	void Close();
	void CloseFenceEvent();
	void CloseImGui();
	void CloseWindowOnException();

	void Update();
	void UpdateTimers();
	void UpdateWorld();
	void UpdateRender();
	void UpdateGUI();
	void UpdateGUILoading(ImGuiViewport* pImGuiViewPort, ImVec2 imGuiCenterPos);
	void UpdateGUIRuntime(ImGuiViewport* pImGuiViewPort, ImVec2 imGuiCenterPos);
	void UpdateGUIMenu(ImGuiViewport* pImGuiViewPort, ImVec2 imGuiCenterPos);
	void UpdateCaption();

	void InputMouseMove(WPARAM wParam, LPARAM lParam);
	void InputMouseLeftButtonDown(WPARAM wParam, LPARAM lParam);
	void InputMouseLeftButtonUp(WPARAM wParam, LPARAM lParam);
	void InputMouseRightButtonDown(WPARAM wParam, LPARAM lParam);
	void InputMouseRightButtonUp(WPARAM wParam, LPARAM lParam);
	void InputMouseWheelScroll(WPARAM wParam, LPARAM lParam);
	void InputKeyboardDown(WPARAM wParam, LPARAM lParam);

private:

	void OptionSave();
	void OptionRestore();
	bool OptionReadBool(std::ifstream& fin, std::string optionName, bool& outOptionEnabled);
	bool OptionReadInt(std::ifstream& fin, std::string optionName, int& outOptionValue);

	void FlushCommandQueue();
	void SaveWindow();

private:

	//NOTE: ImGui에 넘겨주는 콜백 함수 속에서 기능해야 하므로 static으로 둠
	static inline		ImGuiDescriptorHeapAllocator	m_imGuiDescriptorHeapAllocator		= {};

	static constexpr	UINT							m_screenBackBufferCount				= 2;
	static constexpr	DXGI_FORMAT						m_screenBackBufferFormatSDR			= DXGI_FORMAT_R8G8B8A8_UNORM;
	static constexpr	DXGI_FORMAT						m_screenBackBufferFormatHDR			= DXGI_FORMAT_R10G10B10A2_UNORM;
	static constexpr	DXGI_COLOR_SPACE_TYPE			m_screenBackBufferColorSpaceSDR		= DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
	static constexpr	DXGI_COLOR_SPACE_TYPE			m_screenBackBufferColorSpaceHDR		= DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
	static constexpr	DXGI_FORMAT						m_screenDepthStencilBufferFormat	= DXGI_FORMAT_D24_UNORM_S8_UINT;

	static constexpr	UINT							m_imGuiDescriptorHeapCapacity		= 64;
	static constexpr	ImGuiWindowFlags				m_imGuiBasicFlag					= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
																							| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

	static constexpr	unsigned int					m_inputDragThresholdDist			= 20;

private:

	template <typename Interface>
	using ComPtr = Microsoft::WRL::ComPtr<Interface>;

	bool								NeedReset() const									{ return m_needResetScreenMode; }
	bool								m_needResetScreenMode								= false;
	bool								NeedResetInterfaces() const							{ return (m_dxgiFactory->IsCurrent() == FALSE) || m_needResetInterfaces; }
	bool								m_needResetInterfaces								= false;

	ComPtr<IDXGIFactory6>				m_dxgiFactory;													//NOTE: (기본) 성능순 어댑터 획득
	ComPtr<IDXGIAdapter3>				m_dxgiAdapter;													//NOTE: (기본) 자원의 메모리 상주성 관리
	ComPtr<IDXGIOutput>					m_dxgiOutput;
	ComPtr<IDXGIOutput6>				m_dxgiOutput6;													//NOTE: (옵션) HDR 모니터 정보 획득
	DXGI_OUTPUT_DESC1					m_dxgiOutputDesc									= {};		//NOTE: (옵션) HDR 모니터 정보 획득
	
	ComPtr<ID3D12Device>				m_device;
	ComPtr<ID3D12Device2>				m_device2;														//NOTE: (옵션) 메시 셰이더
	ComPtr<ID3D12Device5>				m_device5;														//NOTE: (옵션) 레이 트레이싱
	
	ComPtr<ID3D12Fence>					m_fence;
	UINT64								m_fenceCurrent										= 0;
	HANDLE								m_fenceEvent										= nullptr;
	
	ComPtr<ID3D12CommandQueue>			m_commandQueue;
	ComPtr<ID3D12CommandAllocator>		m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_commandList;
	ComPtr<ID3D12GraphicsCommandList4>	m_commandList4;													//NOTE: (옵션) 레이 트레이싱
	ComPtr<ID3D12GraphicsCommandList6>	m_commandList6;													//NOTE: (옵션) 메시 셰이더 생성
	
	ComPtr<IDXGISwapChain3>				m_screenSwapChain;												//NOTE: (기본) 백 버퍼 인덱스 추적
	ComPtr<ID3D12Resource>				m_screenBackBuffers[m_screenBackBufferCount];
	UINT								m_screenBackBufferIndex								= 0;
	UINT								m_screenBackBufferWidth								= 0;
	UINT								m_screenBackBufferHeight							= 0;
	float								m_screenBackBufferAspectRatio						= 0.0f;
	D3D12_VIEWPORT						m_screenViewPort									= {};
	D3D12_RECT							m_screenScissorRectangle							= {};
	ComPtr<ID3D12Resource>				m_screenDepthStencilBuffer;
	
	UINT								m_descriptorHeapCBVCount							= 0;
	UINT								m_descriptorHeapSRVCount							= 0;
	UINT								m_descriptorHeapUAVCount							= 0;
	UINT								m_descriptorHeapRTVIncrementSize					= 0;
	UINT								m_descriptorHeapDSVIncrementSize					= 0;
	UINT								m_descriptorHeapCBVSRVUAVIncrementSize				= 0;
	ComPtr<ID3D12DescriptorHeap>		m_descriptorHeapRTV;
	ComPtr<ID3D12DescriptorHeap>		m_descriptorHeapDSV;
	ComPtr<ID3D12DescriptorHeap>		m_descriptorHeapCBVSRVUAV;
	CD3DX12_CPU_DESCRIPTOR_HANDLE		m_descriptorHeapRTVCpuStartHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE		m_descriptorHeapDSVCpuStartHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE		m_descriptorHeapCBVSRVUAVSCpuStartHandleForImGui;
	CD3DX12_GPU_DESCRIPTOR_HANDLE		m_descriptorHeapCBVSRVUAVSGpuStartHandleForImGui;
	CD3DX12_CPU_DESCRIPTOR_HANDLE		m_descriptorHeapCBVSRVUAVSCpuStartHandleForRender;
	CD3DX12_GPU_DESCRIPTOR_HANDLE		m_descriptorHeapCBVSRVUAVSGpuStartHandleForRender;

	bool								m_imGuiInitialized									= false;
	ImVec2								m_imGuiSpacingSize									= ImVec2(0.0f, 10.0f);
	ImVec2								m_imGuiSmallButtonSize								= ImVec2(120.0f, 40.0f);
	ImVec2								m_imGuiMediumButtonSize								= ImVec2(240.0f, 40.0f);
	ImVec2								m_imGuiLargeButtonSize								= ImVec2(360.0f, 40.0f);

	bool								IsUpdateStopped() const								{ return (IsWorldStopped() && IsRenderStopped()); }
	bool								IsWorldStopped() const								{ return (m_engineStatePresent != ENGINE_STATE_RUNTIME); }
	bool								IsRenderStopped() const								{ return m_isWindowResizing || m_isWindowMoving || m_isWindowMinimized; }
	bool								m_isWindowResizing									= false;
	bool								m_isWindowMoving									= false;
	bool								m_isWindowMinimized									= false;

	void								WorldTimersReset()									{ m_worldTimerTotal.Reset(); m_worldTimerFrame.Reset(); }
	void								WorldTimersStop()									{ m_worldTimerTotal.Stop(); m_worldTimerFrame.Stop(); }
	void								WorldTimersStart()									{ m_worldTimerTotal.Start(); m_worldTimerFrame.Start(); }
	Timer								m_worldTimerTotal;
	Timer								m_worldTimerFrame;

	EngineState							m_engineStatePresent								= ENGINE_STATE_LOADING;

	void								InputReset()										{ m_inputMousePositionClient = { 0,0 }; m_inputMouseClickedPositionClient = { 0, 0 }; m_inputIsClicked = false; m_inputScrollDelta = 0; }
	POINT								m_inputMousePositionClient							= { 0, 0 };
	POINT								m_inputMouseClickedPositionClient					= { 0, 0 };
	bool								m_inputIsClicked									= false;
	int									m_inputScrollDelta									= 0;

private:

	OptionFullScreen					m_optionFullScreen;
	OptionWindowSave					m_optionWindowSave;
	OptionVSync							m_optionVSync;
	OptionTearing						m_optionTearing;
	OptionHDR							m_optionHDR;
	OptionRayTracing					m_optionRayTracing;
	OptionMeshShader					m_optionMeshShader;
	OptionGUI							m_optionGUI;
};