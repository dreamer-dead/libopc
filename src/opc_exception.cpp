//#include <sstream>

//#include "utils/encode.hpp"
#include "opc_exception.hpp"

namespace opc
{
	static void format_error( HRESULT res, std::basic_string<OLECHAR>& dst )
	{
		WCHAR buffer[MAX_PATH] = {0};
		wsprintfW( buffer, OLESTR("HRESULT = 0x%X"), res );

		dst = buffer;
	}

	/*
	template < typename CharT >
	static std::basic_string<CharT> format_error( HRESULT res )
	{
		std::basic_stringstream< CharT > stream;
		stream << std::uppercase << std::hex << res;

		return stream.str();
	}

	template <>
	static std::basic_string<char> format_error( HRESULT res )
	{
		std::basic_stringstream< char > stream;
		stream << "HRESULT = 0x" << std::uppercase << std::hex << res;

		return stream.str();
	}

	template <>
	static std::basic_string<WCHAR> format_error( HRESULT res )
	{
		std::basic_stringstream< WCHAR > stream;
		stream << L"HRESULT = 0x" << std::uppercase << std::hex << res;

		return stream.str();
	}
	//*/

	opc_exception::opc_exception( HRESULT res, LPOLESTR message )
		: std::exception( 
				""
				//utils::encode<char>( 
				//	format_error<OLECHAR>(res) + L"\n" + std::basic_string<OLECHAR>(message) 
				//	).c_str() 
				)
		, result_( res )
		//, message_( format_error<OLECHAR>(res) )
	{
		format_error( res, message_ );
		message_ += OLESTR("\n");
		message_ += message;
	}

	opc_exception::opc_exception( HRESULT res )
		: std::exception( 
			"" //format_error<char>(res).c_str() 
		)
		, result_( res )
		//, message_( format_error<OLECHAR>(res) )
	{
		format_error( res, message_ );
	}

	opc_exception::opc_exception( LPOLESTR message )
		: std::exception( 
				""
				//utils::encode<char>( 
				//	std::basic_string<OLECHAR>(message) 
				//).c_str() 
			)
		, message_( message )
		, result_( E_FAIL )
	{
	}

	LPCOLESTR opc_exception::ole_what() const
	{
		return message_.c_str();
	}
}
