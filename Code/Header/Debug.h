#pragma once
#include <windows.h>
#include <format>
#include <string>
#include <source_location>
#include <stdexcept>

class PodoException : public std::runtime_error
{
public:

	template <typename ResultType>
	PodoException(ResultType result, std::string statement, std::source_location location)
		: std::runtime_error(ErrorString(ResultString(result), statement, location))
	{

	}

private:

	static std::string ErrorString(std::string result, std::string statement, const std::source_location& location)
	{
		return std::format(
			"pResult: {}\n\n"
			"statement: {}\n\n"
			"function: {}\n\n"
			"file: {}\n\n"
			"line: {}",
			result,
			statement,
			location.function_name(),
			location.file_name(),
			location.line()
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
		return std::format("0x{:p}", static_cast<const void*>(pResult));
	}
};

#define ThrowIfFailed(statement)\
{\
	HRESULT pResult = (statement);\
	if (FAILED(pResult) == true)\
	{\
		throw PodoException(pResult, #statement, std::source_location::current());\
	}\
}

#define ThrowIfFalse(statement)\
{\
	bool pResult = (statement);\
	if (pResult == false)\
	{\
		throw PodoException(pResult, #statement, std::source_location::current());\
	}\
}

#define ThrowIfNull(statement)\
{\
	void* pResult = (statement);\
	if(pResult == nullptr)\
	{\
		throw PodoException(pResult, #statement, std::source_location::current());\
	}\
}

//NOTE:		메모리 누수로 이어진 new 할당을 추적하기 위해 오버로딩된 new를 디버깅 과정에서 사용할 수 있도록 함
//			이를 명시적으로 사용해야 하므로, make_unique 대신 unique_ptr<T>(DBG_NEW T(argument))를 사용해야 함
#ifdef _DEBUG
	#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
	#define DBG_NEW new
#endif
