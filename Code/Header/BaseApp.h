#pragma once
#define NOMINMAX
#include "Debug.h"
#include <windows.h>

//NOTE: WindowProc에서 생성할 pThis 객체의 클래스가 자식 클래스일 수 있도록 템플릿 타입 매개변수를 사용함
template <class DerievedApp>
class BaseApp
{
protected:

	BaseApp(const wchar_t* pAppName, HINSTANCE hInstance, int nCmdShow) : m_pAppName(pAppName), m_hInstance(hInstance)
	{
		InitWindow(nCmdShow);
	}

	~BaseApp()
	{

	}

	//NOTE: 앱의 기본 생성자, 복사, 이동을 허용하지 않도록 함
	BaseApp() = delete;
	BaseApp(const BaseApp& sourceApp) = delete;
	BaseApp(BaseApp&& sourceApp) noexcept = delete;
	BaseApp& operator = (const BaseApp& sourceApp) = delete;
	BaseApp& operator = (BaseApp&& sourceApp) = delete;

protected:

	const wchar_t*		m_pAppName		= nullptr;

	HINSTANCE			m_hInstance		= nullptr;
	HWND				m_hWnd			= nullptr;

private:

	//NOTE:	static 메서드는 객체에 얽힌 메서드가 아니기에 this가 없으므로, 멤버 변수에 접근할 수가 없다는 문제가 생김
	//			이를 해결하기 위해 CreatWindowEx의 마지막 매개변수, SetWindowLongPtr 함수, GetWindowLongPtr 함수를 이용함
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		DerievedApp* pThis = nullptr;

		if (uMsg == WM_CREATE)
		{
			CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis = reinterpret_cast<DerievedApp*>(pCreateStruct->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
		}
		else
		{
			LONG_PTR pUserData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
			pThis = reinterpret_cast<DerievedApp*>(pUserData);
		}

		if (pThis != nullptr)
		{
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

private:

	void InitWindow(int show)
	{
		ThrowIfFalse(m_bNotCreated);

		m_bNotCreated = false;

		const wchar_t* windowClassName = L"Window Class";

		WNDCLASS wc = { };
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = BaseApp::WindowProcedure;
		wc.hInstance = m_hInstance;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszClassName = windowClassName;

		ThrowIfFalse(RegisterClass(&wc));

		m_hWnd = CreateWindowEx(
			0,
			windowClassName,
			m_pAppName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			m_hInstance,
			(LPVOID)this	//NOTE: WindowProcedure에서 이 객체에 접근할 수 있도록 하기 위한 단계 중 하나임
		);

		ThrowIfNull(m_hWnd);
	}

private:

	inline static bool	m_bNotCreated = true;
};