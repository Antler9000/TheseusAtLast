#include "Podo.h"
#include "imgui.h"
#include <windows.h>
#include <windowsx.h>
#include <format>
#include <string>
#include <cstdlib>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//NOTE:		WindowProc이 수행 중에는 해당 스레드의 메시지 큐에 쌓인 다른 메시지들을 처리하지 못하므로,
//			되도록 이 안에서는 짧은 로직만 수행하도록 하고, 긴 대기가 필요한 로직은 별도 스레드로 처리하자
LRESULT Podo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT imGuiHandled = ImGui_ImplWin32_WndProcHandler(m_hWnd, uMsg, wParam, lParam);
	if (imGuiHandled)
	{
		return 0;
	}

	switch (uMsg)
	{
		case WM_MOUSEMOVE:
		{
			ImGuiIO& io = ImGui::GetIO();

			if (io.WantCaptureMouse == false)
			{
				InputMouseMove(wParam, lParam);
			}

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			ImGuiIO& io = ImGui::GetIO();

			if (io.WantCaptureMouse == false)
			{
				InputMouseLeftButtonDown(wParam, lParam);
			}

			return 0;
		}

		case WM_LBUTTONUP:
		{
			ImGuiIO& io = ImGui::GetIO();

			if (io.WantCaptureMouse == false)
			{
				InputMouseLeftButtonUp(wParam, lParam);
			}

			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			ImGuiIO& io = ImGui::GetIO();

			if (io.WantCaptureMouse == false)
			{
				InputMouseRightButtonDown(wParam, lParam);
			}

			return 0;
		}

		case WM_RBUTTONUP:
		{
			ImGuiIO& io = ImGui::GetIO();

			if (io.WantCaptureMouse == false)
			{
				InputMouseRightButtonUp(wParam, lParam);
			}

			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			InputMouseWheelScroll(wParam, lParam);

			return 0;
		}

		case WM_KEYDOWN:
		{
			InputKeyboardDown(wParam, lParam);

			return 0;
		}

		case WM_SYSKEYDOWN:
		{
			//NOTE: ALT+ENTER을 이용한 전체 화면 모드 전환을 처리함
			if (wParam == VK_RETURN && (lParam & 0x40000000) == 0)
			{
				m_optionFullScreen.userEnabled = !m_optionFullScreen.userEnabled;
				m_needResetScreenMode = true;

				return 0;
			}

			return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		}

		case WM_SYSCHAR:
		{
			//NOTE: ALT+ENTER을 이용한 전체 화면 모드 전환시 윈도우 알림음이 안 나도록 함
			if (wParam == '\r')
			{
				return 0;
			}

			return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		}

		case WM_ENTERSIZEMOVE:
		{
			m_isWindowResizing = true;
			m_isWindowMoving = true;

			return 0;
		}

		case WM_EXITSIZEMOVE:
		{
			RECT rectClient = {};
			BOOL queryResult = GetClientRect(m_hWnd, &rectClient);
			if (queryResult != FALSE)
			{
				LONG widthClient = rectClient.right - rectClient.left;
				LONG heightClient = rectClient.bottom - rectClient.top;

				if (widthClient >= 10 && heightClient >= 10)
				{
					m_needResetScreenResolution = true;
					m_needResetAdapterAndOutput = true;
				}
			}

			m_isWindowResizing = false;
			m_isWindowMoving = false;

			return 0;
		}

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				return 0;
			}

			if (m_isWindowResizing == true)
			{
				return 0;
			}

			m_needResetScreenResolution = true;

			return 0;
		}

		case WM_MOVE:
		{
			if (IsIconic(m_hWnd) != FALSE)
			{
				return 0;
			}

			if (m_isWindowMoving == true)
			{
				return 0;
			}

			m_needResetAdapterAndOutput = true;

			return 0;
		}

		case WM_DISPLAYCHANGE:
		{
			m_needResetScreenMode = true;

			return 0;
		}

		case WM_ACTIVATE:
		{
			bool isMinimized = (HIWORD(wParam) != 0);
			if (isMinimized == true)
			{
				m_isWindowMinimized = true;
			}
			else
			{
				m_isWindowMinimized = false;
			}

			return 0;
		}
		
		//NOTE: ALT+F4 혹은 우상단 창닫기 버튼을 이용한 종료를 처리함
		case WM_CLOSE:
		{
			OptionSave();
			DestroyWindow(m_hWnd);

			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);

			return 0;
		}
	}

	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

void Podo::InputMouseMove(WPARAM wParam, LPARAM lParam)
{
	m_inputMousePositionClient.x = GET_X_LPARAM(lParam);
	m_inputMousePositionClient.y = GET_Y_LPARAM(lParam);
}

void Podo::InputMouseLeftButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (m_inputIsClicked == true)
	{
		return;
	}

	m_inputMouseClickedPositionClient.x = GET_X_LPARAM(lParam);
	m_inputMouseClickedPositionClient.y = GET_Y_LPARAM(lParam);
	m_inputIsClicked = true;
}

void Podo::InputMouseLeftButtonUp(WPARAM wParam, LPARAM lParam)
{
	std::wstring message = std::format(
		L"{}{} 시작 (x, y) = ({}, {}) \n 끝 (x, y) = ({}, {})",
		wParam & MK_CONTROL ? L"[CTRL]" : L"",
		wParam & MK_SHIFT ? L"[SHIFT]" : L"",
		m_inputMouseClickedPositionClient.x,
		m_inputMouseClickedPositionClient.y,
		GET_X_LPARAM(lParam),
		GET_Y_LPARAM(lParam)
	);

	unsigned int mouseMovedManhattanDist = abs(GET_X_LPARAM(lParam) - m_inputMouseClickedPositionClient.x) + abs(GET_Y_LPARAM(lParam) - m_inputMouseClickedPositionClient.y);
	bool isDrag = (mouseMovedManhattanDist > m_inputDragThresholdDist);
	MessageBoxW(m_hWnd, message.c_str(), isDrag ? L"좌측 마우스 드래그" : L"좌측 마우스 클릭", MB_OK);

	m_inputIsClicked = false;
}

void Podo::InputMouseRightButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (m_inputIsClicked == true)
	{
		return;
	}

	m_inputMouseClickedPositionClient.x = GET_X_LPARAM(lParam);
	m_inputMouseClickedPositionClient.y = GET_Y_LPARAM(lParam);
	m_inputIsClicked = true;
}

void Podo::InputMouseRightButtonUp(WPARAM wParam, LPARAM lParam)
{
	std::wstring message = std::format(
		L"{}{} 시작 (x, y) = ({}, {}) \n 끝 (x, y) = ({}, {})",
		wParam & MK_CONTROL ? L"[CTRL]" : L"",
		wParam & MK_SHIFT ? L"[SHIFT]" : L"",
		m_inputMouseClickedPositionClient.x,
		m_inputMouseClickedPositionClient.y,
		GET_X_LPARAM(lParam),
		GET_Y_LPARAM(lParam)
	);

	unsigned int mouseMovedManhattanDist = abs(GET_X_LPARAM(lParam) - m_inputMouseClickedPositionClient.x) + abs(GET_Y_LPARAM(lParam) - m_inputMouseClickedPositionClient.y);
	bool isDrag = (mouseMovedManhattanDist > m_inputDragThresholdDist);
	MessageBoxW(m_hWnd, message.c_str(), isDrag ? L"우측 마우스 드래그" : L"우측 마우스 클릭", MB_OK);

	m_inputIsClicked = false;
}

void Podo::InputMouseWheelScroll(WPARAM wParam, LPARAM lParam)
{
	m_inputScrollDelta += GET_WHEEL_DELTA_WPARAM(wParam);
}

void Podo::InputKeyboardDown(WPARAM wParam, LPARAM lParam)
{
	//if (wParam == VK_ESCAPE)
	//{
	//	
	//}
}