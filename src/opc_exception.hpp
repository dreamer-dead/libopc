/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_EXCEPTION_HPP
#define OPC_EXCEPTION_HPP

#include <stdexcept>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <comdef.h>

namespace opc 
{
	struct opc_exception : public std::exception 
	{
		opc_exception( HRESULT res );
		opc_exception( LPOLESTR message );
		opc_exception( HRESULT res, LPOLESTR message );

		virtual LPCOLESTR ole_what() const;
		HRESULT error_code() const { return this->result_; }

	private :
		std::basic_string< OLECHAR > message_;
		HRESULT result_;
	};
}

#endif //OPC_EXCEPTION_HPP
