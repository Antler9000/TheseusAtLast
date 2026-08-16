#define NOMINMAX
#include "Podo.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <windows.h>

void Podo::Close()
{
	CloseFenceEvent();
	CloseImGui();
	CloseWindowOnException();
}

void Podo::CloseImGui()
{
	if (m_imGuiInitialized == false)
	{
		return;
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	m_imGuiDescriptorHeapAllocator.Destroy();

	m_imGuiInitialized = false;
}

void Podo::CloseFenceEvent()
{
	CloseHandle(m_fenceEvent);
	m_fenceEvent = nullptr;
}

//NOTE:	예외에 의해 소멸되는 경우에 창을 없애주며, 이후 소멸된 객체의 HandleMessage(..) 메서드가 호출되지 않도록 해줌
//		DestoryWindow(..)로 들어간 WM_QUIT 메시지가 남아 이후 예외 메시지 박스를 바로 종료시키는 것을 막기 위해 메시지 펌프로 소진시킴
void Podo::CloseWindowOnException()
{
	if (m_hWnd != nullptr)
	{
		DestroyWindow(m_hWnd);

		MSG msg = { };
		while (PeekMessage(&msg, nullptr, WM_QUIT, WM_QUIT, PM_REMOVE))
		{

		}
	}
}