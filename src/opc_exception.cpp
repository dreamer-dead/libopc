#include "opc_exception.hpp"

namespace
{
	static void format_error( HRESULT res, std::basic_string<OLECHAR>& dst )
	{
		WCHAR buffer[MAX_PATH] = { 0 };
		wsprintfW( buffer, OLESTR("HRESULT = 0x%X"), res );

		dst = buffer;
	}
}

namespace opc
{
	opc_exception::opc_exception( HRESULT res, LPOLESTR message )
		: std::exception( "opc_exception" )
		, result_( res )
	{
		format_error( res, message_ );
		message_ += OLESTR("\n");
		message_ += message;
	}

	opc_exception::opc_exception( HRESULT res )
		: std::exception( "opc_exception" )
		, result_( res )
	{
		format_error( res, message_ );
	}

	opc_exception::opc_exception( LPOLESTR message )
		: std::exception( "opc_exception" )
		, message_( message )
		, result_( E_FAIL )
	{
	}

	LPCOLESTR opc_exception::ole_what() const
	{
		return message_.c_str();
	}
}
