#pragma once

struct OptionFullScreen
{
	bool userEnabled = false;

	bool IsActive() const
	{
		return IsSupported() && userEnabled;
	}

	bool IsSupported() const
	{
		return true;
	}

	void DebugPrint() const
	{
#ifdef _DEBUG
		OutputDebugStringW(userEnabled ? L"[PODO DEBUG] FullScreen: 유저 활성화 On\n" : L"[PODO DEBUG] FullScreen: 유저 활성화 off\n");
		OutputDebugStringW(IsActive() ? L"[PODO DEBUG] => FullScreen On \n" : L"[PODO DEBUG] => FullScreen Off \n");
		OutputDebugStringW(L"\n");
#endif
	}
};

struct OptionVSync
{
	bool userEnabled = false;

	bool IsActive() const
	{
		return IsSupported() && userEnabled;
	}

	bool IsSupported() const
	{
		return true;
	}

	void DebugPrint() const
	{
#ifdef _DEBUG
		OutputDebugStringW(userEnabled ? L"[PODO DEBUG] VSync: 유저 활성화 On\n" : L"[PODO DEBUG] VSync: 유저 활성화 off\n");
		OutputDebugStringW(IsActive() ? L"[PODO DEBUG] => VSync On \n" : L"[PODO DEBUG] => VSync Off \n");
		OutputDebugStringW(L"\n");
#endif
	}
};

struct OptionTearing
{
	bool featureSupported = false;

	bool IsActive() const
	{
		return IsSupported();
	}

	bool IsSupported() const
	{
		return featureSupported;
	}

	void DebugPrint() const
	{
#ifdef _DEBUG
		OutputDebugStringW(featureSupported ? L"[PODO DEBUG] Tearing: 피처 지원 On\n" : L"[PODO DEBUG] Tearing: 피처 지원 off\n");
		OutputDebugStringW(IsActive() ? L"[PODO DEBUG] => Tearing On \n" : L"[PODO DEBUG] => Tearing Off \n");
		OutputDebugStringW(L"\n");
#endif
	}
};

struct OptionRayTracing
{
	bool deviceSupported		= false;
	bool featureSupported		= false;
	bool commandListSupported	= false;
	bool userEnabled			= false;

	bool IsActive() const
	{
		return IsSupported() && userEnabled;
	}

	bool IsSupported() const
	{
		return deviceSupported && featureSupported && commandListSupported;
	}

	void DebugPrint() const
	{
#ifdef _DEBUG
		OutputDebugStringW(deviceSupported		? L"[PODO DEBUG] RayTracing: 디바이스 지원 On\n"		: L"[PODO DEBUG] RayTracing: 디바이스 지원 off\n");
		OutputDebugStringW(featureSupported		? L"[PODO DEBUG] RayTracing: 피처 지원 On\n"			: L"[PODO DEBUG] RayTracing: 피처 지원 off\n");
		OutputDebugStringW(commandListSupported	? L"[PODO DEBUG] RayTracing: 커맨드 리스트 지원 On\n"	: L"[PODO DEBUG] RayTracing: 커맨드 리스트 지원 off\n");
		OutputDebugStringW(userEnabled			? L"[PODO DEBUG] RayTracing: 유저 활성화 On\n"			: L"[PODO DEBUG] RayTracing: 유저 활성화 off\n");
		OutputDebugStringW(IsActive()			? L"[PODO DEBUG] => RayTracing On \n"					: L"[PODO DEBUG] => RayTracing Off \n");
		OutputDebugStringW(L"\n");
#endif
	}
};

struct OptionHDR
{
	bool outputSupported = false;
	bool formatSupported = false;
	bool colorSpaceSupported = false;
	bool userEnabled = false;

	bool IsActive() const
	{
		return IsSupported() && userEnabled;
	}

	bool IsSupported() const
	{
		return outputSupported && formatSupported && colorSpaceSupported;
	}

	void DebugPrint() const
	{
#ifdef _DEBUG
		OutputDebugStringW(outputSupported ? L"[PODO DEBUG] HDR: 아웃풋 지원 On\n" : L"[PODO DEBUG] HDR: 아웃풋 지원 off\n");
		OutputDebugStringW(formatSupported ? L"[PODO DEBUG] HDR: 포맷 지원 On\n" : L"[PODO DEBUG] HDR: 포맷 지원 off\n");
		OutputDebugStringW(colorSpaceSupported ? L"[PODO DEBUG] HDR: 색 공간 지원 On\n" : L"[PODO DEBUG] HDR: 색 공간 지원 off\n");
		OutputDebugStringW(userEnabled ? L"[PODO DEBUG] HDR: 유저 활성화 On\n" : L"[PODO DEBUG] HDR: 유저 활성화 off\n");
		OutputDebugStringW(IsActive() ? L"[PODO DEBUG] => HDR On \n" : L"[PODO DEBUG] => HDR Off \n");
		OutputDebugStringW(L"\n");
#endif
	}
};

struct OptionMeshShader
{
	bool deviceSupported		= false;
	bool featureSupported		= false;
	bool commandListSupported	= false;
	bool userEnabled			= false;

	bool IsActive() const
	{
		return IsSupported() && userEnabled;
	}

	bool IsSupported() const
	{
		return deviceSupported && featureSupported && commandListSupported;
	}

	void DebugPrint() const
	{
#ifdef _DEBUG
		OutputDebugStringW(deviceSupported		? L"[PODO DEBUG] MeshShader: 디바이스 지원 On\n"		: L"[PODO DEBUG] MeshShader: 디바이스 지원 off\n");
		OutputDebugStringW(featureSupported		? L"[PODO DEBUG] MeshShader: 피처 지원 On\n"			: L"[PODO DEBUG] MeshShader: 피처 지원 off\n");
		OutputDebugStringW(commandListSupported	? L"[PODO DEBUG] MeshShader: 커맨드 리스트 지원 On\n"	: L"[PODO DEBUG] MeshShader: 커맨드 리스트 지원 off\n");
		OutputDebugStringW(userEnabled			? L"[PODO DEBUG] MeshShader: 유저 활성화 On\n"			: L"[PODO DEBUG] MeshShader: 유저 활성화 off\n");
		OutputDebugStringW(IsActive()			? L"[PODO DEBUG] => MeshShader On \n"					: L"[PODO DEBUG] => MeshShader Off \n");
		OutputDebugStringW(L"\n");
#endif
	}
};

struct OptionGUI
{
	int masterSize = 100;

	bool IsActive() const
	{
		return IsSupported();
	}

	bool IsSupported() const
	{
		return true;
	}

	float GetMasterScale() const
	{
		return static_cast<float>(masterSize) / 100;
	}

	void DebugPrint() const
	{
#ifdef _DEBUG
		std::wstring masterSizeString = std::format(L"[PODO DEBUG] GUI: 마스터 사이즈 {}%\n", masterSize);
		OutputDebugStringW(masterSizeString.c_str());
		OutputDebugStringW(IsActive() ? L"[PODO DEBUG] => GUI On \n" : L"[PODO DEBUG] => GUI Off \n");
		OutputDebugStringW(L"\n");
#endif
	}
};