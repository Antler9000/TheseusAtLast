#define IMGUI_DEFINE_MATH_OPERATORS
#include "Podo.h"
#include "State.h"
#include "Timer.h"
#include "Debug.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3dx12_root_signature.h>
#include <d3dx12_barriers.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <windows.h>
#include <dxgi.h>
#include <pix3.h>
#include <string>
#include <cstdlib>

using namespace DirectX;
using std::wstring;

void Podo::UpdateTimers()
{
	m_worldTimerTotal.Update();
	m_worldTimerFrame.Update();

	m_worldTimerFrame.Mark();
}

void Podo::UpdateWorld()
{
	if (IsWorldStopped() == true)
	{
		return;
	}
}

void Podo::UpdateRender()
{
	if (IsRenderStopped() == true)
	{
		return;
	}

	{
		PIXScopedEvent(PIX_COLOR_INDEX(4), L"CPU: 4. Reset Command List");

		FlushCommandQueue();
		ThrowIfFailed(m_commandAllocator->Reset());
		ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
	}

	{
		PIXScopedEvent(m_commandList.Get(), PIX_COLOR_INDEX(11), L"GPU: 1. Frame Time");

		{
			PIXScopedEvent(PIX_COLOR_INDEX(5), L"CPU: 5. Bind Resources");
			PIXScopedEvent(m_commandList.Get(), PIX_COLOR_INDEX(12), L"GPU: 2. Bind Resources");

			ID3D12DescriptorHeap* descriptorHeaps[] = { m_descriptorHeapCBVSRVUAV.Get() };
			m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

			CD3DX12_RESOURCE_BARRIER barrierPresentToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
				m_screenBackBuffers[m_screenBackBufferIndex].Get(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			m_commandList->ResourceBarrier(1, &barrierPresentToRenderTarget);

			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandleRTV = m_descriptorHeapRTVCpuStartHandle;
			cpuHandleRTV.Offset(m_screenBackBufferIndex, m_descriptorHeapRTVIncrementSize);

			m_commandList->OMSetRenderTargets(1, &cpuHandleRTV, true, &m_descriptorHeapDSVCpuStartHandle);
			m_commandList->RSSetViewports(1, &m_screenViewPort);
			m_commandList->RSSetScissorRects(1, &m_screenScissorRectangle);

			FLOAT sinZeroToOne = (XMScalarSin(static_cast<float>(m_worldTimerTotal.GetTimeMilli()) / 1000) + 1) / 2;
			FLOAT pTestColor[4] = { sinZeroToOne, sinZeroToOne, sinZeroToOne, 1.0f };
			m_commandList->ClearRenderTargetView(cpuHandleRTV, pTestColor, 0, nullptr);
			m_commandList->ClearDepthStencilView(
				m_descriptorHeapDSVCpuStartHandle,
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
				1.0f,
				0,
				0,
				nullptr
			);
		}

		{
			PIXScopedEvent(PIX_COLOR_INDEX(6), L"CPU: 6. Draw Scene");
			PIXScopedEvent(m_commandList.Get(), PIX_COLOR_INDEX(13), L"GPU: 3. Draw Scene");
		}

		{
			PIXScopedEvent(PIX_COLOR_INDEX(7), L"CPU: 7. Draw GUI");
			PIXScopedEvent(m_commandList.Get(), PIX_COLOR_INDEX(14), L"GPU: 4. Draw GUI");

			UpdateGUI();
		}

		{
			PIXScopedEvent(PIX_COLOR_INDEX(8), L"CPU: 8. Unbind Resources");
			PIXScopedEvent(m_commandList.Get(), PIX_COLOR_INDEX(15), L"GPU: 5. Unbind Resources");

			CD3DX12_RESOURCE_BARRIER barrierRenderTargetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
				m_screenBackBuffers[m_screenBackBufferIndex].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			);
			m_commandList->ResourceBarrier(1, &barrierRenderTargetToPresent);
		}
	}

	{
		PIXScopedEvent(PIX_COLOR_INDEX(9), L"CPU: 9. Submit Command List");

		ThrowIfFailed(m_commandList->Close());
		ID3D12CommandList* commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	}

	{
		PIXScopedEvent(PIX_COLOR_INDEX(10), L"CPU: 10. Present");

		if (m_optionVSync.IsActive() == true)
		{
			ThrowIfFailed(m_screenSwapChain->Present(1, 0));
		}
		else
		{
			ThrowIfFailed(m_screenSwapChain->Present(0, m_optionTearing.IsActive() ? DXGI_PRESENT_ALLOW_TEARING : 0));
		}

		m_screenBackBufferIndex = m_screenSwapChain->GetCurrentBackBufferIndex();
	}
}

void Podo::UpdateGUI()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiViewport* pImGuiViewPort = ImGui::GetMainViewport();
	ImVec2 imGuiCenterPos = pImGuiViewPort->GetCenter();

	m_imGuiSpacingSize = ImVec2(0.0f, 10.0f) * m_optionGUI.GetMasterScale();
	m_imGuiSmallButtonSize = ImVec2(120.0f, 40.0f) * m_optionGUI.GetMasterScale();
	m_imGuiMediumButtonSize = ImVec2(240.0f, 40.0f) * m_optionGUI.GetMasterScale();
	m_imGuiLargeButtonSize = ImVec2(360.0f, 40.0f) * m_optionGUI.GetMasterScale();

	switch (m_engineStatePresent)
	{
		case ENGINE_STATE_LOADING:
		{
			UpdateGUILoading(pImGuiViewPort, imGuiCenterPos);

			break;
		}

		case ENGINE_STATE_RUNTIME:
		{
			UpdateGUIRuntime(pImGuiViewPort, imGuiCenterPos);

			break;
		}

		case ENGINE_STATE_MENU:
		{
			UpdateGUIMenu(pImGuiViewPort, imGuiCenterPos);

			break;
		}
	}

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList.Get());
}

void Podo::UpdateGUILoading(ImGuiViewport* pImGuiViewPort, ImVec2 imGuiCenterPos)
{
	ImGuiWindowFlags loadingGuiFlag = m_imGuiBasicFlag | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

	ImVec2 pos = ImVec2(
		imGuiCenterPos.x,
		pImGuiViewPort->Pos.y + pImGuiViewPort->Size.y * 0.9f
	);

	ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f) * m_optionGUI.GetMasterScale(), ImGuiCond_Always);

	ImGui::Begin("Loading", nullptr, loadingGuiFlag);

	bool startButtonClicked = ImGui::Button("Click here to start", m_imGuiLargeButtonSize);
	if (startButtonClicked == true)
	{
		m_engineStatePresent = ENGINE_STATE_RUNTIME;
		InputReset();
		WorldTimersReset();
		WorldTimersStart();
	}

	ImGui::End();
}

void Podo::UpdateGUIRuntime(ImGuiViewport* pImGuiViewPort, ImVec2 imGuiCenterPos)
{
	ImGuiWindowFlags loadingGuiFlag = m_imGuiBasicFlag | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

	ImGui::SetNextWindowPos(ImVec2(0.0f,0.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f) * m_optionGUI.GetMasterScale(), ImGuiCond_Always);

	ImGui::Begin("Loading", nullptr, loadingGuiFlag);

	bool menuButtonClicked = ImGui::Button("Menu", m_imGuiSmallButtonSize);
	bool escKeyPressed = ImGui::IsKeyPressed(ImGuiKey_Escape, false);
	if (menuButtonClicked == true || escKeyPressed == true)
	{
		m_engineStatePresent = ENGINE_STATE_MENU;
		WorldTimersStop();
	}

	ImGui::End();
}

void Podo::UpdateGUIMenu(ImGuiViewport* pImGuiViewPort, ImVec2 imGuiCenterPos)
{
	ImGui::SetNextWindowPos(imGuiCenterPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(700.0f, 550.0f) * m_optionGUI.GetMasterScale(), ImGuiCond_Always);

	ImGui::Begin("Menu", nullptr, m_imGuiBasicFlag);

	bool backButtonClicked = ImGui::Button("Back", m_imGuiSmallButtonSize);
	bool escKeyPressed = ImGui::IsKeyPressed(ImGuiKey_Escape, false);
	if (backButtonClicked == true || escKeyPressed == true)
	{
		OptionSave();
		m_engineStatePresent = ENGINE_STATE_RUNTIME;
		WorldTimersStart();
	}

	ImGui::SameLine();

	bool exitToWindowButtonClicked = ImGui::Button("Exit", m_imGuiSmallButtonSize);
	if (exitToWindowButtonClicked == true)
	{
		OptionSave();
		DestroyWindow(m_hWnd);
	}

	bool previousFullScreenState = m_optionFullScreen.IsActive();
	bool previousHDRState = m_optionHDR.IsActive();
	int previousGUIState = m_optionGUI.masterSize;

	ImGui::Dummy(m_imGuiSpacingSize);

	ImGui::Text("Display");
	ImGui::BeginDisabled(m_optionFullScreen.IsSupported() == false);
	ImGui::Checkbox("Full Screen", &m_optionFullScreen.userEnabled);
	ImGui::EndDisabled();
	ImGui::BeginDisabled(m_optionVSync.IsSupported() == false);
	ImGui::Checkbox("VSync", &m_optionVSync.userEnabled);
	ImGui::EndDisabled();
	ImGui::SameLine();
	ImGui::BeginDisabled(m_optionHDR.IsSupported() == false);
	ImGui::Checkbox("HDR", &m_optionHDR.userEnabled);
	ImGui::EndDisabled();

	ImGui::Dummy(m_imGuiSpacingSize);
	ImGui::Separator();

	ImGui::Text("Graphics");
	ImGui::BeginDisabled(m_optionRayTracing.IsSupported() == false);
	ImGui::Checkbox("Ray Tracing", &m_optionRayTracing.userEnabled);
	ImGui::EndDisabled();
	ImGui::BeginDisabled(m_optionMeshShader.IsSupported() == false);
	ImGui::Checkbox("Mesh Shader", &m_optionMeshShader.userEnabled);
	ImGui::EndDisabled();

	ImGui::Dummy(m_imGuiSpacingSize);
	ImGui::Separator();

	ImGui::Text("GUI");
	const char* masterSizeStringArray[] = { "50%", "75%", "100%", "125%", "150%" };
	int selectedIndex = (m_optionGUI.masterSize - 50) / 25;
	if (ImGui::Combo("Master Size", &selectedIndex, masterSizeStringArray, _countof(masterSizeStringArray)) == true)
	{
		m_optionGUI.masterSize = 50 + 25 * selectedIndex;
	}

	bool nowFullScreenState = m_optionFullScreen.IsActive();
	bool nowHDRState = m_optionHDR.IsActive();
	int nowGUIState = m_optionGUI.masterSize;

	if (previousFullScreenState != nowFullScreenState)
	{
		m_needResetScreenMode = true;
	}
	if (previousHDRState != nowHDRState)
	{
		m_needResetHDR = true;
	}
	if (previousGUIState != nowGUIState)
	{
		m_needResetGUI = true;
	}

	ImGui::End();
}

void Podo::UpdateCaption()
{
#ifdef _DEBUG
	static Timer worldTimerCaption;

	worldTimerCaption.Update();

	//NOTE: SetWindowTextW를 너무 자주 호출하면 시스템 부하로 인해 윈도우 전체가 먹통이 되니 반복에 텀을 주자
	if (worldTimerCaption.GetTimeMilli() > 100.0f)
	{

		int fps = (m_worldTimerFrame.GetTimeMilli() != 0) ? static_cast<int>(1000 / m_worldTimerFrame.GetTimeMilli()) : 0;

		wstring caption = std::format(
			L"{} (월드 경과 시간: {:06.1F} s / 월드 프레임 시간: {:0.4f} ms / FPS: {:3d} fps / 마우스 위치: {:04d} p, {:04d} p / 스크롤 각도: {:04d} unit)",
			m_pAppName,
			m_worldTimerTotal.GetTimeMilli() / 1000,
			m_worldTimerFrame.GetTimeMilli(),
			(fps > 999) ? 999 : fps,
			m_inputMousePositionClient.x,
			m_inputMousePositionClient.y,
			m_inputScrollDelta
		);

		SetWindowTextW(m_hWnd, caption.c_str());

		worldTimerCaption.Mark();
	}
#endif
}