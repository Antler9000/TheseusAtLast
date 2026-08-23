#pragma once
#define NOMINMAX
#include <windows.h>
#include <format>
#include <string>
#include <source_location>
#include <stdexcept>

class PodoException : public std::runtime_error
{
public:

	template <typename ResultType>
	PodoException(std::source_location location, std::string statement, ResultType result)
		: std::runtime_error(ErrorString(location, statement, ResultString(result)))
	{

	}

private:

	static std::string ErrorString(const std::source_location& location, std::string statement, std::string result)
	{
		return std::format
		(
			"file: {}\n\n"
			"function: {}\n\n"
			"line: {}\n\n"
			"statement: {}\n\n"
			"pResult: {}",
			location.file_name(),
			location.function_name(),
			location.line(),
			statement,
			result
		);
	}

	static std::string ResultString(HRESULT result)
	{
		return std::format("0x{:X}", static_cast<UINT32>(result));
	}

	static std::string ResultString(bool result)
	{
		return result ? "true" : "false";
	}

	static std::string ResultString(void* pResult)
	{
		return std::format("{:p}", static_cast<const void*>(pResult));
	}
};

#define ThrowIfFailed(statement)\
{\
	HRESULT hResult = (statement);\
	if (FAILED(hResult) == true)\
	{\
		throw PodoException(std::source_location::current(), #statement, hResult);\
	}\
}

#define ThrowIfFalse(statement)\
{\
	bool bResult = (statement);\
	if (bResult == false)\
	{\
		throw PodoException(std::source_location::current(), #statement, bResult);\
	}\
}

#define ThrowIfNull(statement)\
{\
	void* pResult = (statement);\
	if(pResult == nullptr)\
	{\
		throw PodoException(std::source_location::current(), #statement, pResult);\
	}\
}

//NOTE:		메모리 누수로 이어진 new 할당을 추적하기 위해 오버로딩된 new를 디버깅 과정에서 사용할 수 있도록 함
//			이를 명시적으로 사용해야 하므로, make_unique 대신 unique_ptr<T>(DBG_NEW T(argument))를 사용해야 함
#ifdef _DEBUG
	#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
	#define DBG_NEW new
#endif
